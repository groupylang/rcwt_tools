#include <chrono>

#include "vm.h"

// push to stack
void push(thread& t, uint32_t item) {
  t.stack.push_back(item);
}

// pop from stack
uint32_t pop(thread& t) {
  auto item = t.stack.back();
  t.stack.pop_back();
  return item;
}

void print_u8array(uint8_t* array, size_t length) {
  for (size_t k = 0; k < length; k++) {
    if (k % 16 == 15 || k + 1 == length) printf("%02x\r\n", array[k]);
    else printf("%02x ", array[k]);
  }
}

void print_u32array(uint32_t* array, size_t length) {
  for (size_t k = 0; k < length; k++) {
    if (k % 4 == 3 || k + 1 == length) printf("%08x\r\n", array[k]);
    else printf("%08x ", array[k]);
  }
}

void print(process& p) {
  std::cout << "<process>" << std::endl;
  std::cout << "<data>" << std::endl;
  print_u32array(p.data, p.data_size);
  std::cout << "<stack>" << std::endl;
  if (!p.heap.empty()) {
    print_u32array(&p.heap.front(), p.heap.size());
  }
  std::cout << "<registers>" << std::endl;
  print_u32array(p.registers, 256);
}

void print(thread& t) {
  std::cout << "<thread>" << std::endl;
  std::cout << "<text>" << std::endl;
  print_u8array(reinterpret_cast<uint8_t*>(t.text), t.text_size*4);
  std::cout << "<stack>" << std::endl;
  if (!t.stack.empty()) {
    print_u32array(&t.stack.front(), t.stack.size());
  }
  std::cout << "<base pointer>" << std::endl
            << t.base_pointer << std::endl;
}