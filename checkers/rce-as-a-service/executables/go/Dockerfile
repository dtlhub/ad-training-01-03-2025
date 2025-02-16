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
RUN wget https://github.com/tinygo-org/tinygo/releases/download/v0.35.0/tinygo_0.35.0_amd64.deb \
    && dpkg -i tinygo_0.35.0_amd64.deb \
    && rm tinygo_0.35.0_amd64.deb

WORKDIR /build

ENV GOOS=wasip2
ENV GOARCH=wasm
ENTRYPOINT ["tinygo", "build", "-o", "/out/output.wasm"]