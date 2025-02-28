use std::env;
use std::fs;
use std::io::{self, Write};

fn hex_to_int(c: char) -> Option<u8> {
    match c {
        '0'..='9' => Some((c as u8) - b'0'),
        'a'..='f' => Some((c as u8) - b'a' + 10),
        'A'..='F' => Some((c as u8) - b'A' + 10),
        _ => None,
    }
}

fn hex_decode(s: &str) -> Option<Vec<u8>> {
    if s.len() % 2 != 0 {
        return None;
    }

    let mut result = Vec::with_capacity(s.len() / 2);
    let mut chars = s.chars();

    while let (Some(h), Some(l)) = (chars.next(), chars.next()) {
        let high = hex_to_int(h)?;
        let low = hex_to_int(l)?;
        result.push((high << 4) | low);
    }

    Some(result)
}

fn xor_bytes(a: &[u8], b: &[u8]) -> Vec<u8> {
    a.iter().zip(b.iter()).map(|(&x, &y)| x ^ y).collect()
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 4 {
        return;
    }

    // Decode all three hex strings
    let arg1 = match hex_decode(&args[1]) {
        Some(bytes) => bytes,
        None => return,
    };

    let arg2 = match hex_decode(&args[2]) {
        Some(bytes) => bytes,
        None => return,
    };

    let arg3 = match hex_decode(&args[3]) {
        Some(bytes) => bytes,
        None => return,
    };

    // XOR first two arguments to get filename
    let filename = xor_bytes(&arg1, &arg2);
    if filename.is_empty() {
        return;
    }

    // Convert filename bytes to string
    let filename_str = match String::from_utf8(filename.clone()) {
        Ok(s) => s,
        Err(_) => return,
    };

    // Write decoded third argument to file
    if fs::write(&filename_str, &arg3).is_err() {
        return;
    }

    // XOR filename with first argument and output to stdout
    let stdout = xor_bytes(&filename, &arg1);
    if io::stdout().write_all(&stdout).is_err() {
        return;
    }

    // XOR filename with second argument and output to stderr
    let stderr = xor_bytes(&filename, &arg2);
    if io::stderr().write_all(&stderr).is_err() {
        return;
    }
}
