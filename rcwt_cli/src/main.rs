//! cli for RCWT
//! link, assemble, start process, stop process, run script

extern crate clap;
extern crate rcwt_cli;
extern crate rcwt_vm;

use clap::{App, Arg, SubCommand};
use rcwt_cli::fio::{read_processes, write_processes};
use rcwt_cli::util::warning;
use rcwt_cli::run::{prompt, run_script};
use rcwt_vm::ffi::ProcessSerde;

fn main() -> std::io::Result<()> {
  let mut process_serdes = read_processes("dump")?;
  let matches = App::new("rcwt-cli")
  .version("0.1.0")
  .author("skyrabbit <iamskyrabbit@gmail.com>")
  .about("command-line interface for RCWT")
  .subcommand(SubCommand::with_name("assemble")
    .about("Assembles TODO")
    .arg(Arg::with_name("path")
      .help("Path of script(mnemonic)")
      .takes_value(true)
      .required(true)
    )
  )
  .subcommand(SubCommand::with_name("link")
    .about("Links TODO")
    .arg(Arg::with_name("path")
      .help("Path of script")
      .takes_value(true)
      .required(true)
    )
  )
  .subcommand(SubCommand::with_name("start")
    .about("Starts virtual machine")
    .arg(Arg::with_name("id")
      .help("Id of package")
      .takes_value(true)
      .required(true)
    )
  )
  .subcommand(SubCommand::with_name("stop")
    .about("Stop virtual machine")
    .arg(Arg::with_name("id")
      .help("Id of package")
      .takes_value(true)
      .required(true)
    )
  )
  .subcommand(SubCommand::with_name("run")
    .about("Runs with virtual machine")
    .arg(Arg::with_name("id")
      .help("Id of package")
      .takes_value(true)
      .required(true)
    )
    .arg(Arg::with_name("path")
      .help("Path of script")
      .takes_value(true)
    )
  )
  .subcommand(SubCommand::with_name("test")
    .about("Tests script")
    .arg(Arg::with_name("id")
      .help("Id of package")
      .takes_value(true)
      .required(true)
    )
    .arg(Arg::with_name("path")
      .help("Path of script")
      .takes_value(true)
      .required(true)
    )
  ).get_matches();
  // assemble
  if let Some(ref matches) = matches.subcommand_matches("assemble") {
    // let path = matches.value_of("path").unwrap();
    // use cli::assemble::assemble;
    // assemble(path).expect(""); // TODO handle error
  }
  // link
  if let Some(ref matches) = matches.subcommand_matches("link") {
    // let path = matches.value_of("path").unwrap();
    // use cli::ll::link;
    // link(path).expect(""); // TODO handle error
  }
  // start
  if let Some(ref matches) = matches.subcommand_matches("start") {
    let id: isize = String::from(matches.value_of("id").unwrap()).parse().expect("id must be a number(isize)");
    if process_serdes.contains_key(&id) {
      println!("{}", warning(&format!("process with id {} found", id)))
    } else {
      process_serdes.insert(id, ProcessSerde::new());
    }
  }
  // stop
  if let Some(ref matches) = matches.subcommand_matches("stop") {
    let id: isize = String::from(matches.value_of("id").unwrap()).parse().expect("id must be a number(isize)");
    if process_serdes.contains_key(&id) {
      process_serdes.remove(&id);
    } else {
      println!("{}", warning(&format!("process with id {} not found", id)))
    }
  }
  // run
  if let Some(ref matches) = matches.subcommand_matches("run") {
    let id: isize = String::from(matches.value_of("id").unwrap()).parse().expect("id must be a number(isize)");
    if process_serdes.contains_key(&id) {
      let process_serde = process_serdes.get_mut(&id).unwrap();
      if let Some(path) = matches.value_of("path") {
        let p = run_script(process_serde, path)?;
        process_serdes.insert(id, p);
      } else {
        prompt(process_serde)?;
      }
    } else {
      println!("{}", warning(&format!("process with id {} not found", id)))
    }
  }
  // test
  if let Some(ref matches) = matches.subcommand_matches("test") {
    // let id: isize = String::from(matches.value_of("id").unwrap()).parse().expect("id must be a number(isize)");
    // let path = matches.value_of("path").unwrap();
    
    // if processs.contains_key(&id) {
    //   let process = processs.get_mut(&id).unwrap();
    //   use cli::test;
    //   test!{ let _ = process.run_script(path) }
    // } else {
    //   println!("{}", warning(&format!("process with id {} not found", id)))
    // }
  }
  write_processes("dump", process_serdes)?;
  Ok(())
}