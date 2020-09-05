#include <algorithm>

#include "vm.h"

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
  size_t id,
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
  size_t id
) {
  // destroy native thread
  auto status = parent->threads[id].get();
  // TODO free memory from thread
  // remove from children thread list
  parent->threads.erase(id);
  return status;
}

// start & stop
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