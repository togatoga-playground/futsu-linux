use std::env;
use std::fs;
use std::io::Write;

fn do_ls(path: &String) {
    for entry in fs::read_dir(path).unwrap() {
        let dir: fs::DirEntry = entry.unwrap();
        let file = dir.file_name().into_string();
        println!("{}", file.unwrap());
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let program = &args[0];
    if args.len() < 2 {
        write!(std::io::stderr(), "{}: no argument\n", program);
        std::process::exit(1);
    }
    for v in &args[1..] {
        do_ls(v);
    }
}
