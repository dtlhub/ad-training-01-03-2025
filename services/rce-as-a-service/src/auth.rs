use crate::misc::{Error, Result};
use redis::{AsyncCommands, Client};
use rocket::http::Status;

pub struct Authenticator {
    client: Client,
}

impl Authenticator {
    pub async fn new(redis_conn_str: &str) -> Result<Self> {
        let client = Client::open(redis_conn_str)?;
        Ok(Self { client })
    }

    fn is_valid_username(username: &str) -> bool {
        username
            .chars()
            .all(|c| c.is_ascii_alphanumeric() || c == '_' || c == '-')
    }

    fn user_password_key(username: &str) -> String {
        format!("user:{}:password", username)
    }

    pub async fn authenticate(&self, username: &str, password: &str) -> Result<()> {
        if !Self::is_valid_username(username) {
            return Err(Error::new(Status::BadRequest, "invalid username"));
        }

        let mut conn = self.client.get_multiplexed_async_connection().await?;

        let (current_password,): (String,) = redis::pipe()
            .atomic()
            .set_nx(Self::user_password_key(username), password)
            .ignore()
            .get(Self::user_password_key(username))
            .query_async(&mut conn)
            .await?;

        match current_password.eq(password) {
            true => Ok(()),
            false => Err(Error::new(Status::Forbidden, "bad credentials")),
        }
    }

    pub async fn exists(&self, username: &str) -> Result<bool> {
        let mut conn = self.client.get_multiplexed_async_connection().await?;
        let result = conn.exists(Self::user_password_key(username)).await?;
        Ok(result)
    }
}
