FROM ubuntu:22.04

# Install required packages
RUN apt-get update && apt-get install -y \
    wget \
    xz-utils \
    make \
    && rm -rf /var/lib/apt/lists/*

# Install WASI SDK
RUN wget https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-22/wasi-sdk-22.0-linux.tar.gz \
    && tar xvf wasi-sdk-22.0-linux.tar.gz -C /opt \
    && rm wasi-sdk-22.0-linux.tar.gz \
    && ln -s /opt/wasi-sdk-22.0 /opt/wasi-sdk

ENV WASI_SDK_PATH=/opt/wasi-sdk

WORKDIR /build

# Expect source code to be mounted at /src and output at /out
ENTRYPOINT ["/opt/wasi-sdk/bin/clang", "--target=wasm32-wasip2", "-O2", "-o", "/out/output.wasm"]