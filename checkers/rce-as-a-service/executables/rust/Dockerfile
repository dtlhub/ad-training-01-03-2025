FROM rust:1.75-slim

# Install required packages
RUN apt-get update && apt-get install -y \
    make \
    && rm -rf /var/lib/apt/lists/*

# Add wasm32-wasi target
RUN rustup target add wasm32-wasi

WORKDIR /build

# Expect source code to be mounted at /src and output at /out
ENTRYPOINT ["rustc", "--target", "wasm32-wasi", "-o", "/out/output.wasm"]