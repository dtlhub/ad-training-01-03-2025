mod auth;
mod misc;
mod sandbox;
mod storage;

use std::net::{IpAddr, Ipv4Addr};

use rocket::data::{Data, ToByteUnit};
use rocket::{launch, post, routes, State};

use crate::auth::Authenticator;
use crate::sandbox::Sandbox;

#[post("/execute", format = "plain", data = "<data>")]
async fn execute(data: Data<'_>, sandbox: &State<Sandbox>) {
    // TODO: proper error handling
    let bytes = data.open(1.mebibytes()).into_bytes().await.unwrap();
    let _ = sandbox
        .execute_sandboxed(&"mybucket".to_string(), &bytes, &["echo", "hello"])
        .await
        .unwrap();
}

#[launch]
async fn rocket() -> _ {
    let mut config = rocket::Config::default();
    config.address = IpAddr::V4(Ipv4Addr::new(0, 0, 0, 0));
    config.port = 9091;

    let s3_url = std::env::var("S3_URL").expect("S3_URL is set");
    let sandbox = Sandbox::new(s3_url).unwrap();

    let db_url = std::env::var("DB_URL").expect("DB_URL is set");
    let auth = Authenticator::new(db_url).await.unwrap();

    rocket::build()
        .manage(sandbox)
        .manage(auth)
        .configure(config)
        .mount("/api", routes![execute])
}
