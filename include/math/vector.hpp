#ifndef MATH_VECTOR_HPP
#define MATH_VECTOR_HPP

#include <string>
#include <cmath>
#include <iostream>

#include "error.hpp"
#include "float.hpp"

namespace math {
  template <typename T> class vector2;
  template <typename T> class vector3;
  template <typename T> class vector4;

  template <typename T> class point2 : public vector2<T> {
    public:
      point2(const vector2<T>& v) : vector2<T>(v) {}
      point2(T x = 0, T y = 0) : vector2<T>(x, y) {}
  };

  template <typename T> class point3 : public vector3<T> {
    public:
      point3(const vector3<T>& v) : vector3<T>(v) {}
      point3(T x = 0, T y = 0, T z = 0) : vector3<T>(x, y, z) {}
  };

  template <typename T> class point4 : public vector4<T> {
    public:
      point4(const vector4<T>& v) : vector4<T>(v) {}
      point4(T x = 0, T y = 0, T z = 0, T w = 0) : vector4<T>(x, y, z, w) {}
  };

  template <typename T> class normal2 : public vector2<T> {
    public:
      normal2(const vector2<T>& v) : vector2<T>(v) {}
      normal2(T x = 0, T y = 0) : vector2<T>(x, y) {}
  };

  template <typename T> class normal3 : public vector3<T> {
    public:
      normal3(const vector3<T>& v) : vector3<T>(v) {}
      normal3(T x = 0, T y = 0, T z = 0) : vector3<T>(x, y, z) {}
  };

  template <typename T> class normal4 : public vector4<T> {
    public:
      normal4(const vector4<T>& v) : vector4<T>(v) {}
      normal4(T x = 0, T y = 0, T z = 0, T w = 0) : vector4<T>(x, y, z, w) {}
  };

  typedef vector2<Float>  vector2f;
  typedef vector2<int>    vector2i;
  typedef point2<Float>   point2f;
  typedef point2<int>     point2i;
  typedef normal2<Float>  normal2f;
  typedef normal2<int>    normal2i;

  typedef vector3<Float>  vector3f;
  typedef vector3<int>    vector3i;
  typedef point3<Float>   point3f;
  typedef point3<int>     point3i;
  typedef normal3<Float>  normal3f;
  typedef normal3<int>    normal3i;

  typedef vector4<Float>  vector4f;
  typedef vector4<int>    vector4i;
  typedef point4<Float>   point4f;
  typedef point4<int>     point4i;
  typedef normal4<Float>  normal4f;
  typedef normal4<int>    normal4i;

  template <typename T, typename U>
    vector2<T> operator*(U s, const vector2<T> vec) {
      return { vec.x * s, vec.y * s };
    }

  template <typename T, typename U>
    vector2<T> operator*(const vector2<T> vec, U s) {
      return { vec.x * s, vec.y * s };
    }

  template <typename T, typename U>
    vector3<T> operator*(U s, const vector3<T> vec) {
      return { vec.x * s, vec.y * s, vec.z * s };
    }

  template <typename T, typename U>
    vector3<T> operator*(const vector3<T> vec, U s) {
      return { vec.x * s, vec.y * s, vec.z * s };
    }

  template <typename T, typename U>
    vector4<T> operator*(U s, const vector4<T> vec) {
      return { vec.x * s, vec.y * s, vec.z * s, vec.w * s };
    }

  template <typename T, typename U>
    vector4<T> operator*(const vector4<T> vec, U s) {
      return { vec.x * s, vec.y * s, vec.z * s, vec.w * s};
    }

  template <typename T>
    class vector2 {
      public:
        T x, y;
        vector2(T x, T y) : x(x), y(y) {}
        vector2(T x = 0) : x(x), y(x) {}
        vector2(const vector2& cpy) : x(cpy.x), y(cpy.y) {}
        vector2(const vector3<T>& vec3) : x(vec3.x), y(vec3.y) {}
        vector2(const vector4<T>& vec4) : x(vec4.x), y(vec4.y) {}

        template <typename U>
          vector2(const vector2<U>& vu) :
            x(static_cast<T>(vu.x)), y(static_cast<T>(vu.y)) {}

