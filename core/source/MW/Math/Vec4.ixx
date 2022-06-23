/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Vec4;
import Microwave.Math.Vec2;
import Microwave.Math.Vec3;
import Microwave.System.Exception;
import <MW/System/Debug.h>;
import <algorithm>;
import <cmath>;
import <cstdint>;
import <ostream>;

export namespace mw {
inline namespace math {

class Vec4
{
public:
    float x;
    float y;
    float z;
    float w;

    static Vec4 Zero() { return Vec4(0, 0, 0, 0); }
    static Vec4 One() { return Vec4(1, 1, 1, 1); }
    static Vec4 Right() { return Vec4(1, 0, 0, 0); }
    static Vec4 Left() { return Vec4(-1, 0, 0, 0); }
    static Vec4 Up() { return Vec4(0, 1, 0, 0); }
    static Vec4 Down() { return Vec4(0, -1, 0, 0); }
    static Vec4 Forward() { return Vec4(0, 0, 1, 0); }
    static Vec4 Backward() { return Vec4(0, 0, -1, 0); }

    Vec4() : Vec4(0, 0, 0, 0) { }
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vec4(const Vec2& v) : x(v.x), y(v.y), z(0), w(0) {}
    Vec4(const Vec2& v, float z, float w) : x(v.x), y(v.y), z(z), w(w) {}
    Vec4(const Vec3& v) : x(v.x), y(v.y), z(v.z), w(0) {}
    Vec4(const Vec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
    operator Vec2() const { return Vec2(x, y); }
    operator Vec3() const { return Vec3(x, y, z); }

    float& operator[](size_t index) {
        Assert(index >= 0 && index < 4);
        return ((float*)this)[index];
    }

    float operator[](size_t index) const {
        return (*(Vec4*)this)[index];
    }

    Vec4 operator-() const {
        return Vec4(-x, -y, -z, -w);
    }

    Vec4 operator+(const Vec4& v) const {
        return Vec4(x + v.x, y + v.y, z + v.z, w + v.w);
    }

    Vec4 operator-(const Vec4& v) const {
        return Vec4(x - v.x, y - v.y, z - v.z, w - v.w);
    }

    Vec4 operator*(float s) const {
        return Vec4(x * s, y * s, z * s, w * s);
    }

    Vec4 operator/(float s) const {
        return Vec4(x / s, y / s, z / s, w / s);
    }

    float operator*(const Vec4& v) const {
        return Dot(v);
    }

    Vec4& operator+=(const Vec4& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    Vec4& operator-=(const Vec4& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    Vec4& operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }

    Vec4& operator/=(float s) {
        float rs = 1.0f / s;
        x *= rs;
        y *= rs;
        z *= rs;
        w *= rs;
        return *this;
    }

    bool operator==(const Vec4& v) const {
        return (x == v.x && y == v.y && z == v.z && w == v.w);
    }

    bool operator!=(const Vec4& v) const {
        return (x != v.x || y != v.y || z != v.z || w != v.w);
    }

    Vec4 Multiply(const Vec4& v) const {
        return Vec4(x * v.x, y * v.y, z * v.z, w * v.w);
    }

    Vec4 Divide(const Vec4& v) const {
        return Vec4(x / v.x, y / v.y, z / v.z, w / v.w);
    }

    float Dot(const Vec4& v) const {
        return x * v.x + y * v.y + z * v.z + w * v.w;
    }

    float Distance(const Vec4& v) const {
        return (v - *this).Length();
    }

    float DistanceSq(const Vec4& v) const {
        return (v - *this).LengthSq();
    }

    float Length() const {
        return sqrt(x * x + y * y + z * z + w * w);
    }

    float LengthSq() const {
        return Dot(*this);
    }

    void Normalize() {
        float sqLen = x * x + y * y + z * z + w * w;
        if (sqLen >= 1e-6)
            *this /= sqrt(sqLen);
    }

    Vec4 Normalized() const {
        float sqLen = x * x + y * y + z * z + w * w;
        return sqLen >= 1e-6 ?
            *this / sqrt(sqLen) : *this;
    }

    bool IsNormalized() const {
        float sqLen = x * x + y * y + z * z + w * w;
        return abs(1.0f - sqLen) <= 1e-6;
    }

    static Vec4 Lerp(const Vec4& a, const Vec4& b, float t) {
        return a + (b - a) * t;
    }

    std::ostream& operator<<(std::ostream& os) const {
        os << "(" << x << ", " << y << ", " << z << ", " << w << ")";
        return os;
    }
};

} // math
} // mw
