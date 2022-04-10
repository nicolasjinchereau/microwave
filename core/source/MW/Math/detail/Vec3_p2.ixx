/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Vec3_p2;
import :Vec3_p1;
import :Vec2_p1;
import :Mat3_p1;
import :Plane_p1;
import :Functions_p1;
import <algorithm>;
import <cmath>;
import <cassert>;
import <ostream>;
import <cstdint>;

export namespace mw {
inline namespace math {

Vec3 Vec3::Zero() { return Vec3(0, 0, 0); }
Vec3 Vec3::One() { return Vec3(1, 1, 1); }
Vec3 Vec3::Right() { return Vec3(1, 0, 0); }
Vec3 Vec3::Left() { return Vec3(-1, 0, 0); }
Vec3 Vec3::Up() { return Vec3(0, 1, 0); }
Vec3 Vec3::Down() { return Vec3(0, -1, 0); }
Vec3 Vec3::Forward() { return Vec3(0, 0, 1); }
Vec3 Vec3::Backward() { return Vec3(0, 0, -1); }

Vec3::Vec3() : Vec3(0, 0, 0) {}
Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
Vec3::Vec3(const Vec2& v) : x(v.x), y(v.y), z(0) {}
Vec3::operator Vec2() const { return Vec2(x, y); }

float& Vec3::operator[](size_t index) {
    assert(index >= 0 && index < 3);
    return ((float*)this)[index];
}

float Vec3::operator[](size_t index) const {
    return (*(Vec3*)this)[index];
}

Vec3 Vec3::operator-() const {
    return Vec3(-x, -y, -z);
}

Vec3 Vec3::operator+(const Vec3& v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::operator-(const Vec3& v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
}

Vec3 Vec3::operator*(float s) const {
    return Vec3(x * s, y * s, z * s);
}

Vec3 Vec3::operator/(float s) const {
    return Vec3(x / s, y / s, z / s);
}

float Vec3::operator*(const Vec3& v) const {
    return Dot(v);
}

Vec3& Vec3::operator+=(const Vec3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vec3& Vec3::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

Vec3& Vec3::operator/=(float s)
{
    x /= s;
    y /= s;
    z /= s;
    return *this;
}

bool Vec3::operator==(const Vec3& v) const {
    return (x == v.x && y == v.y && z == v.z);
}

bool Vec3::operator!=(const Vec3& v) const {
    return (x != v.x || y != v.y || z != v.z);
}

Vec3 Vec3::Multiply(const Vec3& v) const {
    return Vec3(x * v.x, y * v.y, z * v.z);
}

Vec3 Vec3::Divide(const Vec3& v) const {
    return Vec3(x / v.x, y / v.y, z / v.z);
}

float Vec3::Dot(const Vec3& v) const {
    return x * v.x + y * v.y + z * v.z;
}

float Vec3::Dot(const Plane& p) const {
    return x * p.a + y * p.b + z * p.c + p.d;
}

Vec3 Vec3::Cross(const Vec3& v) const
{
    return Vec3((y * v.z) - (z * v.y),
        (z * v.x) - (x * v.z),
        (x * v.y) - (y * v.x));
}

float Vec3::Angle(const Vec3& v) const {
    return Angle(*this, v);
}

float Vec3::Angle(const Vec3& a, const Vec3& b) {
    return acos(a.Dot(b)) * RadToDeg;
}

float Vec3::MaxAcuteAngle(const Vec3& v) const {
    auto dot = Dot(v);
    auto maxval = std::max(dot, 0.0f);
    return acos(maxval) * RadToDeg;
}

float Vec3::Length() const {
    return sqrt(x * x + y * y + z * z);
}

float Vec3::LengthSq() const {
    return Dot(*this);
}

float Vec3::Distance(const Vec3& v) const {
    return (v - *this).Length();
}

float Vec3::DistanceSq(const Vec3& v) const {
    return (v - *this).LengthSq();
}

void Vec3::Normalize()
{
    float sqLen = x * x + y * y + z * z;
    if (sqLen >= 1e-6 && abs(1.0f - sqLen) > 1e-6)
        *this /= sqrt(sqLen);
}

Vec3 Vec3::Normalized() const
{
    float sqLen = x * x + y * y + z * z;
    return sqLen >= 1e-6 && abs(1.0f - sqLen) > 1e-6 ?
        *this / sqrt(sqLen) : *this;
}

bool Vec3::IsNormalized() const {
    float sqLen = x * x + y * y + z * z;
    return abs(1.0f - sqLen) <= 1e-6;
}

Vec3 Vec3::Lerp(const Vec3& a, const Vec3& b, float t) {
    return a + (b - a) * t;
}

std::ostream& Vec3::operator<<(std::ostream& os) const {
    os << "(" << x << ", " << y << ", " << z << ")";
    return os;
}

}
}
