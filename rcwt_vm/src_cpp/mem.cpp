#include <algorithm>
#include <mutex>

#include "vm.h"

process* start_process(
  uint32_t data_size,
  uint32_t heap_capacity
) {
  auto p = new process;
  p->registers = reinterpret_cast<uint32_t*>(calloc(256, 4));
  p->data = reinterpret_cast<uint32_t*>(calloc(data_size, 4));
  p->data_size = data_size;
  p->heap = std::vector<uint32_t>();
  p->heap.reserve(heap_capacity);
  p->threads = std::unordered_map<size_t, std::future<uint8_t>>();
  p->threads.reserve(32);
  return p;
}

void stop_process(
  process* p
) {
  delete p;
}

process* process_from_rust(
  uint32_t* registers,
  uint32_t* data,
  uint32_t data_size,
  uint32_t* heap,
  uint32_t heap_capacity
) {
  auto p = new process;
  p->registers = registers;
  p->data = data;
  p->data_size = data_size;
  p->heap = std::vector<uint32_t>();
  p->heap.reserve(heap_capacity);
  std::copy(heap, heap + heap_capacity, p->heap.begin());
  p->threads = std::unordered_map<size_t, std::future<uint8_t>>();
  p->threads.reserve(32);

  return p;
}

// allocate memory to thread
thread* allocate_thread(
  process* parent,
  uint32_t* text,
  uint32_t text_size,
  uint32_t stack_capacity
) {
  auto t = new thread;
  t->parent = parent;
  t->text = text;
  t->text_size = text_size;
  t->stack = std::vector<uint32_t>();
  t->stack.reserve(stack_capacity);
  t->base_pointer = 0;
  t->natives = std::unordered_map<size_t, native>();
  t->natives.reserve(32);
  return t;
}

void start_thread(
  process* parent,
  uint32_t id,
  thread* t,
  uint32_t entry_point
) {
  // create native thread
  auto native_thread = std::async(std::launch::async, execute, t, entry_point);
  // add to children thread list
  parent->threads[id] = std::move(native_thread);
}

uint8_t stop_thread(
  process* parent,
  uint32_t id
) {
  // destroy native thread
  auto status = parent->threads[id].get();
  // TODO free memory from thread
  // remove from children thread list
  parent->threads.erase(id);
  return status;
}

thread* thread_from_rust(
  process* parent,
  uint32_t* text,
  uint32_t text_size,
  uint32_t* stack,
  uint32_t stack_capacity,
  uint32_t base_pointer
) {
  auto t = new thread;
  t->parent = parent;
  t->text = text;
  t->text_size = text_size;
  t->stack = std::vector<uint32_t>();
  t->stack.reserve(stack_capacity);
  std::copy(stack, stack + stack_capacity, t->stack.begin());
  t->base_pointer = base_pointer;
  t->natives = std::unordered_map<size_t, native>();
  t->natives.reserve(32);
  return t;
}

void run_thread(
  process* p,
  size_t id,
  thread* t,
  uint32_t entry_point
) {
  start_thread(p, id, t, entry_point);
  auto status = stop_thread(p, id);
  std::cout << "log | exit status: " << static_cast<uint32_t>(status) << std::endl;
}

ProcessSerde* run(
  uint32_t* registers,
  uint32_t* data,
  uint32_t data_size,
  uint32_t* heap,
  uint32_t heap_capacity,
  uint32_t* text,
  uint32_t text_size,
  uint32_t entry_point,
  bool timer_on,
  bool jit_on,
  bool gc_on
) {
  auto p = process_from_rust(registers, data, data_size, heap, heap_capacity);
  auto t = allocate_thread(p, text, text_size, 32);
  auto id = reinterpret_cast<size_t>(t);
  if (timer_on) timer(run_thread, p, id, t, entry_point);
  else run_thread(p, id, t, entry_point);
  return process_from_cpp(p->registers, p->data, p->data_size, &p->heap.front(), p->heap.capacity());
}