#!/bin/sh

# Build the WASM file
cd /src
if [ -f "build.zig" ]; then
    zig build
    if [ -f "zig-out/bin/simple.wasm" ]; then
        cp zig-out/bin/simple.wasm /out/output.wasm
    elif [ -f "zig-out/bin/reverser.wasm" ]; then
        cp zig-out/bin/reverser.wasm /out/output.wasm
    elif [ -f "zig-out/bin/writer.wasm" ]; then
        cp zig-out/bin/writer.wasm /out/output.wasm
    elif [ -f "zig-out/bin/reader.wasm" ]; then
        cp zig-out/bin/reader.wasm /out/output.wasm
    else
        echo "No WASM file found in output directory"
        exit 1
    fi
else
    zig build-exe "$1" -target wasm32-wasi.0.2.0 -O ReleaseSmall -fstrip -femit-bin=/out/output.wasm
fi
status=$?

# Clean up any intermediate files
rm -rf zig-cache zig-out

exit $status