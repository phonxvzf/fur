#ifndef TRACER_RAY_HPP
#define TRACER_RAY_HPP

#include <limits>

#include "math/vector.hpp"

namespace tracer {

  using namespace math;

  class ray {
    public:
      point3f origin;
      vector3f dir;
      vector3f inv_dir;
      Float t_max = std::numeric_limits<Float>::max();

      ray() {}
      ray(const point3f& origin, const vector3f& dir)
        : origin(origin), dir(dir), inv_dir(dir.inverse()) {}
      ray(const point3f& origin, const vector3f& dir, Float t_max)
        : origin(origin), dir(dir), inv_dir(dir.inverse()), t_max(t_max) {}
      ray(const ray& cpy)
        : origin(cpy.origin), dir(cpy.dir), inv_dir(cpy.inv_dir), t_max(cpy.t_max) {}

      ray& operator=(const ray& cpy) {
        origin  = cpy.origin;
        dir     = cpy.dir;
        inv_dir = cpy.inv_dir;
        t_max   = cpy.t_max;
        return *this;
      }

      point3f operator()(Float t) const {
        return origin + dir * t;
      }

      ray normalized() const {
        return ray(origin, dir.normalized(), t_max);
      }
  };
}

#endif /* TRACER_RAY_HPP */
