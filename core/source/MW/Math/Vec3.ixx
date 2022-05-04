/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Vec3;
import Microwave.Math.Vec2;
import Microwave.Math.Constants;
import <algorithm>;
import <cassert>;
import <cmath>;
import <cstdint>;
import <ostream>;

export namespace mw {
inline namespace math {

class Vec3
{
public:
    float x;
    float y;
    float z;

    static Vec3 Zero() { return Vec3(0, 0, 0); }
    static Vec3 One() { return Vec3(1, 1, 1); }
    static Vec3 Right() { return Vec3(1, 0, 0); }
    static Vec3 Left() { return Vec3(-1, 0, 0); }
    static Vec3 Up() { return Vec3(0, 1, 0); }
    static Vec3 Down() { return Vec3(0, -1, 0); }
    static Vec3 Forward() { return Vec3(0, 0, 1); }
    static Vec3 Backward() { return Vec3(0, 0, -1); }

    Vec3() : Vec3(0, 0, 0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3(const Vec2& v) : x(v.x), y(v.y), z(0) {}
    operator Vec2() const { return Vec2(x, y); }

    float& operator[](size_t index) {
        assert(index >= 0 && index < 3);
        return ((float*)this)[index];
    }

    float operator[](size_t index) const {
        return (*(Vec3*)this)[index];
    }

    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator+(const Vec3& v) const {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }

    Vec3 operator-(const Vec3& v) const {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }

    Vec3 operator*(float s) const {
        return Vec3(x * s, y * s, z * s);
    }

    Vec3 operator/(float s) const {
        return Vec3(x / s, y / s, z / s);
    }

    float operator*(const Vec3& v) const {
        return Dot(v);
    }

    Vec3& operator+=(const Vec3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vec3& operator*=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vec3& operator/=(float s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    bool operator==(const Vec3& v) const {
        return (x == v.x && y == v.y && z == v.z);
    }

    bool operator!=(const Vec3& v) const {
        return (x != v.x || y != v.y || z != v.z);
    }

    Vec3 Multiply(const Vec3& v) const {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }

    Vec3 Divide(const Vec3& v) const {
        return Vec3(x / v.x, y / v.y, z / v.z);
    }

    float Dot(const Vec3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vec3 Cross(const Vec3& v) const
    {
        return Vec3((y * v.z) - (z * v.y),
            (z * v.x) - (x * v.z),
            (x * v.y) - (y * v.x));
    }

    float Angle(const Vec3& v) const {
        return Angle(*this, v);
    }

    static float Angle(const Vec3& a, const Vec3& b) {
        return acos(a.Dot(b)) * RadToDeg;
    }

    float Length() const {
        return sqrt(x * x + y * y + z * z);
    }

    float LengthSq() const {
        return Dot(*this);
    }

    float Distance(const Vec3& v) const {
        return (v - *this).Length();
    }

    float DistanceSq(const Vec3& v) const {
        return (v - *this).LengthSq();
    }

    void Normalize()
    {
        float sqLen = x * x + y * y + z * z;
        if (sqLen >= 1e-6 && abs(1.0f - sqLen) > 1e-6)
            *this /= sqrt(sqLen);
    }

    Vec3 Normalized() const
    {
        float sqLen = x * x + y * y + z * z;
        return sqLen >= 1e-6 && abs(1.0f - sqLen) > 1e-6 ?
            *this / sqrt(sqLen) : *this;
    }

    bool IsNormalized() const {
        float sqLen = x * x + y * y + z * z;
        return abs(1.0f - sqLen) <= 1e-6;
    }

    static Vec3 Lerp(const Vec3& a, const Vec3& b, float t) {
        return a + (b - a) * t;
    }

    std::ostream& operator<<(std::ostream& os) const {
        os << "(" << x << ", " << y << ", " << z << ")";
        return os;
    }
};

} // math
} // mw
