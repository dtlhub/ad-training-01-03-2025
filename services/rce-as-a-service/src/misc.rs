use rocket::http::Status;
use rocket::request::Outcome;

#[derive(Debug, Clone)]
pub struct Error {
    pub status_code: Status,
    pub message: String,
}

pub type ErrorResponse = (Status, String);

impl Error {
    pub fn new(status_code: Status, message: impl std::fmt::Display) -> Self {
        Self {
            status_code,
            message: message.to_string(),
        }
    }

    pub fn with_context(self, context: impl std::fmt::Display) -> Self {
        Self {
            status_code: self.status_code,
            message: format!("{}: {}", context, self.message),
        }
    }
}

impl std::fmt::Display for Error {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.message)
    }
}

impl From<std::io::Error> for Error {
    fn from(err: std::io::Error) -> Self {
        Self::new(Status::ServiceUnavailable, err.to_string())
    }
}

impl From<wasmtime::Error> for Error {
    fn from(err: wasmtime::Error) -> Self {
        Self::new(Status::ServiceUnavailable, err.to_string())
    }
}

impl From<redis::RedisError> for Error {
    fn from(err: redis::RedisError) -> Self {
        Self::new(Status::ServiceUnavailable, err.to_string())
    }
}

impl From<Error> for ErrorResponse {
    fn from(err: Error) -> Self {
        (err.status_code, err.to_string())
    }
}

impl<T> From<Error> for Outcome<T, ErrorResponse> {
    fn from(err: Error) -> Self {
        Outcome::Error((err.status_code, (err.status_code, err.to_string())))
    }
}

pub type Result<T> = std::result::Result<T, Error>;
