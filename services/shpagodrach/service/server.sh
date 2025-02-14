#!/usr/bin/env bash
echo "[DEBUG] Starting server.sh in: $(pwd)"
cd task
echo "$(ls -lah)"
socat tcp-l:1337,fork,reuseaddr exec:"sudo -E -u nobody ./chall"