#ifndef MATH_QUATERNION_HPP
#define MATH_QUATERNION_HPP

#include "math/matrix.hpp"

namespace math {
  class quat {
    public:
      Float a;
      vector3f v;

      quat(Float x, Float y, Float z, Float w) : a(x), v({ y, z, w }) {}
      quat(Float a = 0, const vector3f& v = vector3f(0)) : a(a), v(v) {}
      quat(const quat& q) : a(q.a), v(q.v) {}

      quat& operator=(const quat& q) {
        a = q.a;
        v = q.v;
        return *this;
      }

      quat operator+(const quat& q) const {
        return quat(a + q.a, v + q.v);
      }

      quat operator-(const quat& q) const {
        return quat(a - q.a, v - q.v);
      }

      quat operator*(const quat& q) const {
        return quat(a * q.a - v.dot(q.v), a * q.v + q.a * v + v.cross(q.v));
      }

      quat operator/(Float s) const {
        assert(!COMPARE_EQ(s, 0));
        s = 1 / s;
        return quat(a * s, v * s);
      }

      Float size() const {
        return std::sqrt(a * a + v.size_sq());
      }

      Float size_sq() const {
        return a * a + v.size_sq();
      }

      quat normalized() const {
        Float sz = size();
        assert(!COMPARE_EQ(sz, 0));
        return *this / sz;
      }

      quat conjugate() const {
        return quat(a, -v);
      }

      quat t() const {
        return conjugate();
      }

      quat inverse() const {
        Float sz = size_sq();
        assert(!COMPARE_EQ(sz, 0));
        return conjugate() / sz;
      }

      matrix4f to_matrix() const {
        const quat uq(normalized());
        const Float r = uq.a;
        const Float x = uq.v.x, y = uq.v.y, z = uq.v.z;
        return matrix4f(
            { 1 - 2 * (y * y + z * z), 2 * (x * y + z * r), 2 * (x * z - y * r), 0 },
            { 2 * (x * y - z * r), 1 - 2 * (x * x + z * z), 2 * (y * z + x * r), 0 },
            { 2 * (x * z + y * r), 2 * (y * z + x * r), 1 - 2 * (x * x + y * y), 0 },
            { 0, 0, 0, 1 }
            );
      }
  }; /* class quat */

  quat operator*(Float s, const quat& q) {
    return quat(q.a * s, q.v * s);
  }

  quat operator*(const quat& q, Float s) {
    return quat(q.a * s, q.v * s);
  }
} /* namespace math */

#endif /* MATH_QUATERNION_HPP */
