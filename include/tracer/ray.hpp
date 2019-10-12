#ifndef TRACER_RAY_HPP
#define TRACER_RAY_HPP

#include "math/vector.hpp"

namespace tracer {

  using namespace math;

  class ray {
    public:
      const vector3f origin;
      const vector3f dir;

      ray(const vector3f& origin, const vector3f& dir) : origin(origin), dir(dir) {}
      ray(const ray& cpy) : origin(cpy.origin), dir(cpy.origin) {}

      vector3f operator()(Float t) {
        return origin + dir * t;
      }
  };
}

#endif /* TRACER_RAY_HPP */
