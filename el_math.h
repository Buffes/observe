#ifndef EL_MATH_H
#define EL_MATH_H

#include "types.h"
#include <math.h> // for sqrt and trig functions

// Constants in 32 bit
#define SQRT_TWO 1.414121356F
#define ONE_OVER_SQRT_TWO 0.70710678F
#define PI 3.1415926F
#define PI_HALF 1.57079632F
#define IDENTITY4 {1.0f, 0.0f, 0.0f, 0.0f,\
0.0f, 1.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 1.0f, 0.0f,\
    0.0f, 0.0f, 0.0f, 1.0f}

// math.h sqrt will usually compile to the sqrtss instruction, but using the
// reciprocal (inverse) rsqrtss is probably faster. We could switch to that
// if we find sqrt to be a significant bottleneck.
inline f32 el_sqrt(f32 a) {
    return sqrt(a);
}

// returns 0 for 0.0f
inline s32 sign(f32 a) {
    return (0.0f < a) - (a < 0.0f);
}

inline f32 min(f32 a, f32 b) {
    return a < b ? a : b;
}

inline f32 max(f32 a, f32 b) {
    return a > b ? a : b;
}

inline f32 clamp(f32 num, f32 low, f32 high) {
    return max(low, min(num, high));
}

inline f32 sq(f32 a) {
    return a * a;
}

inline u32 round_f32_to_u32(f32 f) {
    u32 result = (u32)(f + 0.5f);
    return result;
}

inline s32 round_f32_to_s32(f32 f) {
    s32 result = (s32)(f + 0.5f * sign(f));
    return result;
}

inline s32 el_ceil(f32 x) {
    return ceil(x);
}

inline s32 el_floor(f32 x) {
    return floor(x);
}

// Vector types. Vec2, Vec3 are 32-bit fp, Ivec2 is 32-bit signed integer

union Ivec2 {
    struct {
        s32 x, y;
    };
    s32 el[2];
};

union Ivec3 {
    struct {
        s32 x, y, z;
    };
    s32 el[3];
};

union Vec2 {
    struct {
        f32 x, y;
    };
    f32 el[2];
};

//TODO: Use SIMD for these operations?
inline Vec2 operator*(Vec2 vec, float scalar) {
    Vec2 result;
    result.x = vec.x * scalar;
    result.y = vec.y * scalar;

    return result;
}

inline Vec2 operator*(float scalar, Vec2 vec) {
    Vec2 result;
    result = vec * scalar;

    return result;
}

inline Vec2 operator/(float scalar, Vec2 vec) {
    Vec2 result;
    result.x = vec.x / scalar;
    result.y = vec.y / scalar;

    return result;
}

inline Vec2 operator/(Vec2 vec, float scalar) {
    Vec2 result;
    result.x = vec.x / scalar;
    result.y = vec.y / scalar;

    return result;
}

