/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Vec4_p2;
import :Vec4_p1;
import :Vec2_p1;
import :Vec3_p1;
import <algorithm>;
import <cassert>;
import <cmath>;
import <cstdint>;
import <ostream>;

export namespace mw {
inline namespace math {

Vec4 Vec4::Zero() { return Vec4(0, 0, 0, 0); }
Vec4 Vec4::One() { return Vec4(1, 1, 1, 1); }
Vec4 Vec4::Right() { return Vec4(1, 0, 0, 0); }
Vec4 Vec4::Left() { return Vec4(-1, 0, 0, 0); }
Vec4 Vec4::Up() { return Vec4(0, 1, 0, 0); }
Vec4 Vec4::Down() { return Vec4(0, -1, 0, 0); }
Vec4 Vec4::Forward() { return Vec4(0, 0, 1, 0); }
Vec4 Vec4::Backward() { return Vec4(0, 0, -1, 0); }

Vec4::Vec4() : Vec4(0, 0, 0, 0) { }
Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
Vec4::Vec4(const Vec2& v) : x(v.x), y(v.y), z(0), w(0) {}
Vec4::Vec4(const Vec2& v, float z, float w) : x(v.x), y(v.y), z(z), w(w) {}
Vec4::Vec4(const Vec3& v) : x(v.x), y(v.y), z(v.z), w(0) {}
Vec4::Vec4(const Vec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
Vec4::operator Vec2() const { return Vec2(x, y); }
Vec4::operator Vec3() const { return Vec3(x, y, z); }

float& Vec4::operator[](size_t index) {
    assert(index >= 0 && index < 4);
    return ((float*)this)[index];
}

float Vec4::operator[](size_t index) const {
    return (*(Vec4*)this)[index];
}

Vec4 Vec4::operator-() const {
    return Vec4(-x, -y, -z, -w);
}

Vec4 Vec4::operator+(const Vec4& v) const {
    return Vec4(x + v.x, y + v.y, z + v.z, w + v.w);
}

Vec4 Vec4::operator-(const Vec4& v) const {
    return Vec4(x - v.x, y - v.y, z - v.z, w - v.w);
}

Vec4 Vec4::operator*(float s) const {
    return Vec4(x * s, y * s, z * s, w * s);
}

Vec4 Vec4::operator/(float s) const {
    return Vec4(x / s, y / s, z / s, w / s);
}

float Vec4::operator*(const Vec4& v) const {
    return Dot(v);
}

Vec4& Vec4::operator+=(const Vec4& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

Vec4& Vec4::operator-=(const Vec4& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

Vec4& Vec4::operator*=(float s) {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}

Vec4& Vec4::operator/=(float s) {
    float rs = 1.0f / s;
    x *= rs;
    y *= rs;
    z *= rs;
    w *= rs;
    return *this;
}

bool Vec4::operator==(const Vec4& v) const {
    return (x == v.x && y == v.y && z == v.z && w == v.w);
}

bool Vec4::operator!=(const Vec4& v) const {
    return (x != v.x || y != v.y || z != v.z || w != v.w);
}

Vec4 Vec4::Multiply(const Vec4& v) const {
    return Vec4(x * v.x, y * v.y, z * v.z, w * v.w);
}

Vec4 Vec4::Divide(const Vec4& v) const {
    return Vec4(x / v.x, y / v.y, z / v.z, w / v.w);
}

float Vec4::Dot(const Vec4& v) const {
    return x * v.x + y * v.y + z * v.z + w * v.w;
}

//float Dot(const Plane& p) const {
//    return x * p.a + y * p.b + z * p.c + w * p.d;
//}

float Vec4::Distance(const Vec4& v) const {
    return (v - *this).Length();
}

float Vec4::DistanceSq(const Vec4& v) const {
    return (v - *this).LengthSq();
}

float Vec4::Length() const {
    return sqrt(x * x + y * y + z * z + w * w);
}

float Vec4::LengthSq() const {
    return Dot(*this);
}

void Vec4::Normalize() {
    float sqLen = x * x + y * y + z * z + w * w;
    if (sqLen >= 1e-6)
        *this /= sqrt(sqLen);
}

Vec4 Vec4::Normalized() const {
    float sqLen = x * x + y * y + z * z + w * w;
    return sqLen >= 1e-6 ?
        *this / sqrt(sqLen) : *this;
}

bool Vec4::IsNormalized() const {
    float sqLen = x * x + y * y + z * z + w * w;
    return abs(1.0f - sqLen) <= 1e-6;
}

Vec4 Vec4::Lerp(const Vec4& a, const Vec4& b, float t) {
    return a + (b - a) * t;
}

std::ostream& Vec4::operator<<(std::ostream& os) const {
    os << "(" << x << ", " << y << ", " << z << ", " << w << ")";
    return os;
}

}
}
