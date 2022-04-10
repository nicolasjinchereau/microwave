/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Quat_p1;
import :Vec3_p1;
import <ostream>;

export namespace mw {
inline namespace math {

class Mat3;

class Quat
{
public:
    Vec3 v;
    float w;

    static Quat Zero();
    static Quat Identity();

    Quat();
    Quat(float x, float y, float z, float w);
    Quat(const Vec3& v, float w);
    Quat(float xAngle, float yAngle, float zAngle);
    Quat(const Vec3& angles);

    Quat operator+(const Quat& q) const;
    Quat operator-(const Quat& q) const;
    Quat operator*(const Quat& q) const;
    Quat operator/(const Quat& q) const;
    Quat operator*(float s) const;
    Quat operator/(float s) const;
    Quat operator-() const;
    const Quat& operator+=(const Quat& q);
    const Quat& operator-=(const Quat& q);
    const Quat& operator*=(const Quat& q);
    const Quat& operator*=(float s);
    const Quat& operator/=(float s);
    float Dot(const Quat& q) const;

    float Length() const;
    float LengthSq() const;
    void Normalize();
    Quat Normalized() const;
    void Invert();
    Quat Inverse() const;
    Quat Conjugate() const;
    Mat3 ToMatrix() const;

    static Quat FromMatrix(const Mat3& matrix);
    static Quat AngleAxis(float angle, const Vec3& axis);
    static Quat FromTo(const Vec3& from, const Vec3& to);
    static Quat LookRotation(const Vec3& forward, const Vec3& upward);
    static Quat Lerp(const Quat& a, const Quat& b, float t);
    static Quat Slerp(const Quat& a, const Quat& b, float t);

    void ToAngleAxis(float& angle, Vec3& axis) const;
    Vec3 ToEulerAngles() const;

    std::ostream& operator<<(std::ostream& os) const;
};

}
}
