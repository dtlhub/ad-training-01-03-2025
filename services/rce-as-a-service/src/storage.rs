use crate::misc::Result;
use rocket::data::ToByteUnit;
use std::path::PathBuf;

pub struct Storage {
    base_path: PathBuf,
}

impl Storage {
    pub fn new(base_path: PathBuf) -> Self {
        Self { base_path }
    }

    pub async fn setup_user_directory(&self, user: &str) -> Result<Directory> {
        log::info!("Setting up directory for {}", user);

        let path = self.base_path.join(user);
        if !path.exists() {
            tokio::fs::create_dir_all(&path).await?;
        }

        let file_limit = 10;
        let bytes_limit = 10.mebibytes().as_u64();

        let dir = Directory::new(path, file_limit, bytes_limit).await?;
        Ok(dir)
    }
}

pub struct Directory {
    path: PathBuf,
    bytes_quota: u64,
    files_quota: u64,
}

impl Directory {
    pub async fn new(path: PathBuf, files_quota: u64, bytes_quota: u64) -> Result<Self> {
        let instance = Self {
            path,
            bytes_quota,
            files_quota,
        };
        Ok(instance)
    }

    pub fn path(&self) -> &PathBuf {
        &self.path
    }

    pub async fn close(&mut self) -> Result<()> {
        self.enforce_quota().await?;
        Ok(())
    }

    async fn enforce_quota(&self) -> Result<()> {
        let mut dir = tokio::fs::read_dir(self.path()).await?;

        let mut files_left = self.files_quota;
        let mut bytes_left = self.bytes_quota;

        while let Some(entry) = dir.next_entry().await? {
            let file_type = entry.file_type().await?;
            if file_type.is_file() {
                let metadata = entry.metadata().await?;
                let size = metadata.len();

                if size > bytes_left || files_left == 0 {
                    log::info!(
                        "Deleting file exceeding quota (Size: {}, BytesLeft: {}, FilesLeft: {}): {}",
                        size,
                        bytes_left,
                        files_left,
                        entry.path().display()
                    );
                    tokio::fs::remove_file(entry.path()).await?;
                } else {
                    bytes_left -= size;
                    files_left -= 1;
                }
            } else if file_type.is_dir() {
                log::info!("Deleting directory: {}", entry.path().display());
                tokio::fs::remove_dir_all(entry.path()).await?;
            } else {
                log::info!("Deleting unknown entry: {}", entry.path().display());
                tokio::fs::remove_file(entry.path()).await?;
            }
        }
        Ok(())
    }
}
