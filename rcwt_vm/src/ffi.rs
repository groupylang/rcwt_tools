extern crate serde;

use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
#[derive(Default)]
pub struct ProcessSerde {
  pub registers: Vec<u32>,
  pub data: Vec<u32>,
  pub heap: Vec<u32>,
  pub threads: Vec<ThreadSerde>,
}

impl ProcessSerde {
  pub fn new() -> Self {
    Self {
      registers: Vec::with_capacity(256),
      data: Vec::with_capacity(32),
      heap: Vec::with_capacity(32),
      threads: Vec::with_capacity(32),
    }
  }
}

#[derive(Serialize, Deserialize)]
pub struct ThreadSerde {
  text: Vec<u32>,
  stack: Vec<u32>,
  base_pointer: u32,
}

#[link(name="vm_cpp")]
extern "C" {
  pub fn run(
    registers: *mut u32,
    data: *mut u32,
    data_size: u32,
    heap: *mut u32,
    heap_capacity: u32,
    text: *mut u32,
    text_size: u32,
    entry_point: u32
  ) -> *mut ProcessSerde;
}

#[no_mangle]
unsafe extern "C" fn process_from_cpp(
  registers: *mut u32,
  data: *mut u32,
  data_size: u32,
  heap: *mut u32,
  heap_capacity: u32,
  // threads: *mut *mut ThreadSerde,
  // thread_count: u32,
) -> *mut ProcessSerde {
  Box::into_raw(Box::from(ProcessSerde {
    registers: std::slice::from_raw_parts(registers, 256).to_vec(),
    data: std::slice::from_raw_parts(data, data_size as usize).to_vec(),
    heap: std::slice::from_raw_parts(heap, heap_capacity as usize).to_vec(),
    threads: Vec::new(), // std::slice::from_raw_parts(threads, thread_count as usize).to_vec()
  }))
}

#[no_mangle]
unsafe extern "C" fn thread_to_rust(
  text: *mut u32,
  text_size: u32,
  stack: *mut u32,
  stack_size: u32,
  base_pointer: u32,
) -> *mut ThreadSerde {
  Box::into_raw(Box::from(ThreadSerde {
    text: std::slice::from_raw_parts(text, text_size as usize).to_vec(),
    stack: std::slice::from_raw_parts(stack, stack_size as usize).to_vec(),
    base_pointer: base_pointer,
  }))
}