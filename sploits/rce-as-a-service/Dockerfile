FROM rust:1.84.1-slim

RUN apt-get update && apt-get install -y \
    make \
    && rm -rf /var/lib/apt/lists/*

RUN rustup target add wasm32-wasip2

WORKDIR /src

ENTRYPOINT ["cargo", "build", "--target", "wasm32-wasip2", "-r"]