extern crate getopts;
use getopts::Options;
use std::env;
use std::error::Error;
use std::fs::File;
use std::io::Write;
use std::io::{BufRead, BufReader};
use std::process;

fn print_usage(program: &str, opts: Options) {
    let brief = format!("Usage: {} [OPTION] FILE", program);
    println!("{}", opts.usage(&brief));
}

fn do_head(reader: &mut BufRead, max_line: usize) {
    for (idx, line) in reader.lines().enumerate() {
        if idx >= max_line {
            break;
        }
        write!(std::io::stdout(), "{}\n", line.unwrap());
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let program = args[0].clone();
    let mut opts = Options::new();
    opts.optflag("h", "help", "print this help menu");
    opts.optopt("n", "line", "print the first NUM lines", "NUM");
    let matches = match opts.parse(&args[1..]) {
        Ok(m) => m,
        Err(why) => {
            write!(std::io::stderr(), "{}", why.description());
            process::exit(1);
        }
    };

    if matches.opt_present("h") {
        print_usage(&program, opts);
        process::exit(0);
    }

    let line = match matches
        .opt_str("n")
        .unwrap_or("5".to_string())
        .parse::<usize>()
    {
        Ok(n) => n,
        Err(why) => {
            write!(std::io::stderr(), "{}", why.description());
            process::exit(1);
        }
    };
    if matches.free.is_empty() {
        let stdin = std::io::stdin();
        let mut reader = BufReader::new(stdin);
        do_head(&mut reader, line);
        process::exit(0);
    }
    for v in matches.free.clone() {
        match File::open(v.to_string()) {
            Ok(file) => {
                let mut reader = BufReader::new(file);
                do_head(&mut reader, line);
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
    process::exit(0);
}
