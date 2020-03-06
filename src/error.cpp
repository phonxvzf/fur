#include "error.hpp"

#include <iostream>
#include <cstdlib>
#include <execinfo.h>
#include <unistd.h>
#include <mutex>

std::mutex assert_log_mutex;

void ASSERT(bool cond, const char* message) {
  if (cond) return;
  std::lock_guard<std::mutex> lock(assert_log_mutex);
  std::cerr << std::endl << "ASSERTION FAILED: " << (message ? message : "") << std::endl;
  void* frames[32];
  size_t size = backtrace(frames, 32);
  backtrace_symbols_fd(frames, size, STDERR_FILENO);
  std::abort();
}
