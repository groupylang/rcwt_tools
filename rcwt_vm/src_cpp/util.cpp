#include "mem.h"

// push to stack
void push(thread* t, uint32_t item) {
  t->stack.push_back(item);
}

// pop from stack
uint32_t pop(thread* t) {
  auto item = t->stack.back();
  t->stack.pop_back();
  return item;
}