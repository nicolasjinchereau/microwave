/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <ostream>
#include <sstream>

namespace mw {
inline namespace math {

class Vec2
{
public:
    float x;
    float y;

    static Vec2 Zero();
    static Vec2 One();
    static Vec2 Right();
    static Vec2 Left();
    static Vec2 Up();
    static Vec2 Down();

    Vec2();
    Vec2(float x, float y);

    float& operator[](size_t index);
    float operator[](size_t index) const;
    Vec2 operator-() const;
    Vec2 operator+(const Vec2& v) const;
    Vec2 operator-(const Vec2& v) const;
    Vec2 operator*(float s) const;
    Vec2 operator/(float s) const;
    float operator*(const Vec2& v) const;
    Vec2& operator+=(const Vec2& v);
    Vec2& operator-=(const Vec2& v);
    Vec2& operator*=(float s);
    Vec2& operator/=(float s);
    bool operator==(const Vec2& v) const;
    bool operator!=(const Vec2& v) const;
    Vec2 PerpCW() const;
    Vec2 PerpCCW() const;
    Vec2 Multiply(const Vec2& v) const;
    Vec2 Divide(const Vec2& v) const;
    float Det(const Vec2& v) const;
    float Dot(const Vec2& v) const;
    float Length() const;
    float LengthSq() const;
    float Distance(const Vec2& v) const;
    float DistanceSq(const Vec2& v) const;
    void Normalize();
    Vec2 Normalized() const;
    bool IsNormalized() const;
    static Vec2 Lerp(const Vec2& a, const Vec2& b, float t);

    std::ostream& operator<<(std::ostream& os);
};

}
}
