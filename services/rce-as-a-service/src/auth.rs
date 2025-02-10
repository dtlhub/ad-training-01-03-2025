use crate::misc::Result;
use sqlx::postgres::{PgPool, PgPoolOptions};
use std::time::Duration;

pub struct Authenticator {
    pub pool: PgPool,
}

struct User {
    username: String,
    password: String,
}

impl Authenticator {
    pub async fn new(pg_conn_str: String) -> Result<Self> {
        let pool = PgPoolOptions::new()
            .max_connections(10)
            .idle_timeout(Duration::from_secs(60))
            .acquire_timeout(Duration::from_secs(5))
            .connect(&pg_conn_str)
            .await?;

        Ok(Self { pool })
    }

    pub async fn authenticate(&self, username: &String, password: &String) -> Result<bool> {
        let mut tx = self.pool.begin().await?;

        let user = sqlx::query_as!(User, "SELECT * FROM users WHERE username = $1", username)
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
}
