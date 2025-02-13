use crate::misc::Result;
use sqlx::postgres::{PgPool, PgPoolOptions};
use std::time::Duration;

pub struct Authenticator {
    pub pool: PgPool,
}

impl Authenticator {
    pub async fn new(pg_conn_str: &str) -> Result<Self> {
        let pool = PgPoolOptions::new()
            .max_connections(10)
            .idle_timeout(Duration::from_secs(60))
            .acquire_timeout(Duration::from_secs(5))
            .connect(pg_conn_str)
            .await?;

        Ok(Self { pool })
    }

    pub async fn authenticate(&self, username: &str, password: &str) -> Result<bool> {
        let mut tx = self.pool.begin().await?;

        let current_password =
            sqlx::query_scalar!("SELECT password FROM users WHERE username = $1", username)
                .fetch_optional(&mut *tx)
                .await?;

        match current_password {
            Some(value) => Ok(value.eq(password)),
            None => {
                sqlx::query!(
                    "INSERT INTO users (username, password) VALUES ($1, $2)",
                    username,
                    password
                )
                .execute(&mut *tx)
                .await?;

                tx.commit().await?;
                Ok(true)
            }
        }
    }

    pub async fn exists(&self, username: &str) -> Result<bool> {
        let user = sqlx::query_scalar!("SELECT username FROM users WHERE username = $1", username)
            .fetch_optional(&self.pool)
            .await?;

        Ok(user.is_some())
    }

    pub async fn remove_old_users(&self, minutes: u64) -> Result<Vec<String>> {
        let users = sqlx::query_scalar!(
            "DELETE FROM users WHERE created_at < NOW() - ($1 || ' minutes')::interval RETURNING username",
            minutes.to_string()
        )
        .fetch_all(&self.pool)
        .await?;

        Ok(users)
    }
}
