/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <ostream>
#include <sstream>

namespace mw {
inline namespace math {

class Vec2;
class Quat;
class Mat3;
class Plane;

class Vec3
{
public:
    float x;
    float y;
    float z;

    static Vec3 Zero();
    static Vec3 One();
    static Vec3 Right();
    static Vec3 Left();
    static Vec3 Up();
    static Vec3 Down();
    static Vec3 Forward();
    static Vec3 Backward();

    Vec3();
    Vec3(float x, float y, float z);
    Vec3(const Vec2& v);
    operator Vec2() const;

    float& operator[](size_t index);
    float operator[](size_t index) const;
    Vec3 operator-() const;
    Vec3 operator+(const Vec3& v) const;
    Vec3 operator-(const Vec3& v) const;
    Vec3 operator*(float s) const;
    Vec3 operator/(float s) const;
    float operator*(const Vec3& v) const;
    Vec3 operator*(const Mat3& m) const;
    Vec3 operator*(const Quat& q) const;
    Vec3& operator+=(const Vec3& v);
    Vec3& operator-=(const Vec3& v);
    Vec3& operator*=(float s);
    Vec3& operator/=(float s);
    Vec3& operator*=(const Quat& q);
    Vec3& operator*=(const Mat3& m);
    bool operator==(const Vec3& v) const;
    bool operator!=(const Vec3& v) const;
    Vec3 Multiply(const Vec3& vec) const;
    Vec3 Divide(const Vec3& vec) const;
    float Dot(const Vec3& v) const;
    float Dot(const Plane& p) const;
    Vec3 Cross(const Vec3& v) const;
    float Angle(const Vec3& v) const;
    static float Angle(const Vec3& a, const Vec3& b);
    float MaxAcuteAngle(const Vec3& v) const;
    float Length() const;
    float LengthSq() const;
    float Distance(const Vec3& v) const;
    float DistanceSq(const Vec3& v) const;
    void Normalize();
    Vec3 Normalized() const;
    bool IsNormalized() const;
    static Vec3 Lerp(const Vec3& a, const Vec3& b, float t);

    friend std::ostream& operator<<(std::ostream& os, const Vec3& v);
};

}
}
