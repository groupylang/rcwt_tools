#include <algorithm>

#include "vm.h"

process* allocate_process(
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