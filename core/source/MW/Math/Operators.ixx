/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Operators;
import Microwave.Math.Vec3;
import Microwave.Math.Vec4;
import Microwave.Math.Quat;
import Microwave.Math.Mat3;
import Microwave.Math.Mat4;

export namespace mw {
inline namespace math {

Vec3 operator*(const Vec3& v, const Mat3& m)
{
    return Vec3(
        v.x * m.m11 + v.y * m.m21 + v.z * m.m31,
        v.x * m.m12 + v.y * m.m22 + v.z * m.m32,
        v.x * m.m13 + v.y * m.m23 + v.z * m.m33);
}

Vec3& operator*=(Vec3& v, const Mat3& m) {
    return (v = v * m);
}

Vec4 operator*(const Vec4& v, const Mat4& m)
{
    return Vec4(
        m.m11 * v.x + m.m21 * v.y + m.m31 * v.z + m.m41 * v.w,
        m.m12 * v.x + m.m22 * v.y + m.m32 * v.z + m.m42 * v.w,
        m.m13 * v.x + m.m23 * v.y + m.m33 * v.z + m.m43 * v.w,
        m.m14 * v.x + m.m24 * v.y + m.m34 * v.z + m.m44 * v.w);
}

Vec4& operator*=(Vec4& v, const Mat4& m) {
    return (v = v * m);
}

Vec3 operator*(const Vec3& v, const Quat& q)
{
    Vec3 uv = q.v.Cross(v);
    Vec3 uuv = q.v.Cross(uv);
    uv = uv * (2.0f * q.w);
    uuv = uuv * 2.0f;
    return v + uv + uuv;
}

Vec3& operator*=(Vec3& v, const Quat& q) {
    return (v = v * q);
}

} // math
} // mw
