#ifndef VM_MEM_H
#define VM_MEM_H

#include <stdint.h>
#include <vector>
#include <future>
#include <unordered_map>

typedef struct _process process;
typedef struct _thread thread;
typedef struct _instr instr;

struct _process {
  uint32_t* registers;
  uint32_t* data; // constant pool
  uint32_t data_size;
  std::vector<uint32_t> heap;
  std::unordered_map<uint32_t, std::future<uint8_t>> threads;
};

struct _thread {
  uint32_t* text;
  uint32_t text_size;
  std::vector<uint32_t> stack;
  uint32_t base_pointer;
  process* parent;
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

void start_thread(uint32_t, process*, uint32_t*, uint32_t=32, uint32_t=32, uint32_t=0);
uint8_t stop_thread(uint32_t, process*);
void thread_from_rust(uint32_t, process*, uint32_t*, uint32_t, uint32_t*, uint32_t, uint32_t, uint32_t);

extern "C" ProcessSerde* run(uint32_t*, uint32_t*, uint32_t, uint32_t*, uint32_t, uint32_t*, uint32_t, uint32_t);

// core.cpp
uint8_t execute(thread*, uint32_t);

// util.cpp
void push(thread*, uint32_t);
uint32_t pop(thread*);
#endif