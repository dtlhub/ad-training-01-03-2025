mod auth;
mod cleaner;
mod misc;
mod sandbox;
mod storage;

use crate::auth::Authenticator;
use crate::sandbox::ExecutionResult;
use crate::sandbox::Sandbox;
use base64::prelude::BASE64_STANDARD;
use base64::Engine as _;
use cleaner::Cleaner;
use rocket::http::{Cookie, CookieJar, Status};
use rocket::request::{FromRequest, Outcome};
use rocket::response::status;
use rocket::serde::{json::Json, Deserialize};
use rocket::{launch, post, routes, Request, State};
use std::net::{IpAddr, Ipv4Addr};
use std::sync::Arc;
use std::time::Duration;
use storage::Storage;

const COOKIE_NAME: &str = "username";

struct AuthenticatedUser(pub String);

#[rocket::async_trait]
impl<'r> FromRequest<'r> for AuthenticatedUser {
    type Error = String;

    async fn from_request(request: &'r Request<'_>) -> Outcome<Self, Self::Error> {
        let respond_with = |status: Status| -> Outcome<Self, Self::Error> {
            Outcome::Error((status, status.reason_lossy().to_string()))
        };

        let cookies = request.cookies();
        let username = match cookies.get_private(COOKIE_NAME) {
            Some(cookie) => cookie.value().to_string(),
            None => {
                return respond_with(Status::Unauthorized);
            }
        };

        let auth = match request
            .guard::<&State<Arc<Authenticator>>>()
            .await
            .succeeded()
        {
            Some(auth) => auth.inner(),
            None => {
                return respond_with(Status::ServiceUnavailable);
            }
        };

        let exists = auth.exists(&username).await;
        match exists {
            Ok(exists) => {
                if !exists {
                    respond_with(Status::Unauthorized)
                } else {
                    Outcome::Success(AuthenticatedUser(username))
                }
            }
            Err(_) => respond_with(Status::ServiceUnavailable),
        }
    }
}

#[derive(Deserialize)]
#[serde(crate = "rocket::serde")]
struct ExecutionRequest<'r> {
    wasm: &'r str,
    args: Vec<String>,
}

#[post("/execute", format = "plain", data = "<data>")]
async fn execute(
    data: Json<ExecutionRequest<'_>>,
    authenticated_user: AuthenticatedUser,
    sandbox: &State<Sandbox>,
) -> std::result::Result<Json<ExecutionResult>, status::BadRequest<String>> {
    let base64_wasm = data.wasm.as_bytes();
    let wasm = BASE64_STANDARD
        .decode(base64_wasm)
        .map_err(|e| status::BadRequest(e.to_string()))?;

    let result = sandbox
        .execute_sandboxed(&authenticated_user.0, &wasm, &data.args)
        .await
        .map_err(|e| status::BadRequest(e.to_string()))?;

    Ok(Json(result))
}

#[derive(Deserialize)]
#[serde(crate = "rocket::serde")]
struct LoginData<'r> {
    username: &'r str,
    password: &'r str,
}

#[post("/login", format = "json", data = "<data>")]
async fn login(
    data: Json<LoginData<'_>>,
    auth: &State<Arc<Authenticator>>,
    cookies: &CookieJar<'_>,
) -> std::result::Result<String, status::Forbidden<String>> {
    let authenticated = auth
        .authenticate(&data.username, &data.password)
        .await
        .map_err(|e| status::Forbidden(e.to_string()))?;

    if !authenticated {
        return Err(status::Forbidden(
            "Invalid username or password".to_string(),
        ));
    }

    let cookie = Cookie::new(COOKIE_NAME, data.username.to_string());
    cookies.add_private(cookie);

    Ok("".to_string())
}

async fn get_components() -> (Arc<Storage>, Arc<Authenticator>, Sandbox) {
    let s3_url = std::env::var("S3_URL").expect("S3_URL is set");
    let storage = Arc::new(Storage::new(s3_url));
    let sandbox = Sandbox::new(storage.clone()).unwrap();

    let db_url = std::env::var("DB_URL").expect("DB_URL is set");
    let auth = Arc::new(Authenticator::new(&db_url).await.unwrap());

    (storage, auth, sandbox)
}

async fn launch_cleaner(auth: Arc<Authenticator>, storage: Arc<Storage>) {
    let cleaner_interval = std::env::var("CLEANER_INTERVAL_MINS")
        .unwrap_or("15".to_string())
        .parse::<u64>()
        .expect("CLEANER_INTERVAL_MINS is set to a valid integer");
    let user_lifetime = std::env::var("USER_LIFETIME_MINS")
        .unwrap_or("10".to_string())
        .parse::<u64>()
        .expect("USER_LIFETIME_MINS is set to a valid integer");

    let cleaner = Cleaner::new(auth.clone(), storage.clone(), cleaner_interval)
        .await
        .unwrap();
    tokio::spawn(async move {
        loop {
            cleaner.clean().await.unwrap(); // TODO: handle all errors
            tokio::time::sleep(Duration::from_secs(
                (60 * user_lifetime).try_into().unwrap(),
            ))
            .await;
        }
    });
}

#[launch]
async fn rocket() -> _ {
    let mut config = rocket::Config::default();
    config.address = IpAddr::V4(Ipv4Addr::new(0, 0, 0, 0));
    config.port = 9091;

    let (storage, auth, sandbox) = get_components().await;
    launch_cleaner(auth.clone(), storage.clone()).await;
    rocket::build()
        .manage(sandbox)
        .manage(auth)
        .configure(config)
        .mount("/api", routes![login, execute])
}
