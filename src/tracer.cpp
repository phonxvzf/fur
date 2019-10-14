#include <cstdio>
#include "tracer/spectrum.hpp"

int main(int argc, char** argv) {
  for (int i = 0; i < 60; ++i) {
    printf("%f\n", tracer::X_SPECTRUM[i]);
  }

  printf("\n");

  for (int i = 0; i < 60; ++i) {
    printf("%f\n", tracer::Y_SPECTRUM[i]);
  }

  printf("\n");

  for (int i = 0; i < 60; ++i) {
    printf("%f\n", tracer::Z_SPECTRUM[i]);
  }

  return 0;
}
