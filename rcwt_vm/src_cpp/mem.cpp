#include "mem.h"

#include <algorithm>
#include <mutex>

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
  p->threads = std::unordered_map<uint32_t, std::future<uint8_t>>();
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
  p->threads = std::unordered_map<uint32_t, std::future<uint8_t>>();
  p->threads.reserve(32);

  return p;
}

#include <iostream>

void start_thread(
  uint32_t id,
  process* parent,
  uint32_t* text,
  uint32_t text_size,
  uint32_t stack_capacity,
  uint32_t entry_point
) {
  // allocate memory to thread
  auto t = new thread;
  t->parent = parent;
  t->text = text;
  t->text_size = text_size;
  t->stack = std::vector<uint32_t>();
  t->stack.reserve(stack_capacity);
  t->base_pointer = 0;
  // create native thread
  auto native_thread = std::async(std::launch::async, execute, t, entry_point);
  // add to children thread list
  parent->threads[id] = std::move(native_thread);
}

uint8_t stop_thread(
  uint32_t id,
  process* parent
) {
  // destroy native thread
  auto status = parent->threads[id].get();
  // TODO free memory from thread
  // remove from children thread list
  parent->threads.erase(id);
  return status;
}

void thread_from_rust(
  uint32_t id,
  process* parent,
  uint32_t* text,
  uint32_t text_size,
  uint32_t* stack,
  uint32_t stack_capacity,
  uint32_t base_pointer,
  uint32_t entry_point
) {
  auto t = new thread;
  t->parent = parent;
  t->text = text;
  t->text_size = text_size;
  t->stack = std::vector<uint32_t>();
  t->stack.reserve(stack_capacity);
  std::copy(stack, stack + stack_capacity, t->stack.begin());
  t->base_pointer = base_pointer;
  // create native thread
  auto native_thread = std::async(std::launch::async, execute, t, entry_point);
  // add to children thread list
  parent->threads[id] = std::move(native_thread);
}

ProcessSerde* run(
  uint32_t* registers,
  uint32_t* data,
  uint32_t data_size,
  uint32_t* heap,
  uint32_t heap_capacity,
  uint32_t* text,
  uint32_t text_size,
  uint32_t entry_point
) {
  auto p = process_from_rust(registers, data, data_size, heap, heap_capacity);
  start_thread(0, p, text, text_size, 32, entry_point);
  auto status = stop_thread(0, p);
  std::cout << "log | exit status: " << static_cast<uint32_t>(status) << std::endl;
  return process_from_cpp(p->registers, p->data, p->data_size, &p->heap[0], p->heap.capacity());
}