#!/bin/sh

# Build the WASM file
cd /src || exit 1
zig build-exe "$1" -target wasm32-wasi.0.2.0 -O ReleaseSmall -fstrip -femit-bin=/out/output.wasm

status=$?

# Clean up any intermediate files
rm -rf zig-cache zig-out

exit $status