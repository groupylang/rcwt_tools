//! file IO
extern crate serde;
extern crate paste;
extern crate rcwt_vm;

use serde::{Deserialize, Serialize};
use std::fs::File;
use std::io::{BufReader, BufWriter};
use std::collections::HashMap;
use rcwt_vm::ffi::ProcessSerde;

macro_rules! readable_yaml {
  ($i: ident) => {
    paste::item! {
      #[allow(non_snake_case)]
      pub(crate) fn [<read_ $i>](path: &str) -> std::io::Result<$i> {
        let file = File::open(format!("{}.yml", path))?;
        let reader = BufReader::new(file);
        let [<_ $i>] = serde_yaml::from_reader(reader).expect("error | InvalidYamlFile");
        Ok([<_ $i>])
      }
    }
  };
  ($i: ident, $e: expr) => {
    paste::item! {
      #[allow(non_snake_case)]
      pub(crate) fn [<read_ $i>](path: &str) -> std::io::Result<$i> {
        let file = File::open(format!("{}.{}.yml", path, $e))?;
        let reader = BufReader::new(file);
        let [<_ $i>] = serde_yaml::from_reader(reader).expect("error | InvalidYamlFile");
        Ok([<_ $i>])
      }
    }
  };
}

macro_rules! writable_yaml {
  ($i: ident) => {
    paste::item! {
      #[allow(non_snake_case)]
      pub(crate) fn [<write_ $i>](path: &str, [<_ $i>]: &$i) -> std::io::Result<()> {
        let file = File::create(format!("{}.yml", path))?;
        let writer = BufWriter::new(file);
        serde_yaml::to_writer(writer, [<_ $i>]).expect("error | InvalidYamlFile");
        Ok(())
      }
    }
  };
  ($i: ident, $e: expr) => {
    paste::item! {
      #[allow(non_snake_case)]
      #[allow(dead_code)]
      pub(crate) fn [<write_ $i>](path: &str, [<_ $i>]: &$i) -> std::io::Result<()> {
        let file = File::create(format!("{}.{}.yml", path, $e))?;
        let writer = BufWriter::new(file);
        serde_yaml::to_writer(writer, [<_ $i>]).expect("error | InvalidYamlFile");
        Ok(())
      }
    }
  };
}

pub fn write_processes(path: &str, process_serdes: HashMap<isize, ProcessSerde>) -> std::io::Result<()> {
  let file = File::create(format!("{}.yml", path))?;
  let writer = BufWriter::new(file);
  serde_yaml::to_writer(writer, &process_serdes).expect("error | InvalidYamlFile");
  Ok(())
}

pub fn read_processes(path: &str) -> std::io::Result<HashMap<isize, ProcessSerde>> {
  let file = File::open(format!("{}.yml", path))?;
  let reader = BufReader::new(file);
  let process_serdes: HashMap<isize, ProcessSerde> = serde_yaml::from_reader(reader).expect("error | InvalidYamlFile");
  Ok(process_serdes)
}

/// defined symbol in the module
#[derive(Serialize, Deserialize)]
pub(crate) struct Symbol {
  pub(crate) name: String,
  pub(crate) base_address: u32,
}

/// reference to symbol to be relocated
#[derive(Serialize, Deserialize)]
pub(crate) struct Reference {
  pub(crate) base_address: u32,
  pub(crate) symbol_name: String,
}

#[derive(Serialize, Deserialize)]
pub(crate) struct Native {
  pub(crate) native_id: u32,
  pub(crate) symbol_name: String,
}

#[derive(Serialize, Deserialize)]
pub(crate) struct ScriptM { // TODO rename Assembly
  pub(crate) symbols: Vec<Symbol>,
  pub(crate) includes: Vec<String>,
  pub(crate) references: Vec<Reference>,
  pub(crate) natives: Vec<Native>,
  // pub(crate) body: Vec<Mnemonic>,
}
readable_yaml!(ScriptM, "asm");
writable_yaml!(ScriptM, "asm");

#[derive(Serialize, Deserialize)]
#[repr(C)]
pub(crate) struct Script {
  pub(crate) symbols: Vec<Symbol>,
  pub(crate) includes: Vec<String>,
  pub(crate) references: Vec<Reference>,
  pub(crate) natives: Vec<Native>,
  pub(crate) entry_point: u32,
  pub(crate) body: Vec<u8>,
}
readable_yaml!(Script, "obj");
writable_yaml!(Script, "obj");