/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Quat;
import Microwave.Math.Vec3;
import Microwave.Math.Mat3;
import Microwave.Math.Constants;
import <algorithm>;
import <cmath>;
import <limits>;
import <ostream>;

export namespace mw {
inline namespace math {

class Quat
{
public:
    Vec3 v;
    float w;

    static Quat Zero() {
        return Quat(0, 0, 0, 0);
    }

    static Quat Identity() {
        return Quat(0, 0, 0, 1);
    }

    Quat()
        : Quat(0, 0, 0, 1) {}

    Quat(float xAngle, float yAngle, float zAngle)
    {
        float xRadHalf = xAngle * DegToRad * 0.5f;
        float yRadHalf = yAngle * DegToRad * 0.5f;
        float zRadHalf = zAngle * DegToRad * 0.5f;

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

    Quat(float x, float y, float z, float w)
        : v(x, y, z), w(w)
    {
    }

    Quat(const Vec3& v, float w)
        : v(v), w(w)
    {
    }

    Quat(const Vec3& angles)
        : Quat(angles.x, angles.y, angles.z)
    {
    }

    Quat operator+(const Quat& q) const
    {
        return Quat(v + q.v, w + q.w);
    }

    Quat operator-(const Quat& q) const
    {
        return Quat(v - q.v, w - q.w);
    }

    Quat operator*(const Quat& q) const
    {
        return Quat(
            v.z * q.v.y - v.y * q.v.z + v.x * q.w + w * q.v.x,
            v.x * q.v.z - v.z * q.v.x + v.y * q.w + w * q.v.y,
            v.y * q.v.x - v.x * q.v.y + v.z * q.w + w * q.v.z,
            w * q.w - v * q.v);
    }

    Quat operator/(const Quat& q) const
    {
        return *this * q.Inverse();
    }

    Quat operator*(float s) const
    {
        return Quat(v * s, w * s);
    }

    Quat operator/(float s) const
    {
        return Quat(v / s, w / s);
    }

    Quat operator-() const
    {
        return Quat(-v, -w);
    }

    const Quat& operator+=(const Quat& q)
    {
        v += q.v;
        w += q.w;
        return *this;
    }

    const Quat& operator-=(const Quat& q)
    {
        v -= q.v;
        w -= q.w;
        return *this;
    }

    const Quat& operator*=(const Quat& q)
    {
        Quat c = *this;
        v.x = c.v.z * q.v.y - c.v.y * q.v.z + c.v.x * q.w + c.w * q.v.x;
        v.y = c.v.x * q.v.z - c.v.z * q.v.x + c.v.y * q.w + c.w * q.v.y;
        v.z = c.v.y * q.v.x - c.v.x * q.v.y + c.v.z * q.w + c.w * q.v.z;
        w = c.w * q.w - c.v * q.v;
        return *this;
    }

    const Quat& operator*=(float s)
    {
        v *= s;
        w *= s;
        return *this;
    }

    const Quat& operator/=(float s)
    {
        v /= s;
        w /= s;
        return *this;
    }

    float Dot(const Quat& q) const
    {
        return v * q.v + w * q.w;
    }

    Mat3 ToMatrix() const
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

    float Length() const
    {
        return sqrt(v * v + w * w);
    }

    float LengthSq() const
    {
        return (v * v + w * w);
    }

    void Normalize()
    {
        *this /= Length();
    }

    Quat Normalized() const
    {
        return *this / Length();
    }

    void Invert()
    {
        v = -v;
        *this *= (1.0f / (v * v + w * w));
    }

    Quat Inverse() const
    {
        float s = 1.0f / (v * v + w * w);
        return Quat(-v * s, w * s);
    }

    Quat Conjugate() const {
        return Quat(-v, w);
    }

    static Quat FromMatrix(const Mat3& matrix)
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

    static Quat AngleAxis(float angle, const Vec3& axis) {
        float ha = angle * 0.5f * DegToRad;
        return Quat(axis * sin(ha), cos(ha));
    }

    static Quat FromTo(const Vec3& from, const Vec3& to)
    {
        Vec3 v0 = from.Normalized();
        Vec3 v1 = to.Normalized();
        float dot = v0.Dot(v1);

        if (dot >= 1.0f - std::numeric_limits<float>::epsilon())
        {
            return Identity();
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

    static Quat LookRotation(const Vec3& forward, const Vec3& upward) {
        return FromMatrix(Mat3::LookRotation(forward, upward));
    }

    static Quat Lerp(const Quat& a, const Quat& b, float t)
    {
        float ta = (1.0f - t);
        float tb = (a.Dot(b) < 0) ? -t : t;
        return (a * ta + b * tb).Normalized();
    }

    static Quat Slerp(const Quat& a, const Quat& b, float t)
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

    void ToAngleAxis(float& angle, Vec3& axis) const
    {
        float a = acos(w);
        axis = v * (1.0f / sin(a));
        angle = a * 2.0f * RadToDeg;
    }

    Vec3 ToEulerAngles() const
    {
        Vec3 ret;
        ret.x = asin(Clampf(2 * (v.x * w - v.y * v.z), -1.0f, 1.0f)) * RadToDeg;
        ret.y = atan2(2 * (v.y * w + v.x * v.z), 1 - 2 * (v.x * v.x + v.y * v.y)) * RadToDeg;
        ret.z = atan2(2 * (v.x * v.y + v.z * w), 1 - 2 * (v.x * v.x + v.z * v.z)) * RadToDeg;
        return ret;
    }

    std::ostream& operator<<(std::ostream& os) const {
        auto a = ToEulerAngles();
        os << "(" << a.x << ", " << a.y << ", " << a.z << ")";
        return os;
    }

private:
    static float Clampf(float x, float lower, float upper) {
        return x < lower ? lower : (x > upper ? upper : x);
    }
};

} // math
} // mw
