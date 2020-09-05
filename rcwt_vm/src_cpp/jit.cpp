#include <fstream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux)
#include <dlfcn.h>
#endif

#include "vm.h"

template <typename ... Args>
inline std::string format(const char fmt[], Args ... args) {
  size_t len = std::snprintf(nullptr, 0, fmt, args ...);
  std::vector<char> buf(len + 1);
  std::snprintf(&buf[0], len + 1, fmt, args ...);
  return std::string(&buf[0], &buf[0] + len);
}

uint8_t is_hot(std::unordered_map<size_t, uint32_t>& hot_spots, size_t pc) {
  // if (hot_spots[pc] < 3) { hot_spots[pc]++; return 0; }
  // else { return 1; }
}

/// @arg id identifier of native procedure = address of virtual procedure's first instruction
void jit_compile(thread& t, size_t id, std::string program) {
  std::ofstream fout(format("tmp/jit%zu.cpp", id).c_str());
  fout << program;
  fout.flush();
#if defined(_WIN32) || defined(_WIN64)
  system(format("clang++ tmp/jit%zu.cpp -o tmp/jit%zu.dll -Wall -Wextra -g -shared -fPIC", id, id).c_str());
  native_load(t, id, "tmp/jit%zu.dll", "f");
#elif defined(__linux)
  system(format("clang++ tmp/jit%zu.cpp -o tmp/jit%zu.so -Wall -Wextra -g -shared -fPIC", id, id).c_str());
  native_load(t, id, "tmp/jit%zu.so", "f");
#endif
}

/// @arg id identifier of native procedure = address of virtual procedure's first instruction
/// @arg name name of native procedure
void native_load(thread& t, size_t id, const char* path, const char* name) {
#if defined(_WIN32) || defined(_WIN64)
  auto handle = LoadLibraryA(format("%s.dll", path).c_str());
#elif defined(__linux)
  auto handle = dlopen(format("%s.so", path).c_str(), RTLD_LAZY);
#endif
  if (!handle) {
    std::cout << "error | FileNotFound: " << path << std::endl;
    exit(1);
  }
#if defined(_WIN32) || defined(_WIN64)
  auto fn = reinterpret_cast<native>(GetProcAddress(handle, name));
#elif defined(__linux)
  auto fn = reinterpret_cast<native>(dlsym(handle, name));
#endif
  if (!fn) {
    std::cout << "error | SymbolNotFound: " << name << std::endl;
    exit(1);
  }
  t.natives[id] = fn;
}

void native_execute(std::unordered_map<size_t, native>& natives, size_t id, thread& t) {
  natives[id](t);
}