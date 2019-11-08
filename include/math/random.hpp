#ifndef MATH_RANDOM_HPP
#define MATH_RANDOM_HPP

#include <cinttypes>
#include <limits>

#include "vector.hpp"

namespace math {
  namespace random {

    extern const Float INV_UINT_MAX;

    inline Float normalize(uint32_t num) {
      if (num == 0) return 0;
      return (num - 1u) * INV_UINT_MAX;
    }

    // PCG-XSH-RR
    class rng {
      private:
        uint64_t seed;
        uint64_t state;

        static const uint64_t MULTIPLIER = 6364136223846793005u;
        static const uint64_t INCREMENT  = 1442695040888963407u;

        inline uint32_t rotate32(uint32_t x, uint16_t r) {
          return x >> r | x << (-r & 31);
        }

      public:
        rng(uint64_t seed = 0);
        rng(const rng& cpy);

        uint32_t  next_ui();
        vector2i  next_2ui();
        Float     next_uf();
        vector2f  next_2uf();
    };
  }
}

#endif /* MATH_RANDOM_HPP */
