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

enum color {
  NORMAL=0,
  RED=31,
  GREEN,
  BROWN,
  BLUE,
  PURPLE,
  SKY,
  WHITE
};

void change_font_color(enum color c) {
  printf("\x1b[%dm", c);
}

void print_u8array(uint8_t* array, size_t length) {
  change_font_color(color::RED);
  std::cout << "            00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" << std::endl;
  change_font_color(color::NORMAL);
  for (size_t k = 0; k < length; k++) {
    if (k % 16 == 0) {
      change_font_color(color::RED);
      printf("0x%08x: ", k);
      change_font_color(color::NORMAL);
    }
    if (k % 16 == 15 || k + 1 == length) printf("%02x\r\n", array[k]);
    else printf("%02x ", array[k]);
  }
}

void print_u32array(uint32_t* array, size_t length) {
  change_font_color(color::RED);
  std::cout << "            00010203 04050607 08090A0B 0C0D0E0F" << std::endl;
  change_font_color(color::NORMAL);
  for (size_t k = 0; k < length; k++) {
    if (k % 4 == 0) {
      change_font_color(color::RED);
      printf("0x%08x: ", k*4);
      change_font_color(color::NORMAL);
    }
    if (k % 4 == 3 || k + 1 == length) printf("%08x\r\n", array[k]);
    else printf("%08x ", array[k]);
  }
}

void print_u32array_pretty(uint32_t* array, size_t length) {
  for (size_t k = 0; k < length; k++) {
    change_font_color(color::RED);
    printf("0x%08x: ", k);
    change_font_color(color::NORMAL);
    auto l = reinterpret_cast<uint8_t*>(array+k);
    printf("%02x %02x %02x %02x  ", l[0], l[1], l[2], l[3]);
    change_font_color(color::GREEN);
    std::cout << array[k] << std::endl;
    change_font_color(color::NORMAL);
  }
}

void print_instr(instr& i) {
  switch (i.opcode) {
    case 0x00: printf("NOP"); break;
    case 0x01: printf("EXIT  %d",                                        i.operand0); break;
    case 0x02: printf("BP"); break;
    case 0x03: printf("CONST registers[%d] data[%d]",                    i.operand0, i.operand1); break;
    case 0x04: printf("STORE stack[bp+%d] registers[%d]",               i.operand0, i.operand1); break;
    case 0x05: printf("LOAD  registers[%d] stack[bp+%d]",                i.operand0, i.operand1); break;
    case 0x06: printf("PUSH  registers[%d]",                             i.operand0); break;
    case 0x07: printf("POP   registers[%d]",                             i.operand0); break;
    case 0x10: printf("ADDR  registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x11: printf("SUBR  registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x12: printf("MULR  registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x13: printf("DIVR  registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x14: printf("REMR  registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x15: printf("GT    registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x16: printf("GE    registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x17: printf("EQ    registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x18: printf("AND   registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x19: printf("OR    registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x1a: printf("NOT   registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x1b: printf("XOR   registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x1c: printf("SHL   registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x1d: printf("SHR   registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x20: printf("ADDI  registers[%d] registers[%d] %d",            i.operand0, i.operand1, i.operand2); break;
    case 0x22: printf("MULI  registers[%d] registers[%d] %d",            i.operand0, i.operand1, i.operand2); break;
    case 0x28: printf("ANDI  registers[%d] registers[%d] %d",            i.operand0, i.operand1, i.operand2); break;
    case 0x29: printf("ORI   registers[%d] registers[%d] %d",            i.operand0, i.operand1, i.operand2); break;
    case 0x2b: printf("XORI  registers[%d] registers[%d] %d",            i.operand0, i.operand1, i.operand2); break;
    case 0x40: printf("GOTO  ip+%d",                                     i.operand0); break;
    case 0x42: printf("CALL  ip+%d",                                     i.operand0); break;
    case 0x43: printf("RET"); break;
    case 0x44: printf("IFGT  ip+%d registers[%d] registers[%d]",         i.operand0, i.operand1, i.operand2); break;
    case 0x45: printf("IFGE  ip+%d registers[%d] registers[%d]",         i.operand0, i.operand1, i.operand2); break;
    case 0x46: printf("IFEQ  ip+%d registers[%d] registers[%d]",         i.operand0, i.operand1, i.operand2); break;
    case 0x50: printf("NEW   registers[%d] %d",                          i.operand0, i.operand1); break;
    case 0x51: printf("SET   heap[registers[%d]+%d] registers[%d]",      i.operand0, i.operand1, i.operand2); break;
    case 0x52: printf("GET   registers[%d] heap[registers[%d]+%d]",      i.operand0, i.operand1, i.operand2); break;
    case 0x60: printf("FADD  registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x61: printf("FSUB  registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x62: printf("FMUL  registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x63: printf("FDIV  registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x65: printf("FGT   registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x66: printf("FGE   registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0x67: printf("FEQ   registers[%d] registers[%d] registers[%d]", i.operand0, i.operand1, i.operand2); break;
    case 0xfc: printf("NCALL"); break;
    default: break;
  }
  std::cout << std::endl;
}

void print_text(instr* text, size_t length) {
  for (size_t k = 0; k < length; k++) {
    change_font_color(color::RED);
    printf("0x%08x: ", k);
    change_font_color(color::NORMAL);
    printf("%02x %02x %02x %02x  ", text[k].opcode, text[k].operand0, text[k].operand1, text[k].operand2);
    change_font_color(color::GREEN);
    print_instr(text[k]);
    change_font_color(color::NORMAL);
  }
}

void print(process& p) {
  std::cout << "<process>" << std::endl;
  std::cout << "<data>" << std::endl;
  print_u32array_pretty(p.data, p.data_size);
  std::cout << "<heap>" << std::endl;
  if (!p.heap.empty()) {
    print_u32array_pretty(&p.heap.front(), p.heap.size());
  }
  std::cout << "<registers>" << std::endl;
  print_u32array_pretty(p.registers, 256);
}

void print(thread& t) {
  std::cout << "<thread>" << std::endl;
  std::cout << "<text>" << std::endl;
  // print_u8array(reinterpret_cast<uint8_t*>(t.text), t.text_size*4);
  print_text(reinterpret_cast<instr*>(t.text), t.text_size);
  std::cout << "<stack>" << std::endl;
  if (!t.stack.empty()) {
    print_u32array_pretty(&t.stack.front(), t.stack.size());
  }
  std::cout << "<base pointer>" << std::endl
            << t.base_pointer << std::endl;
}