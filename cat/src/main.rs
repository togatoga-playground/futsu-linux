use std::env;
use std::error::Error;
use std::fs::File;
use std::io::{BufRead, BufReader};
use std::io::{Read, Write};
use std::process;

fn do_cat(reader: &mut BufRead, is_number: bool) {
    for (idx, line) in reader.lines().enumerate() {
        if is_number {
            write!(std::io::stdout(), "{} {}\n", idx, line.unwrap());
        } else {
            write!(std::io::stdout(), "{}\n", line.unwrap());
        }
    }
}
fn main() {
    let mut argv: Vec<String> = env::args().collect();

    //parse argument
    let mut is_number = false;
    for v in &argv[1..] {
        if v == "-n" || v == "--number" {
            is_number = true;
        }
    }
    argv.retain(|v| v != "-n" && v != "--number");

    //input stdin
    if argv.len() < 2 {
        let stdin: std::io::Stdin = std::io::stdin();
        let mut reader = BufReader::new(stdin);
        do_cat(&mut reader, is_number);
    } else {
        for v in &argv[1..] {
            match File::open(v.to_string()) {
                Ok(file) => {
                    let mut reader = BufReader::new(file);
                    do_cat(&mut reader, is_number);
                }
                Err(why) => {
                    write!(
                        std::io::stderr(),
                        "couldn't open {}: {}",
                        v,
                        why.description()
                    );
                    process::exit(1);
                }
            }
        }
    }
    process::exit(0);
}
