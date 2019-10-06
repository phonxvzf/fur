#ifndef TRACER_RAY_HPP
#define TRACER_RAY_HPP

#include "math/vector.hpp"

namespace tracer {

  using namespace math;

  template <typename T> class ray;

  typedef ray<Float> rayf;

  template <typename T>
    class ray {
      public:
        const vector3<T> origin;
        const vector3<T> dir;

        ray(const vector3<T>& origin, const vector3<T>& dir) : origin(origin), dir(dir) {}
        ray(const ray& cpy) : origin(cpy.origin), dir(cpy.origin) {}

        vector3<T> operator()(T t) {
          return origin + dir * t;
        }
    };
}

#endif /* TRACER_RAY_HPP */
