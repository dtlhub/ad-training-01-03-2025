use crate::auth::Authenticator;
use crate::misc::Error;
use crate::misc::ErrorResponse;
use crate::sandbox::ExecutionResult;
use crate::sandbox::Sandbox;
use base64::prelude::BASE64_STANDARD;
use base64::Engine as _;
use log::info;
use rocket::http::{Cookie, CookieJar, Status};
use rocket::request::{FromRequest, Outcome};
use rocket::serde::{json::Json, Deserialize};
use rocket::{post, Request, State};
use std::sync::Arc;

const COOKIE_NAME: &str = "username";

pub struct AuthenticatedUser(pub String);

#[rocket::async_trait]
impl<'r> FromRequest<'r> for AuthenticatedUser {
    type Error = ErrorResponse;

    async fn from_request(request: &'r Request<'_>) -> Outcome<Self, Self::Error> {
        let cookies = request.cookies();
        let username = match cookies.get_private(COOKIE_NAME) {
            Some(cookie) => cookie.value().to_string(),
            None => {
                return Error::new(Status::Unauthorized, "no username in cookie").into();
            }
        };

        let auth = match request
            .guard::<&State<Arc<Authenticator>>>()
            .await
            .succeeded()
        {
            Some(auth) => auth.inner(),
            None => {
                return Error::new(Status::ServiceUnavailable, "no authenticator").into();
            }
        };

        let exists = auth.exists(&username).await;
        match exists {
            Ok(true) => Outcome::Success(AuthenticatedUser(username)),
            Ok(false) => Error::new(Status::Forbidden, "user does not exist").into(),
            Err(e) => e.into(),
        }
    }
}

#[derive(Deserialize)]
#[serde(crate = "rocket::serde")]
pub struct ExecutionRequest<'r> {
    wasm: &'r str,
    args: Vec<String>,
}

#[post("/execute", format = "json", data = "<data>")]
pub async fn execute(
    data: Json<ExecutionRequest<'_>>,
    authenticated_user: AuthenticatedUser,
    sandbox: &State<Sandbox>,
) -> Result<Json<ExecutionResult>, ErrorResponse> {
    info!("Executing WASM from {}", authenticated_user.0);

    let base64_wasm = data.wasm.as_bytes();
    let wasm = BASE64_STANDARD
        .decode(base64_wasm)
        .map_err(|e| Error::new(Status::BadRequest, e.to_string()))?;

    let result = sandbox
        .execute_sandboxed(&authenticated_user.0, &wasm, &data.args)
        .await?;

    Ok(Json(result))
}

#[derive(Deserialize)]
#[serde(crate = "rocket::serde")]
pub struct LoginData<'r> {
    username: &'r str,
    password: &'r str,
}

#[post("/login", format = "json", data = "<data>")]
pub async fn login(
    data: Json<LoginData<'_>>,
    auth: &State<Arc<Authenticator>>,
    cookies: &CookieJar<'_>,
) -> Result<String, ErrorResponse> {
    auth.authenticate(&data.username, &data.password).await?;

    info!("Authenticated user {}", data.username);

    let cookie = Cookie::new(COOKIE_NAME, data.username.to_string());
    cookies.add_private(cookie);

    Ok("".to_string())
}
