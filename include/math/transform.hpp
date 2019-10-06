#ifndef MATH_TRANSFORM_HPP
#define MATH_TRANSFORM_HPP

#include "math/quaternion.hpp"

namespace math {
  namespace tf {
    inline matrix4f translate(const vector3f& ds) {
      return matrix4f(
          { 1, 0, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 0, 1, 0 },
          vector4f(ds, 1)
          );
    }

    inline matrix4f scale(const vector3f& ss) {
      return matrix4f(
          { ss.x, 0, 0, 0 },
          { 0, ss.y, 0, 0 },
          { 0, 0, 0, ss.z },
          { 0, 0, 0, 1 }
          );
    }

    inline matrix4f rotate(const vector3f& axis_n, Float rad) {
      return quat(std::cos(rad / 2), std::sin(rad / 2) * axis_n).to_matrix();
    }
  }
}

#endif /* MATH_TRANSFORM_HPP */
