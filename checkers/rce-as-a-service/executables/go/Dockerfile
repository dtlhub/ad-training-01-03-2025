FROM ubuntu:22.04

# Install required packages
RUN apt-get update && apt-get install -y \
    wget \
    xz-utils \
    make \
    && rm -rf /var/lib/apt/lists/*

# Install Go
RUN wget https://go.dev/dl/go1.23.5.linux-amd64.tar.gz \
    && tar -C /usr/local -xzf go1.23.5.linux-amd64.tar.gz \
    && rm go1.23.5.linux-amd64.tar.gz

# Install wasm-tools
RUN wget https://github.com/bytecodealliance/wasm-tools/releases/download/v1.225.0/wasm-tools-1.225.0-x86_64-linux.tar.gz \
    && tar -xzf wasm-tools-1.225.0-x86_64-linux.tar.gz \
    && mv wasm-tools-1.225.0-x86_64-linux/wasm-tools /usr/local/bin/ \
    && rm -rf wasm-tools-1.225.0-x86_64-linux.tar.gz wasm-tools-1.225.0-x86_64-linux

ENV PATH="/usr/local/go/bin:${PATH}"

# Install TinyGo
RUN wget https://github.com/tinygo-org/tinygo/releases/download/v0.35.0/tinygo_0.35.0_amd64.deb \
    && dpkg -i tinygo_0.35.0_amd64.deb \
    && rm tinygo_0.35.0_amd64.deb

WORKDIR /build

ENV GOOS=wasip2
ENV GOARCH=wasm

ENTRYPOINT ["tinygo", "build", "-target", "wasip2", "-opt=2", "-gc=leaking", "-scheduler=none", "-o", "/out/output.wasm"]