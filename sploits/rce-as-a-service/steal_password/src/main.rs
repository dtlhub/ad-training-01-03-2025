use std::env::args;
use std::io::{Read, Write};
use std::net::TcpStream;

fn main() -> std::io::Result<()> {
    let username = args().nth(1).unwrap();
    let mut stream = TcpStream::connect("172.43.21.3:6379")?;
    stream.write_all(format!("GET user:{}:password\n", username).as_bytes())?;

    let mut buf = [0; 1024];
    let result = stream.read(&mut buf)?;
    let str = String::from_utf8_lossy(&buf[..result]);
    let parts = str.split("\n").collect::<Vec<&str>>();
    println!("{}", parts[1]);

    Ok(())
}
