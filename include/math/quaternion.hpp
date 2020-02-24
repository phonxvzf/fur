#ifndef MATH_QUATERNION_HPP
#define MATH_QUATERNION_HPP

#include "math/matrix.hpp"

namespace math {
  class quat {
    public:
      Float a;
      vector3f v;

      quat(Float x, Float y, Float z, Float w);
      quat(Float a = 0, const vector3f& v = vector3f(0));
      quat(const quat& q);

      quat& operator=(const quat& q);
      quat operator+(const quat& q) const;
      quat operator-(const quat& q) const;
      quat operator*(const quat& q) const;
      quat operator/(Float s) const;

      Float size() const;
      Float size_sq() const;

      quat normalized() const;
      quat conjugate() const;
      quat t() const;
      quat inverse() const;

      matrix4f to_matrix() const;
  }; /* class quat */

  quat operator*(Float s, const quat& q);
  quat operator*(const quat& q, Float s);

} /* namespace math */

#endif /* MATH_QUATERNION_HPP */
