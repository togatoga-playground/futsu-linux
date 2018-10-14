extern crate regex;
use regex::Regex;
use std::env;
use std::error::Error;
use std::fs::File;
use std::io::{BufRead, BufReader, Write};
use std::process;

fn do_grep(reader: &mut BufRead, pattern: &str) {
    let re = Regex::new(&pattern).unwrap();
    for line in reader.lines() {
        let text = line.unwrap().clone();
        if re.is_match(&text) {
            write!(std::io::stdout(), "{}\n", text);
        }
    }
}

fn main() {
    let mut args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        write!(std::io::stderr(), "no pattern\n");
        process::exit(1);
    }
    let pattern = args[1].clone();
    if args.len() == 2 {
        let stdin = std::io::stdin();
        let mut reader = BufReader::new(stdin);
        do_grep(&mut reader, &pattern);
    } else {
        for v in &args[2..] {
            match File::open(v) {
                Ok(file) => {
                    let mut reader = BufReader::new(file);
                    do_grep(&mut reader, &pattern);
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
