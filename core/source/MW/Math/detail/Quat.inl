/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/Quat.h>
#include <MW/Math/detail/Vec3.h>
#include <MW/Math/detail/Mat3.h>
#include <MW/Math/detail/Functions.h>
#include <algorithm>
#include <cmath>
#include <limits>

namespace mw {
inline namespace math {

inline Quat Quat::Zero() {
    return Quat(0, 0, 0, 0);
}

inline Quat Quat::Identity() {
    return Quat(0, 0, 0, 1);
}

inline Quat::Quat()
    : Quat(0, 0, 0, 1) {}

inline Quat::Quat(float xAngle, float yAngle, float zAngle)
{
    float xRadHalf = xAngle * math::DegToRad * 0.5f;
    float yRadHalf = yAngle * math::DegToRad * 0.5f;
    float zRadHalf = zAngle * math::DegToRad * 0.5f;

    float hsinz = sin(zRadHalf);
    float hsinx = sin(xRadHalf);
    float hsiny = sin(yRadHalf);

    float hcosz = cos(zRadHalf);
    float hcosx = cos(xRadHalf);
    float hcosy = cos(yRadHalf);

    v.x = hcosy * hsinx * hcosz + hsiny * hcosx * hsinz;
    v.y = hsiny * hcosx * hcosz - hcosy * hsinx * hsinz;
    v.z = hcosy * hcosx * hsinz - hsiny * hsinx * hcosz;
    w   = hcosy * hcosx * hcosz + hsiny * hsinx * hsinz;
}

inline Quat::Quat(float x, float y, float z, float w)
    : v(x, y, z), w(w)
{
}

inline Quat::Quat(const Vec3& v, float w)
    : v(v), w(w)
{
}

inline Quat::Quat(const Vec3& angles)
    : Quat(angles.x, angles.y, angles.z)
{
}

inline Quat Quat::operator+(const Quat& q) const
{
    return Quat(v + q.v, w + q.w);
}

inline Quat Quat::operator-(const Quat& q) const
{
    return Quat(v - q.v, w - q.w);
}

inline Quat Quat::operator*(const Quat& q) const
{
    return Quat(
        v.z * q.v.y - v.y * q.v.z + v.x * q.w + w * q.v.x,
        v.x * q.v.z - v.z * q.v.x + v.y * q.w + w * q.v.y,
        v.y * q.v.x - v.x * q.v.y + v.z * q.w + w * q.v.z,
        w * q.w - v * q.v);
}

inline Quat Quat::operator/(const Quat& q) const
{
    return *this * q.Inverse();
}

inline Quat Quat::operator*(float s) const
{
    return Quat(v * s, w * s);
}

inline Quat Quat::operator/(float s) const
{
    return Quat(v / s, w / s);
}

inline Quat Quat::operator-() const
{
    return Quat(-v, -w);
}

inline const Quat& Quat::operator+=(const Quat& q)
{
    v += q.v;
    w += q.w;
    return *this;
}

inline const Quat& Quat::operator-=(const Quat& q)
{
    v -= q.v;
    w -= q.w;
    return *this;
}

inline const Quat& Quat::operator*=(const Quat& q)
{
    Quat c = *this;
    v.x = c.v.z * q.v.y - c.v.y * q.v.z + c.v.x * q.w + c.w * q.v.x;
    v.y = c.v.x * q.v.z - c.v.z * q.v.x + c.v.y * q.w + c.w * q.v.y;
    v.z = c.v.y * q.v.x - c.v.x * q.v.y + c.v.z * q.w + c.w * q.v.z;
    w = c.w * q.w - c.v * q.v;
    return *this;
}

inline const Quat& Quat::operator*=(float s)
{
    v *= s;
    w *= s;
    return *this;
}

inline const Quat& Quat::operator/=(float s)
{
    v /= s;
    w /= s;
    return *this;
}

inline float Quat::Dot(const Quat& q) const
{
    return v * q.v + w * q.w;
}

inline Mat3 Quat::ToMatrix() const
{
    float xx = v.x * v.x;
    float yy = v.y * v.y;
    float zz = v.z * v.z;
    float xy = v.x * v.y;
    float xz = v.x * v.z;
    float xw = v.x * w;
    float yz = v.y * v.z;
    float yw = v.y * w;
    float zw = v.z * w;

    return Mat3(
        1 - 2 * (zz + yy), 2 * (xy + zw), 2 * (xz - yw),
        2 * (xy - zw), 1 - 2 * (zz + xx), 2 * (yz + xw),
        2 * (yw + xz), 2 * (yz - xw), 1 - 2 * (yy + xx));
}

inline Quat Quat::FromMatrix(const Mat3& matrix)
{
    Quat q;

    float trace = matrix.m11 + matrix.m22 + matrix.m33;
    if (trace > 0.0f)
    {
        float s = sqrt(trace + 1.0f);
        q.w = s * 0.5f;
        s = 0.5f / s;
        q.v.x = (matrix.m23 - matrix.m32) * s;
        q.v.y = (matrix.m31 - matrix.m13) * s;
        q.v.z = (matrix.m12 - matrix.m21) * s;
    }
    else
    {
        if (matrix.m11 >= matrix.m22 && matrix.m11 >= matrix.m33)
        {
            float s = sqrt(1.0f + matrix.m11 - matrix.m22 - matrix.m33);
            float invS = 0.5f / s;
            q.v.x = 0.5f * s;
            q.v.y = (matrix.m12 + matrix.m21) * invS;
            q.v.z = (matrix.m13 + matrix.m31) * invS;
            q.w = (matrix.m23 - matrix.m32) * invS;
        }
        else if (matrix.m22 > matrix.m33)
        {
            float s = sqrt(1.0f + matrix.m22 - matrix.m11 - matrix.m33);
            float invS = 0.5f / s;
            q.v.x = (matrix.m21 + matrix.m12) * invS;
            q.v.y = 0.5f * s;
            q.v.z = (matrix.m32 + matrix.m23) * invS;
            q.w = (matrix.m31 - matrix.m13) * invS;
        }
        else
        {
            float s = sqrt(1.0f + matrix.m33 - matrix.m11 - matrix.m22);
            float invS = 0.5f / s;
            q.v.x = (matrix.m31 + matrix.m13) * invS;
            q.v.y = (matrix.m32 + matrix.m23) * invS;
            q.v.z = 0.5f * s;
            q.w = (matrix.m12 - matrix.m21) * invS;
        }
    }

    return q;
}

inline float Quat::Length() const
{
    return sqrt(v * v + w * w);
}

inline float Quat::LengthSq() const
{
    return (v * v + w * w);
}

inline void Quat::Normalize()
{
    *this /= Length();
}

inline Quat Quat::Normalized() const
{
    return *this / Length();
}

inline void Quat::Invert()
{
    v = -v;
    *this *= (1.0f / (v * v + w * w));
}

inline Quat Quat::Inverse() const
{
    float s = 1.0f / (v * v + w * w);
    return Quat(-v * s, w * s);
}

inline Quat Quat::Conjugate() const {
    return Quat(-v, w);
}

inline Quat Quat::AngleAxis(float angle, const Vec3& axis) {
    float ha = angle * 0.5f * math::DegToRad;
    return Quat(axis * sin(ha), cos(ha));
}

inline Quat Quat::FromTo(const Vec3& from, const Vec3& to)
{
    Vec3 v0 = from.Normalized();
    Vec3 v1 = to.Normalized();
    float dot = v0.Dot(v1);

    if (dot >= 1.0f - std::numeric_limits<float>::epsilon())
    {
        return Quat::Identity();
    }
    else if (dot <= -1.0f + std::numeric_limits<float>::epsilon())
    {
        Vec3 axis = Vec3(1, 0, 0).Cross(v0);

        if (axis.LengthSq() < std::numeric_limits<float>::epsilon() * 2)
            axis = Vec3(0, 1, 0).Cross(v0);

        axis.Normalize();
        return Quat(axis, 0);
    }

    float s = sqrt((1.0f + dot) * 2.0f);
    float invs = 1.0f / s;
    Vec3 c = v0.Cross(v1) * invs;
    return Quat(c, s * 0.5f).Normalized();
}

inline Quat Quat::LookRotation(const Vec3& forward, const Vec3& upward) {
    return Mat3::LookRotation(forward, upward).GetRotation();
}

inline Quat Quat::Lerp(const Quat& a, const Quat& b, float t)
{
    float ta = (1.0f - t);
    float tb = (a.Dot(b) < 0) ? -t : t;
    return (a * ta + b * tb).Normalized();
}

inline Quat Quat::Slerp(const Quat& a, const Quat& b, float t)
{
    float dot = a.Dot(b);

    bool flip = false;

    if (dot < 0)
    {
        flip = true;
        dot = -dot;
    }

    float ta, tb;
     
    if (dot > (1.0f - std::numeric_limits<float>::epsilon()))
    {
        ta = 1 - t;
        tb = flip ? -t : t;
    }
    else
    {
        float angle = acos(dot);
        float invSinAngle = 1.0f / sin(angle);
        ta = sin((1.0f - t) * angle) * invSinAngle;
        tb = flip ? -sin(t * angle) * invSinAngle : sin(t * angle) * invSinAngle;
    }

    return a * ta + b * tb;
}

inline void Quat::ToAngleAxis(float& angle, Vec3& axis) const
{
    float a = acos(w);
    axis = v * (1.0f / sin(a));
    angle = a * 2.0f * RadToDeg;
}

inline Vec3 Quat::ToEulerAngles() const
{
    Vec3 ret;
    ret.x = asin(math::Clamp(2 * (v.x * w - v.y * v.z), -1.0f, 1.0f)) * RadToDeg;
    ret.y = atan2(2 * (v.y * w + v.x * v.z), 1 - 2 * (v.x * v.x + v.y * v.y)) * RadToDeg;
    ret.z = atan2(2 * (v.x * v.y + v.z * w), 1 - 2 * (v.x * v.x + v.z * v.z)) * RadToDeg;
    return ret;
}

inline std::ostream& Quat::operator<<(std::ostream& os) {
    os << ToEulerAngles();
    return os;
}

}
}
