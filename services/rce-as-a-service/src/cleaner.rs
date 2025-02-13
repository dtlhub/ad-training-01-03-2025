use crate::auth::Authenticator;
use crate::misc::Result;
use crate::storage::Storage;
use std::sync::Arc;
use tokio::sync::Semaphore;

pub struct Cleaner {
    storage: Arc<Storage>,
    auth: Arc<Authenticator>,
    minutes: u64,
}

impl Cleaner {
    pub async fn new(
        auth: Arc<Authenticator>,
        storage: Arc<Storage>,
        minutes: u64,
    ) -> Result<Self> {
        Ok(Self {
            auth,
            storage,
            minutes,
        })
    }

    pub async fn clean(&self) -> Result<()> {
        let users = self.auth.remove_old_users(self.minutes).await?;

        let mut handles = Vec::with_capacity(users.len());
        let semaphore = Arc::new(Semaphore::new(10));

        for user in users {
            let storage = Arc::clone(&self.storage);
            let semaphore = Arc::clone(&semaphore);
            handles.push(tokio::spawn(async move {
                let _guard = semaphore.acquire().await.unwrap();
                let _ = storage.delete_user(&user).await;
            }));
        }

        for handle in handles {
            handle.await?;
        }
        semaphore.close();

        Ok(())
    }
}
