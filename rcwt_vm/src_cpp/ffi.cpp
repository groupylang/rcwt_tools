#include "vm.h"

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
  auto t = allocate_thread(p, text, text_size);
  auto id = reinterpret_cast<size_t>(t);
  if (timer_on) timer(run_thread, p, id, t, entry_point);
  else run_thread(p, id, t, entry_point);
  return process_from_cpp(p->registers, p->data, p->data_size, &p->heap.front(), p->heap.capacity());
}