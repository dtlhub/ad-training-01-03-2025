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
use log::{error, info};
use rand::TryRngCore;
use rocket::config::SecretKey;
use rocket::data::{Limits, ToByteUnit};
use rocket::{launch, routes};
use simplelog::{ColorChoice, CombinedLogger, Config, LevelFilter, TermLogger, TerminalMode};
use std::net::{IpAddr, Ipv4Addr};
use std::path::PathBuf;
use std::sync::Arc;
use std::time::Duration;
use storage::Storage;

fn setup_logging() {
    CombinedLogger::init(vec![TermLogger::new(
        LevelFilter::Info,
        Config::default(),
        TerminalMode::Mixed,
        ColorChoice::Auto,
    )])
    .expect("Logging should be set up");
}

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

    let sleep_duration = Duration::from_secs((60 * cleaner_interval).into());
    tokio::spawn(async move {
        loop {
            tokio::time::sleep(sleep_duration).await;
            info!("Cleaning up expired users");
            match cleaner.clean().await {
                Ok(count) => info!("Successfully cleaned up {count} users"),
                Err(e) => error!("Error cleaning up expired users: {e}"),
            }
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
    setup_logging();

    let mut config = rocket::Config::default();
    config.address = IpAddr::V4(Ipv4Addr::new(0, 0, 0, 0));
    config.port = 9091;
    config.secret_key = get_secret_key();
    config.limits = Limits::new().limit("json", 3.mebibytes());

    let (storage, auth, sandbox) = get_components().await;

    launch_cleaner(auth.clone(), storage.clone()).await;

    info!("Starting server at {}:{}", config.address, config.port);
    rocket::build()
        .manage(sandbox)
        .manage(auth)
        .configure(config)
        .mount("/api", routes![login, execute])
}

