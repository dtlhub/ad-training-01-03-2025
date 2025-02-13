use std::time::Duration;

use sqlx::postgres::{PgPool, PgPoolOptions};

use crate::misc::Result;
pub struct Authenticator {
    pub pool: PgPool,
}

struct User {
    password: String,
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

        let user = sqlx::query_as!(
            User,
            "SELECT password FROM users WHERE username = $1",
            username
        )
        .fetch_optional(&mut *tx)
        .await?;

        match user {
            Some(user) => Ok(user.password.eq(password)),
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
        let user = sqlx::query_as!(
            User,
            "SELECT password FROM users WHERE username = $1",
            username
        )
        .fetch_optional(&self.pool)
        .await?;

        Ok(user.is_some())
    }
}
