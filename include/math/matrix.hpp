#ifndef MATH_MATRIX_HPP
#define MATH_MATRIX_HPP

#include "vector.hpp"
#include <iostream>

#ifdef __SSE__
#include <immintrin.h>
#endif

namespace math {
  template <typename T> class matrix2;
  template <typename T> class matrix3;
  template <typename T> class matrix4;

  typedef matrix2<Float>  matrix2f;
  typedef matrix2<int>    matrix2i;

  typedef matrix3<Float>  matrix3f;
  typedef matrix3<int>    matrix3i;

  typedef matrix4<Float>  matrix4f;
  typedef matrix4<int>    matrix4i;

  template <typename T>
    class matrix2 {
      public:
        T value[2][2];

        matrix2(const T** values) {
          for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
              value[i][j] = values[i][j];
            }
          }
        }

        explicit matrix2(T x = 1) {
          for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
              if (i == j) value[i][j] = x;
              else value[i][j] = 0;
            }
          }
        }

        matrix2(const matrix2& cpy) {
          for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
              value[i][j] = cpy.value[i][j];
            }
          }
        }

        matrix2(const vector2<T>& v1, const vector2<T>& v2) {
          value[0][0] = v1.x;
          value[1][0] = v1.y;
          value[0][1] = v2.x;
          value[1][1] = v2.y;
        }

        template <typename U>
          matrix2(const matrix2<U>& mat) {
            for (int i = 0; i < 2; ++i) {
              for (int j = 0; j < 2; ++j) {
                value[i][j] = static_cast<T>(mat.value[i][j]);
              }
            }
          }

        matrix2& operator=(const matrix2& cpy) {
          for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
              value[i][j] = cpy.value[i][j];
            }
          }
          return *this;
        }

        T det() const {
          return value[0][0] * value[1][1] - value[1][0] * value[0][1];
        }

        vector2<T> row(int i) const {
          ASSERT(i < 2);
          return { value[i][0], value[i][1] };
        }

        vector2<T> col(int j) const {
          ASSERT(j < 2);
          return { value[0][j], value[1][j] };
        }

        matrix2 t() const {
          return matrix2({ value[0][0], value[0][1] }, { value[1][0], value[1][1] });
        }

        template <typename U>
          vector2<U> dot(const vector2<U>& v) const {
            vector2<U> c1(value[0][0], value[1][0]);
            vector2<U> c2(value[0][1], value[1][1]);
            return c1 * v.x + c2 * v.y;
          }

        matrix2 operator*(const matrix2& m) const {
          const vector2<T> c0(col(0)), c1(col(1));
          return matrix2(
              c0 * m[0][0] + c1 * m[1][0],
              c0 * m[0][1] + c1 * m[1][1]
              );
        }

        T* operator[](int i) {
          return value[i];
        }

        const T* operator[](int i) const {
          return value[i];
        }

        std::string to_string() const {
          return "[" + row(0).to_string() + "\n " + row(1).to_string() + "]";
        }

        std::wstring to_wstring() const {
          return L"[" + row(0).to_wstring() + "\n " + row(1).to_wstring() + L"]";
        }

        friend std::ostream& operator<<(std::ostream& os, matrix2<T> m) {
          return os << m.to_string();
        }

        friend std::wostream& operator<<(std::wostream& os, matrix2<T> m) {
          return os << m.to_wstring();
        }
    }; /* class matrix2 */

  template <typename T>
    class matrix3 {
      public:
        T value[3][3];

        matrix3(const T** values) {
          for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
              value[i][j] = values[i][j];
            }
          }
        }

        explicit matrix3(T x = 1) {
          for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
              if (i == j) value[i][j] = x;
              else value[i][j] = 0;
            }
          }
        }

        matrix3(const matrix3& cpy) {
          for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
              value[i][j] = cpy.value[i][j];
            }
          }
        }

        matrix3(const vector3<T>& v1, const vector3<T>& v2, const vector3<T>& v3) {
          value[0][0] = v1.x; value[1][0] = v1.y; value[2][0] = v1.z;
          value[0][1] = v2.x; value[1][1] = v2.y; value[2][1] = v2.z;
          value[0][2] = v3.x; value[1][2] = v3.y; value[2][2] = v3.z;
        }

        template <typename U>
          matrix3(const matrix3<U>& mat) {
            for (int i = 0; i < 3; ++i) {
              for (int j = 0; j < 3; ++j) {
                value[i][j] = static_cast<T>(mat.value[i][j]);
              }
            }
          }

        matrix3& operator=(const matrix3& cpy) {
          for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
              value[i][j] = cpy.value[i][j];
            }
          }
          return *this;
        }

        T det() const {
          return value[0][0] * value[1][1] * value[2][2]
            + value[0][1] * value[1][2] * value[2][0]
            + value[0][2] * value[1][0] * value[2][1]
            - (
                value[2][0] * value[1][1] * value[0][2]
                + value[2][1] * value[1][2] * value[0][0]
                + value[2][2] * value[1][0] * value[0][1]
              );
        }

        matrix3 t() const {
          return matrix3(
              { value[0][0], value[0][1], value[0][2] },
              { value[1][0], value[1][1], value[1][2] },
              { value[2][0], value[2][1], value[2][2] }
              );
        }

        vector3<T> row(int i) const {
          ASSERT(i < 3);
          return { value[i][0], value[i][1], value[i][2] };
        }

        vector3<T> col(int j) const {
          ASSERT(j < 3);
          return { value[0][j], value[1][j], value[2][j] };
        }

        template <typename U>
          vector3<U> dot(const vector3<U>& v) const {
            return vector3<U>(
                value[0][0] * v.x + value[0][1] * v.y + value[0][2] * v.z,
                value[1][0] * v.x + value[1][1] * v.y + value[1][2] * v.z,
                value[2][0] * v.x + value[2][1] * v.y + value[2][2] * v.z
                );
          }

        matrix3 operator*(const matrix3& m) const {
          const vector3<T> c0(col(0)), c1(col(1)), c2(col(2));
          return matrix3(
              c0 * m[0][0] + c1 * m[1][0] + c2 * m[2][0],
              c0 * m[0][1] + c1 * m[1][1] + c2 * m[2][1],
              c0 * m[0][2] + c1 * m[1][2] + c2 * m[2][2]
              );
        }

        T* operator[](int i) {
          return value[i];
        }

        const T* operator[](int i) const {
          return value[i];
        }

        std::string to_string() const {
          return "[" + row(0).to_string() + "\n "
            + row(1).to_string() + "\n "
            + row(2).to_string() + "]";
        }

        std::wstring to_wstring() const {
          return L"[" + row(0).to_wstring() + "\n "
            + row(1).to_wstring() + "\n "
            + row(2).to_wstring() + L"]";
        }

        friend std::ostream& operator<<(std::ostream& os, matrix3<T> m) {
          return os << m.to_string();
        }

        friend std::wostream& operator<<(std::wostream& os, matrix3<T> m) {
          return os << m.to_wstring();
        }
    }; /* class matrix3 */

  template <typename T>
    class matrix4 {
      public:
        // align on a 16-byte boundary for SSE compatibility
        union alignas(16) {
          T value[4][4];
          T value_flat[16];
        };

        matrix4(const T** values) {
          for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
              value[i][j] = values[i][j];
            }
          }
        }

        explicit matrix4(T x = 1) {
          for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
              if (i == j) value[i][j] = x;
              else value[i][j] = 0;
            }
          }
        }

        matrix4(const matrix4& cpy) {
          for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
              value[i][j] = cpy.value[i][j];
            }
          }
        }

        matrix4(
            const vector4<T>& v1,
            const vector4<T>& v2,
            const vector4<T>& v3,
            const vector4<T>& v4)
        {
          value[0][0] = v1.x; value[1][0] = v1.y; value[2][0] = v1.z; value[3][0] = v1.w;
          value[0][1] = v2.x; value[1][1] = v2.y; value[2][1] = v2.z; value[3][1] = v2.w;
          value[0][2] = v3.x; value[1][2] = v3.y; value[2][2] = v3.z; value[3][2] = v3.w;
          value[0][3] = v4.x; value[1][3] = v4.y; value[2][3] = v4.z; value[3][3] = v4.w;
        }

        template <typename U>
          matrix4(const matrix4<U>& mat) {
            for (int i = 0; i < 4; ++i) {
              for (int j = 0; j < 4; ++j) {
                value[i][j] = static_cast<T>(mat.value[i][j]);
              }
            }
          }

        matrix4& operator=(const matrix4& cpy) {
          for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
              value[i][j] = cpy.value[i][j];
            }
          }
          return *this;
        }

        T det() const {
          return
            value[0][3] * value[1][2] * value[2][1] * value[3][0]
            - value[0][2] * value[1][3] * value[2][1] * value[3][0]
            - value[0][3] * value[1][1] * value[2][2] * value[3][0]
            + value[0][1] * value[1][3] * value[2][2] * value[3][0]
            + value[0][2] * value[1][1] * value[2][3] * value[3][0]
            - value[0][1] * value[1][2] * value[2][3] * value[3][0]
            - value[0][3] * value[1][2] * value[2][0] * value[3][1]
            + value[0][2] * value[1][3] * value[2][0] * value[3][1]
            + value[0][3] * value[1][0] * value[2][2] * value[3][1]
            - value[0][0] * value[1][3] * value[2][2] * value[3][1]
            - value[0][2] * value[1][0] * value[2][3] * value[3][1]
            + value[0][0] * value[1][2] * value[2][3] * value[3][1]
            + value[0][3] * value[1][1] * value[2][0] * value[3][2]
            - value[0][1] * value[1][3] * value[2][0] * value[3][2]
            - value[0][3] * value[1][0] * value[2][1] * value[3][2]
            + value[0][0] * value[1][3] * value[2][1] * value[3][2]
            + value[0][1] * value[1][0] * value[2][3] * value[3][2]
            - value[0][0] * value[1][1] * value[2][3] * value[3][2]
            - value[0][2] * value[1][1] * value[2][0] * value[3][3]
            + value[0][1] * value[1][2] * value[2][0] * value[3][3]
            + value[0][2] * value[1][0] * value[2][1] * value[3][3]
            - value[0][0] * value[1][2] * value[2][1] * value[3][3]
            - value[0][1] * value[1][0] * value[2][2] * value[3][3]
            + value[0][0] * value[1][1] * value[2][2] * value[3][3];
        }

        matrix4 t() const {
          return matrix4(
              { value[0][0], value[0][1], value[0][2], value[0][3] },
              { value[1][0], value[1][1], value[1][2], value[1][3] },
              { value[2][0], value[2][1], value[2][2], value[2][3] },
              { value[3][0], value[3][1], value[3][2], value[3][3] }
              );
        }

        vector4<T> row(int i) const {
          ASSERT(i < 4);
          return { value[i][0], value[i][1], value[i][2], value[i][3] };
        }

        vector4<T> col(int j) const {
          ASSERT(j < 4);
          return { value[0][j], value[1][j], value[2][j], value[3][j] };
        }

        template <typename U>
          vector4<U> dot(const vector4<U>& v) const {
            return vector4<U>(
                value[0][0] * v.x + value[0][1] * v.y + value[0][2] * v.z + value[0][3] * v.w,
                value[1][0] * v.x + value[1][1] * v.y + value[1][2] * v.z + value[1][3] * v.w,
                value[2][0] * v.x + value[2][1] * v.y + value[2][2] * v.z + value[2][3] * v.w,
                value[3][0] * v.x + value[3][1] * v.y + value[3][2] * v.z + value[3][3] * v.w
                );
          }

        matrix4f inverse() const {
          ASSERT(!COMPARE_EQ(det(), 0), "inverse singular matrix");

          // Reduce the matrix to RREF using Gauss-Jordan algorithm
          vector4f rows[4] = { row(0), row(1), row(2), row(3) };
          vector4f irows[4] = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };
          for (int i = 0, j = 0; i < 4 && j < 4;) {
            if (COMPARE_EQ(rows[i][j], 0)) {
              for (int si = i + 1; si < 4; ++si) {
                if (!COMPARE_EQ(rows[si][j], 0)) {
                  // make pivot non-zero
                  std::swap(rows[i], rows[si]);
                  std::swap(irows[i], irows[si]);
                  break;
                }
              }
            }
            if (!COMPARE_EQ(rows[i][j], 0)) {
              // make pivot 1
              const Float d = rows[i][j];
              rows[i] /= d;
              irows[i] /= d;
              // make column entries that is not the pivot zeros
              for (int si = 0; si < 4; ++si) {
                if (si != i) {
                  const Float mult = rows[si][j];
                  rows[si] -= mult * rows[i];
                  irows[si] -= mult * irows[i];
                }
              }
              ++i; ++j;
            } else {
              ++j;
            }
          }

          return matrix4f(irows[0], irows[1], irows[2], irows[3]).t();
        }

        matrix4 operator*(const matrix4& m) const {
#ifdef __SSE__
          matrix4 ret;
          __m128 m128[14];
          for (int row = 0; row < 4; ++row) {
            m128[0] = _mm_broadcast_ss(&value[row][0]);
            m128[1] = _mm_broadcast_ss(&value[row][1]);
            m128[2] = _mm_broadcast_ss(&value[row][2]);
            m128[3] = _mm_broadcast_ss(&value[row][3]);

            m128[4] = _mm_load_ps(m.value[0]);
            m128[5] = _mm_load_ps(m.value[1]);
            m128[6] = _mm_load_ps(m.value[2]);
            m128[7] = _mm_load_ps(m.value[3]);

            // multiply
            m128[8]   = _mm_mul_ps(m128[0], m128[4]);
            m128[9]   = _mm_mul_ps(m128[1], m128[5]);
            m128[10]  = _mm_mul_ps(m128[2], m128[6]);
            m128[11]  = _mm_mul_ps(m128[3], m128[7]);

            // partial sums
            m128[12] = _mm_add_ps(m128[8], m128[9]);
            m128[13] = _mm_add_ps(m128[10], m128[11]);

            // sum up partial sums
            m128[12] = _mm_add_ps(m128[12], m128[13]);

            _mm_store_ps(ret.value_flat + (row << 2), m128[12]);
          }
          return ret;
#else
#warning Not using SSE to multiply matrix4
          const vector4<T> c0(col(0)), c1(col(1)), c2(col(2)), c3(col(3));
          return matrix4(
              c0 * m[0][0] + c1 * m[1][0] + c2 * m[2][0] + c3 * m[3][0],
              c0 * m[0][1] + c1 * m[1][1] + c2 * m[2][1] + c3 * m[3][1],
              c0 * m[0][2] + c1 * m[1][2] + c2 * m[2][2] + c3 * m[3][2],
              c0 * m[0][3] + c1 * m[1][3] + c2 * m[2][3] + c3 * m[3][3]
              );
#endif
        }

        T* operator[](int i) {
          return value[i];
        }

        const T* operator[](int i) const {
          return value[i];
        }

        std::string to_string() const {
          return "[" + row(0).to_string() + "\n "
            + row(1).to_string() + "\n "
            + row(2).to_string() + "\n "
            + row(3).to_string() + "]";
        }

        std::wstring to_wstring() const {
          return L"[" + row(0).to_wstring() + "\n "
            + row(1).to_wstring() + "\n "
            + row(2).to_wstring() + "\n "
            + row(3).to_wstring() + L"]";
        }

        friend std::ostream& operator<<(std::ostream& os, matrix4<T> m) {
          return os << m.to_string();
        }

        friend std::wostream& operator<<(std::wostream& os, matrix4<T> m) {
          return os << m.to_wstring();
        }
    }; /* class matrix3 */

  template <typename T>
    matrix2<T> operator*(const matrix2<T>& m, T s) {
      matrix2<T> r;
      for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
          r[i][j] = m[i][j] * s;
        }
      }
      return r;
    }

  template <typename T>
    inline matrix2<T> operator*(T s, const matrix2<T>& m) {
      return m * s;
    }

  template <typename T>
    inline matrix2<T> operator*(Float s, const matrix2<T>& m) {
      return m * s;
    }

  template <typename T>
    matrix2<T> operator/(const matrix2<T>& m, Float s) {
      matrix2<Float> r;
      ASSERT(!COMPARE_EQ(s, 0));
      s = 1 / s;
      for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
          r[i][j] = m[i][j] * s;
        }
      }
      return r;
    }

  template <typename T>
    matrix3<T> operator*(const matrix3<T>& m, T s) {
      matrix3<T> r;
      for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
          r[i][j] = m[i][j] * s;
        }
      }
      return r;
    }

  template <typename T>
    inline matrix3<T> operator*(T s, const matrix3<T>& m) {
      return m * s;
    }

  template <typename T>
    inline matrix3<T> operator*(Float s, const matrix3<T>& m) {
      return m * s;
    }

  template <typename T>
    matrix3<T> operator/(const matrix3<T>& m, Float s) {
      matrix3<Float> r;
      ASSERT(!COMPARE_EQ(s, 0));
      s = 1 / s;
      for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
          r[i][j] = m[i][j] * s;
        }
      }
      return r;
    }

  template <typename T>
    matrix4<T> operator*(const matrix4<T>& m, T s) {
      matrix4<T> r;
      for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
          r[i][j] = m[i][j] * s;
        }
      }
      return r;
    }

  template <typename T>
    inline matrix4<T> operator*(T s, const matrix4<T>& m) {
      return m * s;
    }

  template <typename T>
    inline matrix4<T> operator*(Float s, const matrix4<T>& m) {
      return m * s;
    }

  template <typename T>
    matrix4<T> operator/(const matrix4<T>& m, Float s) {
      matrix4<Float> r;
      ASSERT(!COMPARE_EQ(s, 0));
      s = 1 / s;
      for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
          r[i][j] = m[i][j] * s;
        }
      }
      return r;
    }
} /* namespace math */

#endif /* MATH_MATRIX_HPP */
