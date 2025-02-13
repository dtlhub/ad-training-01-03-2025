mod auth;
mod cleaner;
mod misc;
mod routes;
mod sandbox;
mod storage;

use crate::auth::Authenticator;
use crate::routes::{execute, login};
use crate::sandbox::Sandbox;
use cleaner::Cleaner;
use rand::TryRngCore;
use rocket::config::SecretKey;
use rocket::{launch, routes};
use std::net::{IpAddr, Ipv4Addr};
use std::path::PathBuf;
use std::sync::Arc;
use std::time::Duration;
use storage::Storage;

async fn get_components() -> (Arc<Storage>, Arc<Authenticator>, Sandbox) {
    let s3_url = std::env::var("S3_URL").expect("S3_URL should be set");
    let storage = Arc::new(Storage::new(s3_url));
    let sandbox = Sandbox::new(storage.clone()).unwrap();

    let db_url = std::env::var("DB_URL").expect("DB_URL should be set");
    let auth = Arc::new(Authenticator::new(&db_url).await.unwrap());

    (storage, auth, sandbox)
}

async fn launch_cleaner(auth: Arc<Authenticator>, storage: Arc<Storage>) {
    let cleaner_interval = std::env::var("CLEANER_INTERVAL_MINS")
        .unwrap_or("1".to_string())
        .parse::<u64>()
        .expect("CLEANER_INTERVAL_MINS should be set to a valid integer");
    let user_lifetime = std::env::var("USER_LIFETIME_MINS")
        .unwrap_or("20".to_string())
        .parse::<u64>()
        .expect("USER_LIFETIME_MINS should be set to a valid integer");

    let cleaner = Cleaner::new(auth.clone(), storage.clone(), user_lifetime)
        .await
        .expect("Cleaner should be initialized");
    tokio::spawn(async move {
        loop {
            tokio::time::sleep(Duration::from_secs(
                (60 * cleaner_interval).try_into().unwrap(),
            ))
            .await;
            cleaner.clean().await.unwrap(); // TODO: handle all errors
        }
    });
}

fn generate_new_key(path: PathBuf) -> SecretKey {
    let mut data = [0u8; 64];
    rand::rngs::OsRng
        .try_fill_bytes(&mut data)
        .expect("Should be able to generate a new key");
    std::fs::write(path, data).expect("Should be able to save a new key");
    SecretKey::from(data.as_slice())
}

fn get_secret_key() -> SecretKey {
    let secret_key_path = std::env::var("SECRET_KEY_PATH").expect("SECRET_KEY_PATH should be set");

    if !std::path::Path::new(&secret_key_path).exists() {
        return generate_new_key(secret_key_path.into());
    }

    let secret_key =
        std::fs::read(&secret_key_path).expect("SECRET_KEY_PATH should be a valid file");
    if secret_key.len() == 0 {
        return generate_new_key(secret_key_path.into());
    }

    SecretKey::from(secret_key.as_slice())
}

#[launch]
async fn rocket() -> _ {
    let mut config = rocket::Config::default();
    config.address = IpAddr::V4(Ipv4Addr::new(0, 0, 0, 0));
    config.port = 9091;
    config.secret_key = get_secret_key();

    let (storage, auth, sandbox) = get_components().await;

    launch_cleaner(auth.clone(), storage.clone()).await;

    rocket::build()
        .manage(sandbox)
        .manage(auth)
        .configure(config)
        .mount("/api", routes![login, execute])
}
