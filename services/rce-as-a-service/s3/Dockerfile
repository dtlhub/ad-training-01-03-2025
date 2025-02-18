FROM rclone/rclone:latest

WORKDIR /data

CMD ["serve", "s3", "/data", "--no-auth", "--addr", ":8333"]