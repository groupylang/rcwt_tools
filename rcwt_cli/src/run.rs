extern crate rcwt_vm;

use rcwt_vm::ffi::{ProcessSerde, run};

// interactive mode like command prompt
pub fn prompt(p: &mut ProcessSerde) -> std::io::Result<()> {
  // use p::instruction::check_instruction_format;
  // use super::util::{print, prompt_mut};
  // prompt_mut(">", |line|
  //   match &check_instruction_format(line) {
  //     Ok(instruction) => p.execute(instruction),
  //     Err(e) => print(e).unwrap(),
  //   }
  // )
  Ok(())
}

// run .obj.yml script
pub fn run_script(p: &mut ProcessSerde, path: &str) -> std::io::Result<ProcessSerde> {
  let mut script = super::fio::read_Script(path)?;
  let data_size = p.data.len() as u32;
  let heap_capacity = p.heap.len() as u32;
  let text_size = (script.body.len() / 4) as u32;
  let q = unsafe {
    run(
      p.registers.as_mut_ptr(),
      p.data.as_mut_ptr(),
      data_size,
      p.heap.as_mut_ptr(),
      heap_capacity,
      script.body.as_mut_ptr() as *mut u32,
      text_size,
      script.entry_point
    )
  };
  Ok(unsafe {std::mem::replace(&mut *q, Default::default())})
}