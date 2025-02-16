FROM ubuntu:22.04

# Install required packages
RUN apt-get update && apt-get install -y \
    wget \
    xz-utils \
    make \
    && rm -rf /var/lib/apt/lists/*

# Install Go
RUN wget https://go.dev/dl/go1.21.6.linux-amd64.tar.gz \
    && tar -C /usr/local -xzf go1.21.6.linux-amd64.tar.gz \
    && rm go1.21.6.linux-amd64.tar.gz

ENV PATH="/usr/local/go/bin:${PATH}"

# Install TinyGo
RUN wget https://github.com/tinygo-org/tinygo/releases/download/v0.30.0/tinygo_0.30.0_amd64.deb \
    && dpkg -i tinygo_0.30.0_amd64.deb \
    && rm tinygo_0.30.0_amd64.deb

WORKDIR /build

# Expect source code to be mounted at /src and output at /out
ENTRYPOINT ["tinygo", "build", "-target=wasi", "-o", "/out/output.wasm"]