        vector2& operator=(const vector2& cpy) {
          x = cpy.x; y = cpy.y;
          return *this;
        }

        vector2 operator+(const vector2& rhs) const {
          return { x + rhs.x, y + rhs.y };
        }

        vector2 operator-(const vector2& rhs) const {
          return { x - rhs.x, y - rhs.y };
        }

        vector2 operator*(const vector2& rhs) const {
          return { x * rhs.x, y * rhs.y };
        }

        vector2 operator/(T s) const {
          ASSERT(!COMPARE_EQ(s, 0.0f));
          Float inv = 1.0f / s;
          return vector2f(*this) * inv;
        }

        vector2 operator/(const vector2& rhs) const {
          ASSERT(!rhs.is_zero());
          return { x / rhs.x, y / rhs.y };
        }

        vector2 operator-() const {
          return -1 * (*this);
        }

        vector2& operator+=(const vector2& vec) {
          x += vec.x;
          y += vec.y;
          return *this;
        }

        vector2& operator-=(const vector2& vec) {
          x -= vec.x;
          y -= vec.y;
          return *this;
        }

        vector2& operator*=(T s) {
          x *= s;
          y *= s;
          return *this;
        }

        vector2& operator/=(T s) {
          ASSERT(!COMPARE_EQ(s, 0.0f));
          Float inv = 1.0f / s;
          x *= inv;
          y *= inv;
          return *this;
        }

        T& operator[](int i) {
          ASSERT(i >= 0 && i < 2);
          switch (i) {
            case 0:
              return x;
            default:
              return y;
          }
        }

        T operator[](int i) const {
          ASSERT(i >= 0 && i < 2);
          switch (i) {
            case 0:
              return x;
            default:
              return y;
          }
        }

        T dot(const vector2& rhs) const {
          return x * rhs.x + y * rhs.y;
        }

        Float size() const {
          return std::sqrt(x * x + y * y);
        }

        Float size_sq() const {
          return x * x + y * y;
        }

        bool is_zero() const {
          return COMPARE_EQ(x, 0.0f) && COMPARE_EQ(y, 0.0f);
        }

        vector2 clamped(T min_val, T max_val) const {
          return vector2(math::clamp(x, min_val, max_val), math::clamp(y, min_val, max_val));
        }

        vector2 normalized() const {
          Float sz = this->size();
          ASSERT(!COMPARE_EQ(sz, 0.0f));
          return *this / sz;
        }

        std::string to_string() const {
          return "[\t" + std::to_string(x) + "\t" + std::to_string(y) + "\t]";
        }

        std::wstring to_wstring() const {
          return L"[\t" + std::to_wstring(x) + L"\t" + std::to_wstring(y) + L"\t]";
        }

        friend std::ostream& operator<<(std::ostream& os, const vector2& vec) {
          return os << vec.to_string();
        }

        friend std::wostream& operator<<(std::wostream& os, const vector2& vec) {
          return os << vec.to_wstring();
        }
    }; /* class vector2 */

  template <typename T>
    class vector3 {
      public:
        T x, y, z;
        vector3(T x, T y, T z) : x(x), y(y), z(z) {}
        vector3(T x = 0) : x(x), y(x), z(x) {}
        vector3(const vector3& cpy) : x(cpy.x), y(cpy.y), z(cpy.z) {}
        vector3(const vector2<T>& vec2) : x(vec2.x), y(vec2.y), z(0) {}
        vector3(const vector4<T>& vec4) : x(vec4.x), y(vec4.y), z(vec4.z) {}

        template <typename U>
          vector3(const vector3<U>& vu) :
            x(static_cast<T>(vu.x)), y(static_cast<T>(vu.y)), z(static_cast<T>(vu.z)) {}

        template <typename U>
          vector3(const vector2<U>& v2, T z) :
            x(static_cast<T>(v2.x)),
            y(static_cast<T>(v2.y)),
            z(static_cast<T>(z)) {}

        vector3& operator=(const vector3& cpy) {
          x = cpy.x; y = cpy.y; z = cpy.z;
          return *this;
        }

        vector3 operator+(const vector3& rhs) const {
          return { x + rhs.x, y + rhs.y, z + rhs.z };
        }

