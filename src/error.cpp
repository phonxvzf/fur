#include "error.hpp"

#include <iostream>
#include <cstdlib>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

void ASSERT(bool cond) {
#ifndef NDEBUG
  if (cond) return;
  std::cerr << std::endl << "ASSERTION FAILED:" << std::endl;
  void* frames[32];
  size_t size = backtrace(frames, 32);
  backtrace_symbols_fd(frames, size, STDERR_FILENO);
  std::exit(1);
#endif
}
