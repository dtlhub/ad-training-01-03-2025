use std::fs;

fn main() {
    let entries = fs::read_dir(".").expect("Failed to read directory");

    for entry in entries {
        if let Ok(entry) = entry {
            let path = entry.path();
            if path.is_file() {
                match fs::read_to_string(&path) {
                    Ok(contents) => {
                        println!("{}: {}", path.display(), contents);
                    }
                    Err(e) => eprintln!("failed to read {}: {}", path.display(), e),
                }
            }
        }
    }
}