        vector3 operator-(const vector3& rhs) const {
          return { x - rhs.x, y - rhs.y, z - rhs.z };
        }

        vector3 operator*(const vector3& rhs) const {
          return { x * rhs.x, y * rhs.y, z * rhs.z };
        }

        vector3 operator/(T s) const {
          ASSERT(!COMPARE_EQ(s, 0.0f));
          Float inv = 1.0f / s;
          return vector3f(*this) * inv;
        }

        vector3 operator/(const vector3& rhs) const {
          ASSERT(!rhs.is_zero());
          return { x / rhs.x, y / rhs.y, z / rhs.z };
        }

        vector3 operator-() const {
          return -1 * (*this);
        }

        vector3& operator+=(const vector3& vec) {
          x += vec.x;
          y += vec.y;
          z += vec.z;
          return *this;
        }

        vector3& operator-=(const vector3& vec) {
          x -= vec.x;
          y -= vec.y;
          z -= vec.z;
          return *this;
        }

        vector3& operator*=(T s) {
          x *= s;
          y *= s;
          z *= s;
          return *this;
        }

        vector3& operator/=(T s) {
          ASSERT(!COMPARE_EQ(s, 0.0f));
          Float inv = 1.0f / s;
          x *= inv;
          y *= inv;
          z *= inv;
          return *this;
        }

        T& operator[](int i) {
          ASSERT(i >= 0 && i < 3);
          switch (i) {
            case 0:
              return x;
            case 1:
              return y;
            default:
              return z;
          }
        }

        T operator[](int i) const {
          ASSERT(i >= 0 && i < 3);
          switch (i) {
            case 0:
              return x;
            case 1:
              return y;
            default:
              return z;
          }
        }

        T dot(const vector3& rhs) const {
          return x * rhs.x + y * rhs.y + z * rhs.z;
        }

        vector3 cross(const vector3& rhs) const {
          return vector3(
              y * rhs.z - z * rhs.y,
              rhs.x * z - x * rhs.z,
              x * rhs.y - y * rhs.x
              );
        }

        Float size() const {
          return std::sqrt(x * x + y * y + z * z);
        }

        Float size_sq() const {
          return x * x + y * y + z * z;
        }

        bool is_zero() const {
          return COMPARE_EQ(x, 0.0f) && COMPARE_EQ(y, 0.0f) && COMPARE_EQ(z, 0.0f);
        }

        vector3 clamped(T min_val, T max_val) const {
          return vector3(
              math::clamp(x, min_val, max_val),
              math::clamp(y, min_val, max_val),
              math::clamp(z, min_val, max_val)
              );
        }

        vector3 normalized() const {
          Float sz = this->size();
          ASSERT(!COMPARE_EQ(sz, 0.0f));
          return *this / sz;
        }

        std::string to_string() const {
          return "[\t"
            + std::to_string(x)
            + "\t"
            + std::to_string(y)
            + "\t"
            + std::to_string(z)
            + "\t]";
        }

        std::wstring to_wstring() const {
          return L"[\t"
            + std::to_wstring(x)
            + L"\t"
            + std::to_wstring(y)
            + L"\t"
            + std::to_wstring(z)
            + L"\t]";
        }

        friend std::ostream& operator<<(std::ostream& os, const vector3& vec) {
          return os << vec.to_string();
        }

        friend std::wostream& operator<<(std::wostream& os, const vector3& vec) {
          return os << vec.to_wstring();
        }
    }; /* class vector3 */

  template <typename T>
    class vector4 {
      public:
        T x, y, z, w;
        vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
        vector4(T x = 0) : x(x), y(x), z(x), w(x) {}
        vector4(const vector4& cpy) : x(cpy.x), y(cpy.y), z(cpy.z), w(cpy.w) {}
        vector4(const vector2<T>& vec2) : x(vec2.x), y(vec2.y), z(0), w(0) {}
        vector4(const vector3<T>& vec3) : x(vec3.x), y(vec3.y), z(vec3.z), w(0) {}

        template <typename U>
          vector4(const vector4<U>& vu) :
            x(static_cast<T>(vu.x)),
            y(static_cast<T>(vu.y)),
            z(static_cast<T>(vu.z)),
            w(static_cast<T>(vu.w)) {}

