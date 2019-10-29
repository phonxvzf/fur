#ifndef TRACER_RAY_HPP
#define TRACER_RAY_HPP

#include <limits>

#include "math/vector.hpp"

namespace tracer {

  using namespace math;

  class ray {
    public:
      const vector3f origin;
      const vector3f dir;
      const Float t_max = std::numeric_limits<Float>::max();

      ray(const vector3f& origin, const vector3f& dir) : origin(origin), dir(dir) {}
      ray(const vector3f& origin, const vector3f& dir, Float t_max)
        : origin(origin), dir(dir), t_max(t_max) {}
      ray(const ray& cpy) : origin(cpy.origin), dir(cpy.origin), t_max(cpy.t_max) {}

      vector3f operator()(Float t) {
        return origin + dir * t;
      }

      ray normalized() const {
        return ray(origin, dir.normalized(), t_max);
      }
  };
}

#endif /* TRACER_RAY_HPP */
