use std::env;

fn decode_base64(input: &str) -> Result<Vec<u8>, String> {
    let mut output = Vec::new();
    let mut buf = 0u32;
    let mut bits = 0u32;
    let mut padding = 0;

    let chars: Vec<char> = input.chars().collect();
    let mut i = 0;
    while i < chars.len() {
        let c = chars[i];
        i += 1;

        // Handle padding
        if c == '=' {
            if bits >= 8 {
                bits -= 8;
                output.push((buf >> bits) as u8);
            }
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

    // Handle remaining bits if we have any
    if bits > 0 && padding < 2 {
        output.push((buf << (8 - bits)) as u8);
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
