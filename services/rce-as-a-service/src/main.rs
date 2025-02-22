mod auth;
mod misc;
mod routes;
mod sandbox;
mod storage;

use crate::auth::Authenticator;
use crate::routes::{execute, login};
use crate::sandbox::Sandbox;
use log::info;
use rand::TryRngCore;
use rocket::config::SecretKey;
use rocket::data::{Limits, ToByteUnit};
use rocket::{launch, routes};
use simplelog::{ColorChoice, CombinedLogger, Config, LevelFilter, TermLogger, TerminalMode};
use std::net::{IpAddr, Ipv4Addr};
use std::path::PathBuf;
use std::sync::Arc;
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

async fn get_components() -> (Arc<Authenticator>, Sandbox) {
    let user_data_path = std::env::var("USER_DATA_PATH").expect("USER_DATA_PATH should be set");
    let path = PathBuf::from(user_data_path);
    assert!(path.exists(), "USER_DATA_PATH should be a valid path");
    let storage = Arc::new(Storage::new(path));

    let sandbox = Sandbox::new(storage.clone()).unwrap();

    let redis_url = std::env::var("REDIS_URL").expect("REDIS_URL should be set");
    let auth = Arc::new(Authenticator::new(&redis_url).await.unwrap());

    (auth, sandbox)
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

    let (auth, sandbox) = get_components().await;

    info!("Starting server at {}:{}", config.address, config.port);
    rocket::build()
        .manage(sandbox)
        .manage(auth)
        .configure(config)
        .mount("/api", routes![login, execute])
}
