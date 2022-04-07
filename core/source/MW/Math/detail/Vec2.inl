/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/Vec2.h>
#include <cassert>
#include <cmath>

namespace mw {
inline namespace math {

inline Vec2 Vec2::Zero() { return Vec2(0, 0); }
inline Vec2 Vec2::One() { return Vec2(1, 1); }
inline Vec2 Vec2::Right() { return Vec2(1, 0); }
inline Vec2 Vec2::Left() { return Vec2(-1, 0); }
inline Vec2 Vec2::Up() { return Vec2(0, 1); }
inline Vec2 Vec2::Down() { return Vec2(0, -1); }

inline Vec2::Vec2() : Vec2(0, 0) { }
inline Vec2::Vec2(float x, float y) : x(x), y(y) { }

inline float& Vec2::operator[](size_t index) {
    assert(index >= 0 && index < 2);
    return ((float*)this)[index];
}

inline float Vec2::operator[](size_t index) const {
    return (*(Vec2*)this)[index];
}

inline Vec2 Vec2::operator-() const {
    return Vec2(-x, -y);
}

inline Vec2 Vec2::operator+(const Vec2& v) const {
    return Vec2(x + v.x, y + v.y);
}

inline Vec2 Vec2::operator-(const Vec2& v) const {
    return Vec2(x - v.x, y - v.y);
}

inline Vec2 Vec2::operator*(float s) const {
    return Vec2(x * s, y * s);
}

inline Vec2 Vec2::operator/(float s) const {
    return Vec2(x / s, y / s);
}

inline float Vec2::operator*(const Vec2& v) const {
    return x * v.x + y * v.y;
}

inline Vec2& Vec2::operator+=(const Vec2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}

inline Vec2& Vec2::operator-=(const Vec2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

inline Vec2& Vec2::operator*=(float s)
{
    x *= s;
    y *= s;
    return *this;
}

inline Vec2& Vec2::operator/=(float s)
{
    x /= s;
    y /= s;
    return *this;
}

inline bool Vec2::operator==(const Vec2& v) const {
    return (x == v.x && y == v.y);
}

inline bool Vec2::operator!=(const Vec2& v) const {
    return (x != v.x || y != v.y);
}

inline Vec2 Vec2::PerpCW() const {
    return Vec2(y, -x);
}

inline Vec2 Vec2::PerpCCW() const {
    return Vec2(-y, x);
}

inline Vec2 Vec2::Multiply(const Vec2& v) const {
    return Vec2(x * v.x, y * v.y);
}

inline Vec2 Vec2::Divide(const Vec2& v) const {
    return Vec2(x / v.x, y / v.y);
}

inline float Vec2::Det(const Vec2& v) const {
    return x * v.y - y * v.x;
}

inline float Vec2::Dot(const Vec2& v) const {
    return x * v.x + y * v.y;
}

inline float Vec2::Length() const {
    return sqrt(x * x + y * y);
}

inline float Vec2::LengthSq() const {
    return x * x + y * y;
}

inline float Vec2::Distance(const Vec2& v) const {
    return Vec2(v.x - x, v.y - y).Length();
}

inline float Vec2::DistanceSq(const Vec2& v) const {
    return Vec2(v.x - x, v.y - y).LengthSq();
}

inline void Vec2::Normalize()
{
    float sqLen = x * x + y * y;
    if (sqLen >= 1e-6 && abs(1.0f - sqLen) > 1e-6)
        *this /= sqrt(sqLen);
}

inline Vec2 Vec2::Normalized() const
{
    float sqLen = x * x + y * y;
    return sqLen >= 1e-6 && abs(1.0f - sqLen) > 1e-6 ?
        *this / sqrt(sqLen) : *this;
}

inline bool Vec2::IsNormalized() const
{
    float sqLen = x * x + y * y;
    return abs(1.0f - sqLen) <= 1e-6;
}

inline Vec2 Vec2::Lerp(const Vec2& a, const Vec2& b, float t) {
    return a + (b - a) * t;
}

inline std::ostream& Vec2::operator<<(std::ostream& os) {
    os << "(" << x << ", " << y << ")";
    return os;
}

}
}