        template <typename U>
          vector4(const vector2<U>& v2, T z, T w) :
            x(static_cast<T>(v2.x)),
            y(static_cast<T>(v2.y)),
            z(static_cast<T>(z)),
            w(static_cast<T>(w)) {}

        template <typename U>
          vector4(const vector3<U>& v3, T w) :
            x(static_cast<T>(v3.x)),
            y(static_cast<T>(v3.y)),
            z(static_cast<T>(v3.z)),
            w(static_cast<T>(w)) {}

        vector4& operator=(const vector4& cpy) {
          x = cpy.x; y = cpy.y; z = cpy.z; w = cpy.w;
          return *this;
        }

        vector4 operator+(const vector4& rhs) const {
          return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
        }

        vector4 operator-(const vector4& rhs) const {
          return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
        }

        vector4 operator*(const vector4& rhs) const {
          return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w };
        }

        vector4 operator/(T s) const {
          ASSERT(!COMPARE_EQ(s, 0.0f));
          Float inv = 1.0f / s;
          return vector4f(*this) * inv;
        }

        vector4 operator/(const vector4& rhs) const {
          ASSERT(!rhs.is_zero());
          return { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w };
        }

        vector4 operator-() const {
          return -1 * (*this);
        }

        vector4& operator+=(const vector4& vec) {
          x += vec.x;
          y += vec.y;
          z += vec.z;
          w += vec.w;
          return *this;
        }

        vector4& operator-=(const vector4& vec) {
          x -= vec.x;
          y -= vec.y;
          z -= vec.z;
          w -= vec.w;
          return *this;
        }

        vector4& operator*=(T s) {
          x *= s;
          y *= s;
          z *= s;
          w *= s;
          return *this;
        }

        vector4& operator/=(T s) {
          ASSERT(!COMPARE_EQ(s, 0.0f));
          Float inv = 1.0f / s;
          x *= inv;
          y *= inv;
          z *= inv;
          w *= inv;
          return *this;
        }

        T& operator[](int i) {
          ASSERT(i >= 0 && i < 4);
          switch (i) {
            case 0:
              return x;
            case 1:
              return y;
            case 2:
              return z;
            default:
              return w;
          }
        }

        T operator[](int i) const {
          ASSERT(i >= 0 && i < 3);
          switch (i) {
            case 0:
              return x;
            case 1:
              return y;
            case 2:
              return z;
            default:
              return w;
          }
        }

        T dot(const vector4& rhs) const {
          return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
        }

        Float size() const {
          return std::sqrt(x * x + y * y + z * z + w * w);
        }

        Float size_sq() const {
          return x * x + y * y + z * z + w * w;
        }

        bool is_zero() const {
          return COMPARE_EQ(x, 0.0f)
            && COMPARE_EQ(y, 0.0f)
            && COMPARE_EQ(z, 0.0f)
            && COMPARE_EQ(w, 0.0f);
        }

        vector4 clamped(T min_val, T max_val) const {
          return vector4(
              math::clamp(x, min_val, max_val),
              math::clamp(y, min_val, max_val),
              math::clamp(z, min_val, max_val),
              math::clamp(w, min_val, max_val)
              );
        }

        vector4 normalized() const {
          Float sz = this->size();
          ASSERT(!COMPARE_EQ(sz, 0.0f));
          return *this / sz;
        }

        std::string to_string() const {
          return "[\t"
            + std::to_string(x)
            + "\t"
            + std::to_string(y)
            + "\t"
            + std::to_string(z)
            + "\t"
            + std::to_string(w)
            + "\t]";
        }

        std::wstring to_wstring() const {
          return L"[\t"
            + std::to_wstring(x)
            + L"\t"
            + std::to_wstring(y)
            + L"\t"
            + std::to_wstring(z)
            + L"\t"
            + std::to_wstring(w)
            + L"\t]";
        }

        friend std::ostream& operator<<(std::ostream& os, const vector4& vec) {
          return os << vec.to_string();
        }

        friend std::wostream& operator<<(std::wostream& os, const vector4& vec) {
          return os << vec.to_wstring();
        }
    };
} /* namespace math */

#endif /* MATH_VECTOR_HPP */
