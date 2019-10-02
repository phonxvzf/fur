#ifndef MATH_MATRIX_HPP
#define MATH_MATRIX_HPP

#include "vector.hpp"

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

        matrix2(T x) {
          value[0][0] = x;
          value[1][1] = x;
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

        matrix2 t() const {
          return matrix2({ value[0][0], value[0][1] }, { value[1][0], value[1][1] });
        }

        template <typename U>
          vector2<U> operator*(const vector2<U>& v) {
            vector2<U> c1(value[0][0], value[1][0]);
            vector2<U> c2(value[0][1], value[1][1]);
            return c1 * v.x + c2 * v.y;
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

        matrix3(T x) {
          value[0][0] = x;
          value[1][1] = x;
          value[2][2] = x;
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

        template <typename U>
          vector3<U> operator*(const vector3<U>& v) {
            vector3<U> c1(value[0][0], value[1][0], value[2][0]);
            vector3<U> c2(value[0][1], value[1][1], value[2][1]);
            vector3<U> c3(value[0][2], value[1][2], value[2][2]);
            return c1 * v.x + c2 * v.y + c3 * v.z;
          }
    }; /* class matrix3 */

  template <typename T>
    class matrix4 {
      public:
        T value[4][4];

        matrix4(const T** values) {
          for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
              value[i][j] = values[i][j];
            }
          }
        }

        matrix4(T x) {
          value[0][0] = x;
          value[1][1] = x;
          value[2][2] = x;
          value[3][3] = x;
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

        template <typename U>
          vector4<U> operator*(const vector4<U>& v) {
            vector4<U> c1(value[0][0], value[1][0], value[2][0], value[3][0]);
            vector4<U> c2(value[0][1], value[1][1], value[2][1], value[3][1]);
            vector4<U> c3(value[0][2], value[1][2], value[2][2], value[3][2]);
            vector4<U> c4(value[0][2], value[1][2], value[2][2], value[3][3]);
            return c1 * v.x + c2 * v.y + c3 * v.z + c4 * v.w;
          }
    }; /* class matrix3 */
} /* namespace math */

#endif /* MATH_MATRIX_HPP */
