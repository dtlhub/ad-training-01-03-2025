use std::env;

fn decode_base64(input: &str) -> Result<Vec<u8>, String> {
    // Remove whitespace characters.
    let clean: Vec<char> = input.chars().filter(|c| !c.is_whitespace()).collect();

    if clean.len() % 4 != 0 {
        return Err("Invalid base64 input length".to_string());
    }

    let mut output = Vec::new();
    for chunk in clean.chunks(4) {
        let mut vals = [0u32; 4];
        let mut pad = 0;
        for (i, &c) in chunk.iter().enumerate() {
            if c == '=' {
                vals[i] = 0;
                pad += 1;
            } else {
                vals[i] = match c {
                    'A'..='Z' => c as u32 - 'A' as u32,
                    'a'..='z' => c as u32 - 'a' as u32 + 26,
                    '0'..='9' => c as u32 - '0' as u32 + 52,
                    '+' => 62,
                    '/' => 63,
                    _ => return Err(format!("Invalid base64 character: {}", c)),
                };
            }
        }

        let triple = (vals[0] << 18) | (vals[1] << 12) | (vals[2] << 6) | vals[3];
        output.push(((triple >> 16) & 0xff) as u8);
        if pad < 2 {
            output.push(((triple >> 8) & 0xff) as u8);
        }
        if pad == 0 {
            output.push((triple & 0xff) as u8);
        }
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
