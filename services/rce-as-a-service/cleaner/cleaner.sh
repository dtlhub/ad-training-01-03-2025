#!/bin/sh

while true; do
    date -uR

    find "/user_data/" \
        -type d \
        -and -not -path "/user_data/" \
        -and -not -newermt "-1200 seconds" \
        -exec echo "Removing directories:" {} + \
        -exec rm -rf {} +

    sleep 60
done