#include "error.hpp"

#include <iostream>
#include <cstdlib>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

void ASSERT(bool cond) {
  if (cond) return;
  std::cerr << "ASSERTION ERROR:" << std::endl;
  void* frames[32];
  size_t size = backtrace(frames, 32);
  backtrace_symbols_fd(frames, size, STDERR_FILENO);
  std::exit(1);
}
