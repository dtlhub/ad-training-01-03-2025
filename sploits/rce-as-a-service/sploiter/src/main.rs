use std::io::Write;
use std::net::TcpStream;
use std::thread::sleep;
use std::time::{Duration, Instant};

fn main() -> std::io::Result<()> {
    println!("Hello, world!");
    let start = Instant::now();
    sleep(Duration::from_millis(100));

    println!("Napped for {:?}", Instant::now().duration_since(start));

    let mut stream = TcpStream::connect("158.160.52.193:34254")?;
    stream.write_all(b"Hello, world!")?;

    Ok(())
}
