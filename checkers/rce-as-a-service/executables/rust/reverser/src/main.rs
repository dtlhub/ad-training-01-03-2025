use std::env;

fn decode_base64(input: &str) -> Result<Vec<u8>, String> {
    let mut output = Vec::new();
    let mut buf: u32 = 0;
    let mut bits: u32 = 0;
    let mut padding = 0;

    for c in input.chars() {
        if c == '=' {
            padding += 1;
            continue;
        }

        let val = match c {
            'A'..='Z' => c as u32 - 'A' as u32,
            'a'..='z' => c as u32 - 'a' as u32 + 26,
            '0'..='9' => c as u32 - '0' as u32 + 52,
            '+' => 62,
            '/' => 63,
            _ => continue,
        };

        buf = (buf << 6) | val;
        bits += 6;

        if bits >= 8 {
            bits -= 8;
            output.push((buf >> bits) as u8);
        }
    }

    // Handle remaining bits based on padding
    if padding == 1 && bits >= 2 {
        bits -= 2;
        output.push((buf >> bits) as u8);
    } else if padding == 2 && bits >= 4 {
        bits -= 4;
        output.push((buf >> bits) as u8);
    }

    Ok(output)
}

fn reverse_string(s: &str) -> String {
    s.chars().rev().collect()
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: {} <base64_string>", args[0]);
        std::process::exit(1);
    }

    match decode_base64(&args[1]) {
        Ok(decoded) => {
            if let Ok(decoded_str) = String::from_utf8(decoded) {
                print!("{}", reverse_string(&decoded_str));
            } else {
                eprintln!("Error: decoded data is not valid UTF-8");
                std::process::exit(1);
            }
        }
        Err(e) => {
            eprintln!("Error decoding base64: {}", e);
            std::process::exit(1);
        }
    }
}
