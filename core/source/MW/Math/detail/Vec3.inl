/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/Vec3.h>
#include <MW/Math/detail/Vec2.h>
#include <MW/Math/detail/Quat.h>
#include <MW/Math/detail/Mat3.h>
#include <MW/Math/detail/Plane.h>
#include <MW/Math/detail/Functions.h>
#include <cmath>

namespace mw {
inline namespace math {

inline Vec3 Vec3::Zero() { return Vec3(0, 0, 0); }
inline Vec3 Vec3::One() { return Vec3(1, 1, 1); }
inline Vec3 Vec3::Right() { return Vec3(1, 0, 0); }
inline Vec3 Vec3::Left() { return Vec3(-1, 0, 0); }
inline Vec3 Vec3::Up() { return Vec3(0, 1, 0); }
inline Vec3 Vec3::Down() { return Vec3(0, -1, 0); }
inline Vec3 Vec3::Forward() { return Vec3(0, 0, 1); }
inline Vec3 Vec3::Backward() { return Vec3(0, 0, -1); }

inline Vec3::Vec3() : Vec3(0, 0, 0) {}
inline Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
inline Vec3::Vec3(const Vec2& v) : x(v.x), y(v.y), z(0) {}
inline Vec3::operator Vec2() const { return Vec2(x, y); }

inline float& Vec3::operator[](size_t index) {
    assert(index >= 0 && index < 3);
    return ((float*)this)[index];
}

inline float Vec3::operator[](size_t index) const {
    return (*(Vec3*)this)[index];
}

inline Vec3 Vec3::operator-() const {
    return Vec3(-x, -y, -z);
}

inline Vec3 Vec3::operator+(const Vec3& v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
}

inline Vec3 Vec3::operator-(const Vec3& v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
}

inline Vec3 Vec3::operator*(float s) const {
    return Vec3(x * s, y * s, z * s);
}

inline Vec3 Vec3::operator/(float s) const {
    return Vec3(x / s, y / s, z / s);
}

inline float Vec3::operator*(const Vec3& v) const {
    return Dot(v);
}

inline Vec3 Vec3::operator*(const Quat& q) const
{
    Vec3 uv = q.v.Cross(*this);
    Vec3 uuv = q.v.Cross(uv);
    uv = uv * (2.0f * q.w);
    uuv = uuv * 2.0f;
    return *this + uv + uuv;
}

inline Vec3 Vec3::operator*(const Mat3& m) const
{
    return Vec3(
        x * m.m11 + y * m.m21 + z * m.m31,
        x * m.m12 + y * m.m22 + z * m.m32,
        x * m.m13 + y * m.m23 + z * m.m33);
}

inline Vec3& Vec3::operator+=(const Vec3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

inline Vec3& Vec3::operator-=(const Vec3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

inline Vec3& Vec3::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

inline Vec3& Vec3::operator/=(float s)
{
    x /= s;
    y /= s;
    z /= s;
    return *this;
}

inline Vec3& Vec3::operator*=(const Quat& q) {
    return (*this = *this * q);
}

inline Vec3& Vec3::operator*=(const Mat3& m) {
    return *this = *this * m;
}

inline bool Vec3::operator==(const Vec3& v) const {
    return (x == v.x && y == v.y && z == v.z);
}

inline bool Vec3::operator!=(const Vec3& v) const {
    return (x != v.x || y != v.y || z != v.z);
}

inline Vec3 Vec3::Multiply(const Vec3& v) const {
    return Vec3(x * v.x, y * v.y, z * v.z);
}

inline Vec3 Vec3::Divide(const Vec3& v) const {
    return Vec3(x / v.x, y / v.y, z / v.z);
}

inline float Vec3::Dot(const Vec3& v) const {
    return x * v.x + y * v.y + z * v.z;
}

inline float Vec3::Dot(const Plane& p) const {
    return x * p.a + y * p.b + z * p.c + p.d;
}

inline Vec3 Vec3::Cross(const Vec3& v) const
{
    return Vec3((y * v.z) - (z * v.y),
        (z * v.x) - (x * v.z),
        (x * v.y) - (y * v.x));
}

inline float Vec3::Angle(const Vec3& v) const {
    return Angle(*this, v);
}

inline float Vec3::Angle(const Vec3& a, const Vec3& b) {
    return acos(a.Dot(b)) * RadToDeg;
}

inline float Vec3::MaxAcuteAngle(const Vec3& v) const {
    auto dot = Dot(v);
    auto maxval = std::max(dot, 0.0f);
    return acos(maxval) * RadToDeg;
}

inline float Vec3::Length() const {
    return sqrt(x * x + y * y + z * z);
}

inline float Vec3::LengthSq() const {
    return Dot(*this);
}

inline float Vec3::Distance(const Vec3& v) const {
    return (v - *this).Length();
}

inline float Vec3::DistanceSq(const Vec3& v) const {
    return (v - *this).LengthSq();
}

inline void Vec3::Normalize()
{
    float sqLen = x * x + y * y + z * z;
    if (sqLen >= 1e-6 && abs(1.0f - sqLen) > 1e-6)
        *this /= sqrt(sqLen);
}

inline Vec3 Vec3::Normalized() const
{
    float sqLen = x * x + y * y + z * z;
    return sqLen >= 1e-6 && abs(1.0f - sqLen) > 1e-6 ?
        *this / sqrt(sqLen) : *this;
}

inline bool Vec3::IsNormalized() const {
    float sqLen = x * x + y * y + z * z;
    return abs(1.0f - sqLen) <= 1e-6;
}

inline Vec3 Vec3::Lerp(const Vec3& a, const Vec3& b, float t) {
    return a + (b - a) * t;
}

inline std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

}
}
