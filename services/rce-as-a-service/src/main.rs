mod misc;
mod sandbox;
mod storage;

use std::net::{IpAddr, Ipv4Addr};

use rocket::data::{Data, ToByteUnit};
use rocket::{launch, post, routes};

use crate::sandbox::Sandbox;

#[post("/debug", format = "plain", data = "<data>")]
async fn debug(data: Data<'_>) {
    let sandbox = Sandbox::new("http://localhost:8333".to_string()).unwrap();
    println!("Sandbox created");

    let bytes = data.open(1.mebibytes()).into_bytes().await.unwrap();
    println!("Bytes read");
    let _ = sandbox
        .execute_sandboxed(&"mybucket".to_string(), &bytes, &["echo", "hello"])
        .await
        .unwrap();
    println!("Sandbox executed");
}

#[launch]
fn rocket() -> _ {
    let mut config = rocket::Config::default();
    config.address = IpAddr::V4(Ipv4Addr::new(0, 0, 0, 0));
    config.port = 9091;

    rocket::build()
        .configure(config)
        .mount("/api", routes![debug])
}
