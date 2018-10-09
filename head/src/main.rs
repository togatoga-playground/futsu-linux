extern crate getopts;
use getopts::Options;
use std::env;
use std::error::Error;
use std::io::Write;
use std::process;

fn print_usage(program: &str, opts: Options) {
    let brief = format!("Usage: {} [OPTION] FILE", program);
    println!("{}", opts.usage(&brief));
}

fn do_head() {}

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
}
