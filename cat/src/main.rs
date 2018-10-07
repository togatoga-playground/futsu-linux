use std::env;
use std::error::Error;
use std::fs::File;
use std::io::Read;
use std::io::Write;
use std::process;

fn do_cat(path: String) {
    let mut file = match File::open(&path) {
        Ok(file) => file,
        Err(why) => {
            write!(
                std::io::stderr(),
                "couldn't open {}: {}",
                path,
                why.description()
            );
            process::exit(1);
        }
    };

    let mut contents = String::new();
    match file.read_to_string(&mut contents) {
        Ok(_) => {}
        Err(why) => {
            write!(
                std::io::stderr(),
                "couldn't read: {}: {}",
                path,
                why.description()
            );
            process::exit(1);
        }
    }
    write!(std::io::stdout(), "{}", contents);
}
fn main() {
    let argv: Vec<String> = env::args().collect();
    if argv.len() < 2 {
        writeln!(std::io::stderr(), "{} file name not given", argv[0]);
        process::exit(1);
    }
    for v in &argv[1..] {
        do_cat(v.to_string());
    }
    process::exit(0);
}
