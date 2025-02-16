use rocket::data::ToByteUnit;
use s3::creds::Credentials;
use s3::{Bucket, BucketConfiguration, Region};
use std::path::PathBuf;
use tmpdir::TmpDir;
use tokio::io::{AsyncReadExt, BufReader};

use crate::misc::Result;
pub struct Storage {
    credentials: Credentials,
    region: Region,
    bucket_configuration: BucketConfiguration,
}

impl Storage {
    pub fn new(s3_url: String) -> Self {
        let credentials = Credentials::anonymous().unwrap();
        let region = Region::Custom {
            region: "eu-central-1".to_owned(),
            endpoint: s3_url,
        };
        Self {
            credentials,
            region,
            bucket_configuration: BucketConfiguration::public(),
        }
    }

    pub async fn setup_user_directory(&self, user: &str) -> Result<Directory> {
        let mut bucket = Bucket::new(user, self.region.clone(), self.credentials.clone())?;
        if !bucket.exists().await? {
            bucket = Bucket::create_with_path_style(
                user,
                self.region.clone(),
                self.credentials.clone(),
                self.bucket_configuration.clone(),
            )
            .await?
            .bucket;
        }

        let file_limit = 10;
        let bytes_limit = 10.mebibytes().as_u64();

        Ok(Directory::new(user, bucket, file_limit, bytes_limit).await?)
    }

    pub async fn delete_user(&self, user: &str) -> Result<()> {
        let bucket = Bucket::new(user, self.region.clone(), self.credentials.clone())?;
        if bucket.exists().await? {
            bucket.delete().await?;
        }
        Ok(())
    }
}

pub struct Directory {
    bucket: Box<Bucket>,
    dir: TmpDir,
    bytes_quota: u64,
    files_quota: u64,
}

impl Directory {
    pub async fn new(
        prefix: &str,
        bucket: Box<Bucket>,
        files_quota: u64,
        bytes_quota: u64,
    ) -> Result<Self> {
        let instance = Self {
            bucket,
            dir: TmpDir::new(prefix).await?,
            bytes_quota,
            files_quota,
        };
        instance.move_from_s3_to_local().await?;
        Ok(instance)
    }

    pub fn path(&self) -> PathBuf {
        self.dir.to_path_buf()
    }

    pub async fn close(&mut self) -> Result<()> {
        self.move_from_local_to_s3().await?;
        self.dir.close().await?;
        Ok(())
    }

    async fn move_from_s3_to_local(&self) -> Result<()> {
        let list_bucket_results = self
            .bucket
            .list("/".to_string(), Some("/".to_string()))
            .await?;

        let mut files_left = self.files_quota;
        let mut bytes_left = self.bytes_quota;
        for list_bucket_result in list_bucket_results {
            for content in list_bucket_result.contents {
                if files_left == 0 || bytes_left < content.size {
                    break;
                }

                let filename = content.key.as_str().split("/").last().unwrap();
                let mut file = tokio::fs::File::create(self.path().join(filename)).await?;
                self.bucket
                    .get_object_range_to_writer(filename, 0, Some(content.size), &mut file)
                    .await?;

                files_left -= 1;
                bytes_left -= content.size;

                // TODO: don't delete in case of failure of running the script?
                self.bucket.delete_object(filename).await?;
            }
        }

        Ok(())
    }

    async fn move_from_local_to_s3(&self) -> Result<()> {
        let mut dir = tokio::fs::read_dir(self.path()).await?;

        let mut files_left = self.files_quota;
        let mut bytes_left = self.bytes_quota;
        while let Some(entry) = dir.next_entry().await? {
            if files_left == 0 || bytes_left == 0 {
                break;
            }

            if entry.file_type().await?.is_file() {
                let file = tokio::fs::File::open(entry.path()).await?;
                let reader = BufReader::new(file);
                let mut buf = Vec::with_capacity(bytes_left as usize);

                let n = reader.take(bytes_left as u64).read_to_end(&mut buf).await?;
                if n == 0 {
                    continue;
                }
                bytes_left -= n as u64;
                files_left -= 1;

                self.bucket
                    .put_object(entry.path().to_str().unwrap(), &buf)
                    .await?;
            }
        }

        Ok(())
    }
}
