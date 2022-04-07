/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/Vec4.h>
#include <MW/Math/detail/Vec2.h>
#include <MW/Math/detail/Vec3.h>
#include <MW/Math/detail/Mat4.h>
#include <algorithm>
#include <cmath>

namespace mw {
inline namespace math {

inline Vec4 Vec4::Zero() { return Vec4(0, 0, 0, 0); }
inline Vec4 Vec4::One() { return Vec4(1, 1, 1, 1); }
inline Vec4 Vec4::Right() { return Vec4(1, 0, 0, 0); }
inline Vec4 Vec4::Left() { return Vec4(-1, 0, 0, 0); }
inline Vec4 Vec4::Up() { return Vec4(0, 1, 0, 0); }
inline Vec4 Vec4::Down() { return Vec4(0, -1, 0, 0); }
inline Vec4 Vec4::Forward() { return Vec4(0, 0, 1, 0); }
inline Vec4 Vec4::Backward() { return Vec4(0, 0, -1, 0); }

inline Vec4::Vec4() : Vec4(0, 0, 0, 0) { }
inline Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
inline Vec4::Vec4(const Vec2& v) : x(v.x), y(v.y), z(0), w(0) {}
inline Vec4::Vec4(const Vec2& v, float z, float w) : x(v.x), y(v.y), z(z), w(w) {}
inline Vec4::Vec4(const Vec3& v) : x(v.x), y(v.y), z(v.z), w(0) {}
inline Vec4::Vec4(const Vec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
inline Vec4::operator Vec2() const { return Vec2(x, y); }
inline Vec4::operator Vec3() const { return Vec3(x, y, z); }

inline float& Vec4::operator[](size_t index) {
    assert(index >= 0 && index < 4);
    return ((float*)this)[index];
}

inline float Vec4::operator[](size_t index) const {
    return (*(Vec4*)this)[index];
}

inline Vec4 Vec4::operator-() const {
    return Vec4(-x, -y, -z, -w);
}

inline Vec4 Vec4::operator+(const Vec4& v) const {
    return Vec4(x + v.x, y + v.y, z + v.z, w + v.w);
}

inline Vec4 Vec4::operator-(const Vec4& v) const {
    return Vec4(x - v.x, y - v.y, z - v.z, w - v.w);
}

inline Vec4 Vec4::operator*(float s) const {
    return Vec4(x * s, y * s, z * s, w * s);
}

inline Vec4 Vec4::operator/(float s) const {
    return Vec4(x / s, y / s, z / s, w / s);
}

inline float Vec4::operator*(const Vec4& v) const {
    return Dot(v);
}

inline Vec4 Vec4::operator*(const Mat4& m) const {
    return Vec4(
        m.m11 * x + m.m21 * y + m.m31 * z + m.m41 * w,
        m.m12 * x + m.m22 * y + m.m32 * z + m.m42 * w,
        m.m13 * x + m.m23 * y + m.m33 * z + m.m43 * w,
        m.m14 * x + m.m24 * y + m.m34 * z + m.m44 * w);
}

inline Vec4& Vec4::operator+=(const Vec4& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

inline Vec4& Vec4::operator-=(const Vec4& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

inline Vec4& Vec4::operator*=(float s) {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}

inline Vec4& Vec4::operator/=(float s) {
    float rs = 1.0f / s;
    x *= rs;
    y *= rs;
    z *= rs;
    w *= rs;
    return *this;
}

inline Vec4& Vec4::operator*=(const Mat4& m) {
    return (*this = *this * m);
}

inline bool Vec4::operator==(const Vec4& v) const {
    return (x == v.x && y == v.y && z == v.z && w == v.w);
}

inline bool Vec4::operator!=(const Vec4& v) const {
    return (x != v.x || y != v.y || z != v.z || w != v.w);
}

inline Vec4 Vec4::Multiply(const Vec4& v) const {
    return Vec4(x * v.x, y * v.y, z * v.z, w * v.w);
}

inline Vec4 Vec4::Divide(const Vec4& v) const {
    return Vec4(x / v.x, y / v.y, z / v.z, w / v.w);
}

inline float Vec4::Dot(const Vec4& v) const {
    return x * v.x + y * v.y + z * v.z + w * v.w;
}

//float Dot(const Plane& p) const {
//    return x * p.a + y * p.b + z * p.c + w * p.d;
//}

inline float Vec4::Distance(const Vec4& v) const {
    return (v - *this).Length();
}

inline float Vec4::DistanceSq(const Vec4& v) const {
    return (v - *this).LengthSq();
}

inline float Vec4::Length() const {
    return sqrt(x * x + y * y + z * z + w * w);
}

inline float Vec4::LengthSq() const {
    return Dot(*this);
}

inline void Vec4::Normalize() {
    float sqLen = x * x + y * y + z * z + w * w;
    if (sqLen >= 1e-6)
        *this /= sqrt(sqLen);
}

inline Vec4 Vec4::Normalized() const {
    float sqLen = x * x + y * y + z * z + w * w;
    return sqLen >= 1e-6 ?
        *this / sqrt(sqLen) : *this;
}

inline bool Vec4::IsNormalized() const {
    float sqLen = x * x + y * y + z * z + w * w;
    return abs(1.0f - sqLen) <= 1e-6;
}

inline Vec4 Vec4::Lerp(const Vec4& a, const Vec4& b, float t) {
    return a + (b - a) * t;
}

inline std::ostream& operator<<(std::ostream& os, const Vec4& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return os;
}

}
}
