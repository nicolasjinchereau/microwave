/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Vec2;
import Microwave.System.Exception;
import <MW/System/Debug.h>;
import <ostream>;
import <cmath>;

export namespace mw {
inline namespace math {

class Vec2
{
public:
    float x;
    float y;

    static Vec2 Zero() { return Vec2(0, 0); }
    static Vec2 One() { return Vec2(1, 1); }
    static Vec2 Right() { return Vec2(1, 0); }
    static Vec2 Left() { return Vec2(-1, 0); }
    static Vec2 Up() { return Vec2(0, 1); }
    static Vec2 Down() { return Vec2(0, -1); }

    Vec2() : Vec2(0, 0) { }
    Vec2(float x, float y) : x(x), y(y) { }

    float& operator[](size_t index) {
        Assert(index >= 0 && index < 2);
        return ((float*)this)[index];
    }

    float operator[](size_t index) const {
        return (*(Vec2*)this)[index];
    }

    Vec2 operator-() const {
        return Vec2(-x, -y);
    }

    Vec2 operator+(const Vec2& v) const {
        return Vec2(x + v.x, y + v.y);
    }

    Vec2 operator-(const Vec2& v) const {
        return Vec2(x - v.x, y - v.y);
    }

    Vec2 operator*(float s) const {
        return Vec2(x * s, y * s);
    }

    Vec2 operator/(float s) const {
        return Vec2(x / s, y / s);
    }

    float operator*(const Vec2& v) const {
        return x * v.x + y * v.y;
    }

    Vec2& operator+=(const Vec2& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vec2& operator*=(float s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    Vec2& operator/=(float s)
    {
        x /= s;
        y /= s;
        return *this;
    }

    bool operator==(const Vec2& v) const {
        return (x == v.x && y == v.y);
    }

    bool operator!=(const Vec2& v) const {
        return (x != v.x || y != v.y);
    }

    Vec2 PerpCW() const {
        return Vec2(y, -x);
    }

    Vec2 PerpCCW() const {
        return Vec2(-y, x);
    }

    Vec2 Multiply(const Vec2& v) const {
        return Vec2(x * v.x, y * v.y);
    }

    Vec2 Divide(const Vec2& v) const {
        return Vec2(x / v.x, y / v.y);
    }

    float Det(const Vec2& v) const {
        return x * v.y - y * v.x;
    }

    float Dot(const Vec2& v) const {
        return x * v.x + y * v.y;
    }

    float Length() const {
        return sqrt(x * x + y * y);
    }

    float LengthSq() const {
        return x * x + y * y;
    }

    float Distance(const Vec2& v) const {
        return Vec2(v.x - x, v.y - y).Length();
    }

    float DistanceSq(const Vec2& v) const {
        return Vec2(v.x - x, v.y - y).LengthSq();
    }

    void Normalize()
    {
        float sqLen = x * x + y * y;
        if (sqLen >= 1e-6 && abs(1.0f - sqLen) > 1e-6)
            *this /= sqrt(sqLen);
    }

    Vec2 Normalized() const
    {
        float sqLen = x * x + y * y;
        return sqLen >= 1e-6 && abs(1.0f - sqLen) > 1e-6 ?
            *this / sqrt(sqLen) : *this;
    }

    bool IsNormalized() const
    {
        float sqLen = x * x + y * y;
        return abs(1.0f - sqLen) <= 1e-6;
    }

    static Vec2 Lerp(const Vec2& a, const Vec2& b, float t) {
        return a + (b - a) * t;
    }

    std::ostream& operator<<(std::ostream& os) const {
        os << "(" << x << ", " << y << ")";
        return os;
    }
};

} // math
} // mw
