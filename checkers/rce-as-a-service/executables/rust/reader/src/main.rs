use std::env;
use std::fs;
use std::io::{self, Write};

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: {} <filename>", args[0]);
        std::process::exit(1);
    }

    let contents = match fs::read(&args[1]) {
        Ok(data) => data,
        Err(err) => {
            eprintln!("Error: Could not read file '{}': {}", args[1], err);
            std::process::exit(1);
        }
    };

    if let Err(err) = io::stdout().write_all(&contents) {
        eprintln!("Error: Failed to write to stdout: {}", err);
        std::process::exit(1);
    }
}