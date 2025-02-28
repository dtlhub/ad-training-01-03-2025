#!/bin/sh

while true; do
    date -uR

    find "/tmp/user_orders/" \
        -type d \
        -and -not -path "/tmp/user_orders/" \
        -and -not -newermt "-1200 seconds" \
        -exec echo "Removing directories:" {} + \
        -exec rm -rf {} +

    for dir in orders sessions users; do
        find "/tmp/$dir" \
            -type f \
            -and -not -path "/tmp/$dir" \
            -and -not -newermt "-1200 seconds" \
            -exec echo "Removing files:" {} + \
            -exec rm -rf {} +
    done

    sleep 60
done