FROM lukemathwalker/cargo-chef:latest-rust-1.84.1-slim-bookworm AS chef
WORKDIR /app
RUN apt-get update && apt-get install -y pkg-config libssl-dev

FROM chef AS planner
COPY Cargo.toml Cargo.lock ./
COPY src ./src
COPY .sqlx ./.sqlx
RUN cargo chef prepare --recipe-path recipe.json

FROM chef AS builder
COPY --from=planner /app/recipe.json recipe.json
RUN cargo chef cook --release --recipe-path recipe.json
COPY Cargo.toml Cargo.lock ./
COPY src ./src
COPY .sqlx ./.sqlx
RUN cargo build --release --bin rce-as-a-service

FROM debian:bookworm AS runner
WORKDIR /app
RUN apt-get update && apt-get install -y pkg-config libssl-dev
COPY --from=builder /app/target/release/rce-as-a-service /opt/rce-as-a-service
ENTRYPOINT ["/opt/rce-as-a-service"]
