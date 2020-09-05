#ifndef VM_MEM_H
#define VM_MEM_H

#include <stdint.h>
#include <vector>
#include <string>
#include <future>
#include <unordered_map>
#include <iostream>

typedef struct _process process;
typedef struct _thread thread;
typedef struct _instr instr;
typedef void(*native)(thread&); // native procedure

struct _process {
  uint32_t* registers;
  uint32_t* data; // constant pool
  uint32_t data_size;
  std::vector<uint32_t> heap;
  std::unordered_map<size_t, std::future<uint8_t>> threads;
};

struct _thread {
  uint32_t* text;
  uint32_t text_size;
  std::vector<uint32_t> stack;
  uint32_t base_pointer;
  process* parent;
  std::unordered_map<size_t, native> natives;
};

struct _instr {
  uint8_t opcode;
  uint8_t operand0; // dst
  uint8_t operand1; // src left
  uint8_t operand2; // src right
};

// ffi.rs
typedef struct ProcessSerde ProcessSerde;
extern "C" ProcessSerde* process_from_cpp(uint32_t*, uint32_t*, uint32_t, uint32_t*, uint32_t);

// mem.cpp
process* start_process(uint32_t=32, uint32_t=32);
void stop_process(process*);
process* process_from_rust(uint32_t*, uint32_t*, uint32_t, uint32_t*, uint32_t);

thread* allocate_thread(process*, uint32_t*, uint32_t=32, uint32_t=32);
void start_thread(process*, uint32_t, thread*, uint32_t=0);
uint8_t stop_thread(process*, uint32_t);
thread* thread_from_rust(process*, uint32_t*, uint32_t, uint32_t*, uint32_t, uint32_t);

extern "C" ProcessSerde* run(uint32_t*, uint32_t*, uint32_t, uint32_t*, uint32_t, uint32_t*, uint32_t, uint32_t, bool=false, bool=false, bool=false);

// core.cpp
uint8_t execute(thread*, uint32_t);

// util.cpp
void push(thread&, uint32_t);
uint32_t pop(thread&);
void print(process&);
void print(thread&);
template <typename TFunc, typename ... Args>
void timer(TFunc func, Args ... args) {
  auto begin = std::chrono::system_clock::now();
  func(args ...);
  auto end = std::chrono::system_clock::now();
  double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin).count();
  std::cout << "log | execution time: " << elapsed << " [millisecond]" << std::endl;
}

// jit.cpp
void jit_compile(thread&, size_t, std::string);
void native_load(thread&, size_t, const char*, const char*);
void native_execute(std::unordered_map<size_t, native>&, size_t, thread&);

#endif