inline Vec2 operator+(Vec2 a, Vec2 b) {
    Vec2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

inline Vec2 operator-(Vec2 a, Vec2 b) {
    Vec2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}

inline Vec2 operator-(Vec2 a) {
    Vec2 result;
    result.x = -a.x;
    result.y = -a.y;

    return result;
}

inline bool operator==(Vec2 left, Vec2 right) {
    return left.x == right.x &&
           left.y == right.y;
}

inline bool operator !=(Vec2 left, Vec2 right) {
    return !(left == right);
}

inline Vec2& operator*=(Vec2 &a, f32 scalar) {
    a = a * scalar;

    return a;
}

inline Vec2& operator/=(Vec2 &a, f32 scalar) {
    a = a / scalar;

    return a;
}

inline Vec2& operator+=(Vec2 &a, Vec2 b) {
    a = a + b;

    return a;
}

inline Vec2& operator-=(Vec2 &a, Vec2 b) {
    a = a - b;

    return a;
}

inline f32 length(Vec2 in) {
    return el_sqrt((in.x * in.x + in.y * in.y));
}

inline Vec2 normalize(Vec2 in) {
    f32 len = length(in);
    return len == 0.0f ? in : in / len;
}

union Vec3 {
    struct {
        f32 x, y, z;
    };
    struct {
        f32 r, g, b;
    };
    f32 el[3];
};

inline Vec3 operator*(Vec3 vec, float scalar) {
    Vec3 result;
    result.x = vec.x * scalar;
    result.y = vec.y * scalar;
    result.z = vec.z * scalar;

    return result;
}

inline Vec3 operator*(float scalar, Vec3 vec) {
    Vec3 result;
    result = vec * scalar;

    return result;
}

inline Vec3 operator/(Vec3 vec, float scalar) {
    Vec3 result;
    result.x = vec.x / scalar;
    result.y = vec.y / scalar;
    result.z = vec.z / scalar;

    return result;
}

inline Vec3 operator+(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

inline Vec3 operator-(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}

inline Vec3 operator-(Vec3 a) {
    Vec3 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;

    return result;
}

inline bool operator==(Vec3 left, Vec3 right) {
    return left.x == right.x &&
           left.y == right.y &&
           left.z == right.z;
}

inline bool operator !=(Vec3 left, Vec3 right) {
    return !(left == right);
}

inline Vec3& operator*=(Vec3 &a, f32 scalar) {
    a = a * scalar;

    return a;
}

inline Vec3& operator/=(Vec3 &a, f32 scalar) {
    a = a / scalar;

    return a;
}

inline Vec3& operator+=(Vec3 &a, Vec3 b) {
    a = a + b;

    return a;
}

inline Vec3& operator-=(Vec3 &a, Vec3 b) {
    a = a - b;

    return a;
}

inline Vec3 vec3(Vec2 xy, f32 z = 0.0f) {
    return Vec3{xy.x, xy.y, z};
}

inline Vec3 vec3(f32 x, f32 y, f32 z) {
    return Vec3{x, y, z};
}

inline f32 length_sq(Vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline f32 length(Vec3 v) {
    return el_sqrt(length_sq(v));
}

inline Vec3 normalize(Vec3 v) {
    f32 len = length(v);
    return len == 0.0f ? v : v / len;
}

inline f32 distance_sq(Vec3 p1, Vec3 p2) {
    return length_sq(p1 - p2);
}

inline f32 distance(Vec3 p1, Vec3 p2) {
    return el_sqrt(distance_sq(p1, p2));
}

inline Vec3 max(Vec3 a, Vec3 b) {
    return length_sq(a) > length_sq(b) ? a : b;
}

inline Vec3 min(Vec3 a, Vec3 b) {
    return length_sq(a) < length_sq(b) ? a : b;
}

inline f32 dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(Vec3 a, Vec3 b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

union Vec4 {
    struct {
        f32 x, y, z, w;
    };
    struct {
        f32 r, g, b, a;
    };
    f32 el[4];
};

inline Vec4 operator*(Vec4 vec, float scalar) {
    Vec4 result;
    result.x = vec.x * scalar;
    result.y = vec.y * scalar;
    result.z = vec.z * scalar;
    result.w = vec.w * scalar;

    return result;
}

inline Vec4 operator*(float scalar, Vec4 vec) {
    Vec4 result;
    result = vec * scalar;

    return result;
}

inline Vec4 operator/(Vec4 vec, float scalar) {
    Vec4 result;
    result.x = vec.x / scalar;
    result.y = vec.y / scalar;
    result.z = vec.z / scalar;
    result.w = vec.w / scalar;

    return result;
}

inline Vec4 operator+(Vec4 a, Vec4 b) {
    Vec4 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;

    return result;
}

inline Vec4 operator-(Vec4 a, Vec4 b) {
    Vec4 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;

    return result;
}

inline Vec4 operator-(Vec4 a) {
    Vec4 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;

    return result;
}

inline bool operator==(Vec4 left, Vec4 right) {
    return left.x == right.x &&
           left.y == right.y &&
           left.z == right.z &&
           left.w == right.w;
}

inline bool operator !=(Vec4 left, Vec4 right) {
    return !(left == right);
}

inline Vec4& operator*=(Vec4 &a, f32 scalar) {
    a = a * scalar;

    return a;
}

inline Vec4& operator/=(Vec4 &a, f32 scalar) {
    a = a / scalar;

    return a;
}

inline Vec4& operator+=(Vec4 &a, Vec4 b) {
    a = a + b;

    return a;
}

inline Vec4& operator-=(Vec4 &a, Vec4 b) {
    a = a - b;

    return a;
}

/*
   Matrix types.
   We use column major storage, i.e.:
   | 0  3  6 |
   | 1  4  7 |
   | 2  5  8 |

   TODO: Optimize with SIMD?
*/

struct Mat3 {
    f32 el[9];
};

inline Mat3 operator*(float scalar, Mat3 mat) {
    for (int i = 0; i < 9; i++) {
        mat.el[i] *= scalar;
    }

    return mat;
}

inline Mat3 operator+(Mat3 left, Mat3 right) {
    for (int i = 0; i < 9; i++) {
        left.el[i] += right.el[i];
    }

    return left;
}

inline Mat3 operator-(Mat3 left, Mat3 right) {
    for (int i = 0; i < 9; i++) {
        left.el[i] -= right.el[i];
    }

    return left;
}

// Matrix multiplication
inline Mat3 operator*(Mat3 left, Mat3 right) {
    Mat3 result = {0};

    for (int col = 0; col < 3; col++) {
        for (int row = 0; row < 3; row++) {
            for (int el = 0; el < 3; el++) {
                result.el[col*3 + row] +=
                    left.el[row + el*3] * right.el[col*3 + el];
            }
        }
    }

    return result;
}

struct Mat4 {
    f32 el[16];
};

inline Mat4 operator*(float scalar, Mat4 mat) {
    for (int i = 0; i < 16; i++) {
        mat.el[i] *= scalar;
    }

    return mat;
}

inline Mat4 operator+(Mat4 left, Mat4 right) {
    for (int i = 0; i < 16; i++) {
        left.el[i] += right.el[i];
    }

    return left;
}

inline Mat4 operator-(Mat4 left, Mat4 right) {
    for (int i = 0; i < 16; i++) {
        left.el[i] -= right.el[i];
    }

    return left;
}

// Matrix vector multiplication
inline Vec4 operator*(Mat4 mat, Vec4 vec) {
    Vec4 result = {};

    result.x = mat.el[0] * vec.x + mat.el[4] * vec.y + mat.el[8]  * vec.z + mat.el[12] * vec.w;
    result.y = mat.el[1] * vec.x + mat.el[5] * vec.y + mat.el[9]  * vec.z + mat.el[13] * vec.w;
    result.z = mat.el[2] * vec.x + mat.el[6] * vec.y + mat.el[10] * vec.z + mat.el[14] * vec.w;
    result.w = mat.el[3] * vec.x + mat.el[7] * vec.y + mat.el[11] * vec.z + mat.el[15] * vec.w;

    return result;
}

// Matrix matrix multiplication
inline Mat4 operator*(Mat4 left, Mat4 right) {
    Mat4 result = {};

    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            for (int el = 0; el < 4; el++) {
                result.el[col*4 + row] +=
                    left.el[row + el*4] * right.el[col*4 + el];
            }
        }
    }

    return result;
}

#endif // EL_MATH_H

