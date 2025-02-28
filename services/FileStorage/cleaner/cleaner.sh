#!/bin/sh

while true; do
    date -uR

    find "/app/uploads/" \
        -type d \
        -and -not -path "/app/uploads/" \
        -and -not -newermt "-1200 seconds" \
        -exec echo "Removing directories:" {} + \
        -exec rm -rf {} +

    sleep 60
done