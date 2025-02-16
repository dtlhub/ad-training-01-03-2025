FROM alpine:3.19

# Install required packages
RUN apk add --no-cache wget xz

# Install Zig
RUN wget https://ziglang.org/download/0.11.0/zig-linux-x86_64-0.11.0.tar.xz \
    && tar xf zig-linux-x86_64-0.11.0.tar.xz \
    && mv zig-linux-x86_64-0.11.0 /usr/local/zig \
    && rm zig-linux-x86_64-0.11.0.tar.xz

ENV PATH="/usr/local/zig:${PATH}"

WORKDIR /build

# Expect source code to be mounted at /src and output at /out
COPY build.sh /build.sh
RUN chmod +x /build.sh
ENTRYPOINT ["/build.sh"]