#include "math/random.hpp"

namespace math {
  namespace random {
    rng::rng(uint64_t seed) : seed(seed), state(seed + INCREMENT) {
      next_ui();
    }

    rng::rng(const rng& cpy) : seed(cpy.seed), state(cpy.state) {}

    uint32_t rng::next_ui() {
      uint64_t old_state = state;
      uint16_t count = static_cast<uint16_t>(old_state >> 59);
      state = old_state * MULTIPLIER + INCREMENT;
      old_state ^= old_state >> 18;
      return rotate32(static_cast<uint32_t>(old_state >> 27), count);
    }

    point2i rng::next_2ui() {
      return vector2i(next_ui(), next_ui());
    }

    Float rng::next_uf() {
      return normalize(static_cast<int>(next_ui()));
    }

    point2f rng::next_2uf() {
      return { next_uf(), next_uf() };
    }
    
    const Float INV_UINT_MAX = 1.0 / std::numeric_limits<uint32_t>::max();
  }
}
