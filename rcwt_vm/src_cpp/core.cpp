#include "vm.h"

uint8_t execute(thread* t, uint32_t entry_point=0) {
#if defined __GNUC__ || defined __clang__ || defined __INTEL_COMPILER
  #define INIT_DISPATCH JUMP;
  #define CASE(opcode) L_ ## opcode:
  #define NEXT i=*++ip; goto *opcode_table[i.opcode]
  #define JUMP i=*ip; goto *opcode_table[i.opcode]
  #define END_DISPATCH
#else
  #define INIT_DISPATCH for (;;) { i=*ip; switch (i.opcode) {
  #define CASE(opcode) case opcode:
  #define NEXT ++ip; break
  #define JUMP break
  #define END_DISPATCH }}
#endif

  // instruction pointer
  instr* ip = reinterpret_cast<instr*>(t->text + entry_point);
  // current instruction
  instr i;

#if defined __GNUC__ || defined __clang__ || defined __INTEL_COMPILER
  static void* opcode_table[] = {
      /* 00 */ &&L_NOP,   /* 01 */ &&L_EXIT,  /* 02 */ &&L_BP, /* 03 */ &&L_CONST,
      /* 04 */ &&L_STORE, /* 05 */ &&L_LOAD,  /* 06 */ &&L_PUSH,  /* 07 */ &&L_POP,
      /* 08 */ &&L_NOP,   /* 09 */ &&L_NOP,   /* 0a */ &&L_NOP,   /* 0b */ &&L_NOP,
      /* 0c */ &&L_NOP,   /* 0d */ &&L_NOP,   /* 0e */ &&L_NOP,   /* 0f */ &&L_NOP,

      /* 10 */ &&L_ADDR,  /* 11 */ &&L_SUBR,  /* 12 */ &&L_MULR,  /* 13 */ &&L_DIVR,
      /* 14 */ &&L_REMR,  /* 15 */ &&L_GT,    /* 16 */ &&L_GE,    /* 17 */ &&L_EQ,
      /* 18 */ &&L_AND,   /* 19 */ &&L_OR,    /* 1a */ &&L_NOT,   /* 1b */ &&L_XOR,
      /* 1c */ &&L_SHL,   /* 1d */ &&L_SHR,   /* 1e */ &&L_NOP,   /* 1f */ &&L_NOP,

      /* 20 */ &&L_ADDI,  /* 21 */ &&L_SUBI,  /* 22 */ &&L_MULI,  /* 23 */ &&L_DIVI,
      /* 24 */ &&L_REMI,  /* 25 */ &&L_NOP,   /* 26 */ &&L_NOP,   /* 27 */ &&L_NOP,
      /* 28 */ &&L_ANDI,  /* 29 */ &&L_ORI,   /* 2a */ &&L_NOP,   /* 2b */ &&L_XORI,
      /* 2c */ &&L_SHLI,  /* 2d */ &&L_SHRI,  /* 2e */ &&L_NOP,   /* 2f */ &&L_NOP,

      /* 30 */ &&L_ADDA,  /* 31 */ &&L_SUBA,  /* 32 */ &&L_NOP,   /* 33 */ &&L_NOP,
      /* 34 */ &&L_NOP,   /* 35 */ &&L_NOP,   /* 36 */ &&L_NOP,   /* 37 */ &&L_NOP,
      /* 38 */ &&L_ANDA,  /* 39 */ &&L_ORA,   /* 3a */ &&L_NOP,   /* 3b */ &&L_NOP,
      /* 3c */ &&L_NOP,   /* 3d */ &&L_NOP,   /* 3e */ &&L_NOP,   /* 3f */ &&L_CASA,

      /* 40 */ &&L_GOTO,  /* 41 */ &&L_NOP,   /* 42 */ &&L_CALL,  /* 43 */ &&L_RET,
      /* 44 */ &&L_IFGT,  /* 45 */ &&L_IFGE,  /* 46 */ &&L_IFEQ,  /* 47 */ &&L_NOP,
      /* 48 */ &&L_NOP,   /* 49 */ &&L_NOP,   /* 4a */ &&L_NOP,   /* 4b */ &&L_NOP,  
      /* 4c */ &&L_NOP,   /* 4d */ &&L_NOP,   /* 4e */ &&L_NOP,   /* 4f */ &&L_NOP,

      /* 50 */ &&L_NEW,   /* 51 */ &&L_SET,   /* 52 */ &&L_GET,   /* 53 */ &&L_COPY,
      /* 54 */ &&L_NOP,   /* 55 */ &&L_NOP,   /* 56 */ &&L_NOP,   /* 57 */ &&L_NOP,
      /* 58 */ &&L_NOP,   /* 59 */ &&L_NOP,   /* 5a */ &&L_NOP,   /* 5b */ &&L_NOP,
      /* 5c */ &&L_NOP,   /* 5d */ &&L_NOP,   /* 5e */ &&L_NOP,   /* 5f */ &&L_NOP,

      /* 60 */ &&L_FADD,  /* 61 */ &&L_FSUB,  /* 62 */ &&L_FMUL,  /* 63 */ &&L_FDIV,
      /* 64 */ &&L_NOP,   /* 65 */ &&L_FGT,   /* 66 */ &&L_FGE,   /* 67 */ &&L_FEQ,
      /* 68 */ &&L_NOP,   /* 69 */ &&L_NOP,   /* 6a */ &&L_NOP,   /* 6b */ &&L_NOP,
      /* 6c */ &&L_NOP,   /* 6d */ &&L_NOP,   /* 6e */ &&L_NOP,   /* 6f */ &&L_NOP,

      /* 70 */ &&L_NOP,   /* 71 */ &&L_NOP,   /* 72 */ &&L_NOP,   /* 73 */ &&L_NOP,
      /* 74 */ &&L_NOP,   /* 75 */ &&L_NOP,   /* 76 */ &&L_NOP,   /* 77 */ &&L_NOP,
      /* 78 */ &&L_NOP,   /* 79 */ &&L_NOP,   /* 7a */ &&L_NOP,   /* 7b */ &&L_NOP,
      /* 7c */ &&L_NOP,   /* 7d */ &&L_NOP,   /* 7e */ &&L_NOP,   /* 7f */ &&L_NOP,

      /* 80 */ &&L_NOP,   /* 81 */ &&L_NOP,   /* 82 */ &&L_NOP,   /* 83 */ &&L_NOP,
      /* 84 */ &&L_NOP,   /* 85 */ &&L_NOP,   /* 86 */ &&L_NOP,   /* 87 */ &&L_NOP,
      /* 88 */ &&L_NOP,   /* 89 */ &&L_NOP,   /* 8a */ &&L_NOP,   /* 8b */ &&L_NOP,
      /* 8c */ &&L_NOP,   /* 8d */ &&L_NOP,   /* 8e */ &&L_NOP,   /* 8f */ &&L_NOP,

      /* 90 */ &&L_NOP,   /* 91 */ &&L_NOP,   /* 92 */ &&L_NOP,   /* 93 */ &&L_NOP,
      /* 94 */ &&L_NOP,   /* 95 */ &&L_NOP,   /* 96 */ &&L_NOP,   /* 97 */ &&L_NOP,
      /* 98 */ &&L_NOP,   /* 99 */ &&L_NOP,   /* 9a */ &&L_NOP,   /* 9b */ &&L_NOP,
      /* 9c */ &&L_NOP,   /* 9d */ &&L_NOP,   /* 9e */ &&L_NOP,   /* 9f */ &&L_NOP,

      /* a0 */ &&L_NOP,   /* a1 */ &&L_NOP,   /* a2 */ &&L_NOP,   /* a3 */ &&L_NOP,
      /* a4 */ &&L_NOP,   /* a5 */ &&L_NOP,   /* a6 */ &&L_NOP,   /* a7 */ &&L_NOP,
      /* a8 */ &&L_NOP,   /* a9 */ &&L_NOP,   /* aa */ &&L_NOP,   /* ab */ &&L_NOP,
      /* ac */ &&L_NOP,   /* ad */ &&L_NOP,   /* ae */ &&L_NOP,   /* af */ &&L_NOP,

      /* b0 */ &&L_NOP,   /* b1 */ &&L_NOP,   /* b2 */ &&L_NOP,   /* b3 */ &&L_NOP,
      /* b4 */ &&L_NOP,   /* b5 */ &&L_NOP,   /* b6 */ &&L_NOP,   /* b7 */ &&L_NOP,
      /* b8 */ &&L_NOP,   /* b9 */ &&L_NOP,   /* ba */ &&L_NOP,   /* bb */ &&L_NOP,
      /* bc */ &&L_NOP,   /* bd */ &&L_NOP,   /* be */ &&L_NOP,   /* bf */ &&L_NOP,

      /* c0 */ &&L_NOP,   /* c1 */ &&L_NOP,   /* c2 */ &&L_NOP,   /* c3 */ &&L_NOP,
      /* c4 */ &&L_NOP,   /* c5 */ &&L_NOP,   /* c6 */ &&L_NOP,   /* c7 */ &&L_NOP,
      /* c8 */ &&L_NOP,   /* c9 */ &&L_NOP,   /* ca */ &&L_NOP,   /* cb */ &&L_NOP,
      /* cc */ &&L_NOP,   /* cd */ &&L_NOP,   /* ce */ &&L_NOP,   /* cf */ &&L_NOP,

      /* d0 */ &&L_NOP,   /* d1 */ &&L_NOP,   /* d2 */ &&L_NOP,   /* d3 */ &&L_NOP,
      /* d4 */ &&L_NOP,   /* d5 */ &&L_NOP,   /* d6 */ &&L_NOP,   /* d7 */ &&L_NOP,
      /* d8 */ &&L_NOP,   /* d9 */ &&L_NOP,   /* da */ &&L_NOP,   /* db */ &&L_NOP,
      /* dc */ &&L_NOP,   /* dd */ &&L_NOP,   /* de */ &&L_NOP,   /* df */ &&L_NOP,

      /* e0 */ &&L_NOP,   /* e1 */ &&L_NOP,   /* e2 */ &&L_NOP,   /* e3 */ &&L_NOP,
      /* e4 */ &&L_NOP,   /* e5 */ &&L_NOP,   /* e6 */ &&L_NOP,   /* e7 */ &&L_NOP,
      /* e8 */ &&L_NOP,   /* e9 */ &&L_NOP,   /* ea */ &&L_NOP,   /* eb */ &&L_NOP,
      /* ec */ &&L_NOP,   /* ed */ &&L_NOP,   /* ee */ &&L_NOP,   /* ef */ &&L_NOP,

      /* f0 */ &&L_NOP,   /* f1 */ &&L_NOP,   /* f2 */ &&L_NOP,   /* f3 */ &&L_NOP,
      /* f4 */ &&L_NOP,   /* f5 */ &&L_NOP,   /* f6 */ &&L_NOP,   /* f7 */ &&L_NOP,
      /* f8 */ &&L_NOP,   /* f9 */ &&L_NOP,   /* fa */ &&L_IMM,   /* fb */ &&L_GOTOL,
      /* fc */ &&L_NCALL, /* fd */ &&L_NOP,   /* fe */ &&L_NOP,   /* ff */ &&L_NOP,
  };
#else
  #define NOP   0x00
  #define BP    0x02
  #define CONST 0x03
  #define STORE 0x04
  #define LOAD  0x05
  #define PUSH  0x06
  #define POP   0x07
  #define ADDR  0x10
  #define SUBR  0x11
  #define MULR  0x12
  #define DIVR  0x13
  #define REMR  0x14
  #define GT    0x15
  #define GE    0x16
  #define EQ    0x17
  #define AND   0x18
  #define OR    0x19
  #define NOT   0x1a
  #define XOR   0x1b
  #define SHL   0x1c
  #define SHR   0x1d
  #define ADDI  0x20
  #define SUBI  0x21
  #define MULI  0x22
  #define DIVI  0x23
  #define REMI  0x24
  #define ANDI  0x28
  #define ORI   0x29
  #define XORI  0x2b
  #define SHLI  0x2c
  #define SHRI  0x2d
  #define GOTO  0x40
  #define EXIT  0x41
  #define CALL  0x42
  #define RET   0x43
  #define IFGT  0x44
  #define IFGE  0x45
  #define IFEQ  0x46
  #define NEW   0x50
  #define SET   0x51
  #define GET   0x52
  #define COPY  0x53
  #define FADD  0x60
  #define FSUB  0x61
  #define FMUL  0x62
  #define FDIV  0x63
  #define FGT   0x65
  #define FGE   0x66
  #define FEQ   0x67
  #define IMM   0xfa
  #define GOTOL 0xfb
  #define NCALL 0xfc
#endif
  try {
  INIT_DISPATCH {
    CASE(NOP) {
      std::cout << "warning | nop (ip: " << reinterpret_cast<uint32_t*>(ip) - t->text << ")" << std::endl;
    } NEXT;
    CASE(EXIT) {
      return i.operand0;
    } NEXT;
    CASE(BP) {
      std::cout << "debug | break point" << std::endl;
      std::string input;
      while (true) {
        std::cout << "> ";
        std::cin >> input;
        if (input == "read") {
          std::cin >> input;
          if (input == "-t" || input == "--thread")  print(*t);
          if (input == "-p" || input == "--process") print(*t->parent);
        }
        if (input == "resume") break;
        if (input == "exit")  return 1;
      }
    } NEXT;

    CASE(CONST) {
      t->parent->registers[i.operand0] = t->parent->data[i.operand1];
    } NEXT;
    CASE(STORE) {
      t->stack[t->base_pointer + i.operand0] = t->parent->registers[i.operand1];
    } NEXT;
    CASE(LOAD) {
      t->parent->registers[i.operand0] = t->stack[t->base_pointer + i.operand1];
    } NEXT;
    CASE(PUSH) {
      push(*t, t->parent->registers[i.operand0]);
    } NEXT;
    CASE(POP) {
      t->parent->registers[i.operand0] = t->stack.back();
      t->stack.pop_back();
    } NEXT;

    CASE(ADDR) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] + t->parent->registers[i.operand2];
    } NEXT;
    CASE(SUBR) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] - t->parent->registers[i.operand2];
    } NEXT;
    CASE(MULR) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] * t->parent->registers[i.operand2];
    } NEXT;
    CASE(DIVR) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] / t->parent->registers[i.operand2];
    } NEXT;
    CASE(REMR) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] % t->parent->registers[i.operand2];
    } NEXT;
    CASE(GT) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] > t->parent->registers[i.operand2];
    } NEXT;
    CASE(GE) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] >= t->parent->registers[i.operand2];
    } NEXT;
    CASE(EQ) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] == t->parent->registers[i.operand2];
    } NEXT;
    CASE(AND) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] & t->parent->registers[i.operand2];
    } NEXT;
    CASE(OR) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] | t->parent->registers[i.operand2];
    } NEXT;
    CASE(NOT) {
      t->parent->registers[i.operand0] = !t->parent->registers[i.operand1];
    } NEXT;
    CASE(XOR) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] ^ t->parent->registers[i.operand2];
    } NEXT;
    CASE(SHL) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] << t->parent->registers[i.operand2];
    } NEXT;
    CASE(SHR) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] >> t->parent->registers[i.operand2];
    } NEXT;

    CASE(ADDI) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] + i.operand2;
    } NEXT;
    CASE(SUBI) {
    } NEXT;
    CASE(MULI) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] * i.operand2;
    } NEXT;
    CASE(DIVI) {
    } NEXT;
    CASE(REMI) {
    } NEXT;
    CASE(ANDI) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] & i.operand2;
    } NEXT;
    CASE(ORI) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] | i.operand2;
    } NEXT;
    CASE(XORI) {
      t->parent->registers[i.operand0] = t->parent->registers[i.operand1] ^ i.operand2;
    } NEXT;
    CASE(SHLI) {
    } NEXT;
    CASE(SHRI) {
    } NEXT;

