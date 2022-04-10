/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Vec4_p1;
import <ostream>;
import <cstdint>;

export namespace mw {
inline namespace math {

class Vec2;
class Vec3;

class Vec4
{
public:
    float x;
    float y;
    float z;
    float w;

    static Vec4 Zero();
    static Vec4 One();
    static Vec4 Right();
    static Vec4 Left();
    static Vec4 Up();
    static Vec4 Down();
    static Vec4 Forward();
    static Vec4 Backward();

    Vec4();
    Vec4(float x, float y, float z, float w);
    Vec4(const Vec2& v);
    Vec4(const Vec2& v, float z, float w);
    Vec4(const Vec3& v);
    Vec4(const Vec3& v, float w);

    operator Vec2() const;
    operator Vec3() const;

    float& operator[](size_t index);
    float operator[](size_t index) const;
    Vec4 operator-() const;
    Vec4 operator+(const Vec4& v) const;
    Vec4 operator-(const Vec4& v) const;
    Vec4 operator*(float s) const;
    Vec4 operator/(float s) const;
    float operator*(const Vec4& v) const;
    Vec4& operator+=(const Vec4& v);
    Vec4& operator-=(const Vec4& v);
    Vec4& operator*=(float s);
    Vec4& operator/=(float s);
    bool operator==(const Vec4& v) const;
    bool operator!=(const Vec4& v) const;
    Vec4 Multiply(const Vec4& v) const;
    Vec4 Divide(const Vec4& v) const;
    float Dot(const Vec4& v) const;
    //float Dot(const Plane& p) const;
    float Distance(const Vec4& v) const;
    float DistanceSq(const Vec4& v) const;
    float Length() const;
    float LengthSq() const;
    void Normalize();
    Vec4 Normalized() const;
    bool IsNormalized() const;
    static Vec4 Lerp(const Vec4& a, const Vec4& b, float t);

    std::ostream& operator<<(std::ostream& os) const;
};

}
}
