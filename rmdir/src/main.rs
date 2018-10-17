use std::env;
use std::error::Error;
use std::fs;
use std::io::Write;
use std::process;

fn do_rmdir(path: &String) {
    match fs::remove_dir(path) {
        Ok(_) => {}
        Err(why) => {
            write!(std::io::stderr(), "{}", why.description());
            process::exit(1);
        }
    };
}
fn main() {
    let args: Vec<String> = env::args().collect();
    let program = &args[0];
    if args.len() < 2 {
        write!(std::io::stderr(), "{}: no argument\n", program);
        process::exit(1);
    }
    for v in &args[1..] {
        do_rmdir(&v);
    }
    process::exit(0);
}