#if defined __GNUC__ || defined __clang__ || defined __INTEL_COMPILER
    CASE(ADDA) {
      __sync_fetch_and_add(&t->parent->registers[i.operand0], t->parent->registers[i.operand1]);
    } NEXT;
    CASE(SUBA) {
      __sync_fetch_and_sub(&t->parent->registers[i.operand0], t->parent->registers[i.operand1]);
    } NEXT;
    CASE(ANDA) {
      __sync_fetch_and_and(&t->parent->registers[i.operand0], t->parent->registers[i.operand1]);
    } NEXT;
    CASE(ORA) {
      __sync_fetch_and_or(&t->parent->registers[i.operand0], t->parent->registers[i.operand1]);
    } NEXT;
    CASE(CASA) {
      __sync_bool_compare_and_swap(&t->parent->registers[i.operand0], t->parent->registers[i.operand1], t->parent->registers[i.operand2]);
    } NEXT;
#endif

    CASE(GOTO) {
      ip += i.operand0;
    } JUMP;
    CASE(CALL) {
      push(*t, t->base_pointer); // save bp to stack
      t->base_pointer = t->stack.size();
      // TODO allocate locals
      push(*t, reinterpret_cast<uint32_t*>(++ip) - t->text); // save ip to stack
      ip += i.operand0;
    } JUMP;
    CASE(RET) {
      ip = reinterpret_cast<instr*>(t->text + pop(*t)); // get pc from stack
    } JUMP;
    CASE(IFGT) {
      if (t->parent->registers[i.operand1] > t->parent->registers[i.operand2]) { ip += i.operand0; JUMP; }
    } NEXT;
    CASE(IFGE) {
      if (t->parent->registers[i.operand1] >= t->parent->registers[i.operand2]) { ip += i.operand0; JUMP; }
    } NEXT;
    CASE(IFEQ) {
      if (t->parent->registers[i.operand1] == t->parent->registers[i.operand2]) { ip += i.operand0; JUMP; }
    } NEXT;

    CASE(NEW) {
      auto offset = t->parent->heap.size();
      for (size_t k = 0; k < i.operand2; k++) t->parent->heap.push_back(0); // TODO
      t->parent->registers[i.operand0] = offset;
    } NEXT;
    CASE(SET) {
      t->parent->heap[t->parent->registers[i.operand0] + i.operand1] = t->parent->registers[i.operand2];
    } NEXT;
    CASE(GET) {
      t->parent->registers[i.operand0] = t->parent->heap[t->parent->registers[i.operand1] + i.operand2];
    } NEXT;
    CASE(COPY) {
    } NEXT;

    CASE(FADD) {
      *reinterpret_cast<float*>(&t->parent->registers[i.operand0])
        = *reinterpret_cast<float*>(&t->parent->registers[i.operand1])
        + *reinterpret_cast<float*>(&t->parent->registers[i.operand2]);
    } NEXT;
    CASE(FSUB) {
      *reinterpret_cast<float*>(&t->parent->registers[i.operand0])
        = *reinterpret_cast<float*>(&t->parent->registers[i.operand1])
        - *reinterpret_cast<float*>(&t->parent->registers[i.operand2]);
    } NEXT;
    CASE(FMUL) {
      *reinterpret_cast<float*>(&t->parent->registers[i.operand0])
        = *reinterpret_cast<float*>(&t->parent->registers[i.operand1])
        * *reinterpret_cast<float*>(&t->parent->registers[i.operand2]);
    } NEXT;
    CASE(FDIV) {
      *reinterpret_cast<float*>(&t->parent->registers[i.operand0])
        = *reinterpret_cast<float*>(&t->parent->registers[i.operand1])
        / *reinterpret_cast<float*>(&t->parent->registers[i.operand2]);
    } NEXT;
    CASE(FGT) {
      *reinterpret_cast<float*>(&t->parent->registers[i.operand0])
        = *reinterpret_cast<float*>(&t->parent->registers[i.operand1])
        > *reinterpret_cast<float*>(&t->parent->registers[i.operand2]);
    } NEXT;
    CASE(FGE) {
      *reinterpret_cast<float*>(&t->parent->registers[i.operand0])
        = *reinterpret_cast<float*>(&t->parent->registers[i.operand1])
        >= *reinterpret_cast<float*>(&t->parent->registers[i.operand2]);
    } NEXT;
    CASE(FEQ) {
      *reinterpret_cast<float*>(&t->parent->registers[i.operand0])
        = *reinterpret_cast<float*>(&t->parent->registers[i.operand1])
        == *reinterpret_cast<float*>(&t->parent->registers[i.operand2]);
    } NEXT;

    CASE(IMM) {
    } NEXT;
    CASE(GOTOL) {
    } NEXT;
    CASE(NCALL) {
      native_execute(t->natives, reinterpret_cast<size_t>(ip), *t);
    } NEXT;
  } END_DISPATCH;

  } catch (std::invalid_argument&) {
    std::cerr << "error | InvalidArgument" << std::endl;
    return 1;
  } catch (std::length_error&) {
    std::cerr << "error | VectorOutOfBounds" << std::endl;
    return 1;
  } catch (std::out_of_range&) {
    std::cerr << "error | ObjectTooLong" << std::endl;
    return 1;
  } catch (std::bad_alloc&) {
    std::cerr << "error | BadAlloc" << std::endl;
    return 1;
  } catch (std::overflow_error&) {
    std::cerr << "error | OverFlow" << std::endl;
    return 1;
  } catch (std::underflow_error&) {
    std::cerr << "error | UnderFlow" << std::endl;
    return 1;
  } catch (std::exception& e) {
    std::cerr << "error | " << e.what() << std::endl;
    return 1;
  }
}