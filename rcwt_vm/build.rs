extern crate cc;
#[cfg(target_os = "linux")]
fn main() {
  cc::Build::new()
  	.cpp(true)
  	.warnings(true)
	  .flag("-pthread")
  	.file("src_cpp/core.cpp")
    .file("src_cpp/mem.cpp")
    .file("src_cpp/util.cpp")
  	.include("src_cpp")
	.compile("vm_cpp")
}
#[cfg(target_os = "windows")]
fn main() {
  	cc::Build::new()
  	.cpp(true)
  	.warnings(true)
  	.file("src_cpp/core.cpp")
    .file("src_cpp/mem.cpp")
    .file("src_cpp/util.cpp")
  	.include("src/c")
  	.compile("vm_cpp")
}