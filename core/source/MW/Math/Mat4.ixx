/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Mat4;
import Microwave.Math.Vec3;
import Microwave.Math.Mat3;
import Microwave.Math.Quat;
import Microwave.Math.Constants;
import <algorithm>;
import <cmath>;
import <limits>;
import <ostream>;

export namespace mw {
inline namespace math {

class Mat4
{
public:
    float m11, m12, m13, m14, //row 1
          m21, m22, m23, m24, //row 2
          m31, m32, m33, m34, //row 3
          m41, m42, m43, m44; //row 4


    static Mat4 Zero() {
        return Mat4(
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0);
    }

    static Mat4 Identity() {
        return Mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }

    Mat4()
        : Mat4(
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        )
    {
    }

    Mat4(float m11, float m12, float m13, float m14,
        float m21, float m22, float m23, float m24,
        float m31, float m32, float m33, float m34,
        float m41, float m42, float m43, float m44)
        : m11(m11), m12(m12), m13(m13), m14(m14)
        , m21(m21), m22(m22), m23(m23), m24(m24)
        , m31(m31), m32(m32), m33(m33), m34(m34)
        , m41(m41), m42(m42), m43(m43), m44(m44)
    {
    }

    Mat4(const Mat3& m)
        : m11(m.m11), m21(m.m21), m31(m.m31), m41(0)
        , m12(m.m12), m22(m.m22), m32(m.m32), m42(0)
        , m13(m.m13), m23(m.m23), m33(m.m33), m43(0)
        , m14(0), m24(0), m34(0), m44(1) {}

    Mat4 operator+(const Mat4& m) const
    {
        return Mat4(
            m11 + m.m11, m12 + m.m12, m13 + m.m13, m14 + m.m14,
            m21 + m.m21, m22 + m.m22, m23 + m.m23, m24 + m.m24,
            m31 + m.m31, m32 + m.m32, m33 + m.m33, m34 + m.m34,
            m41 + m.m41, m42 + m.m42, m43 + m.m43, m44 + m.m44);
    }

    Mat4 operator-(const Mat4& m) const
    {
        return Mat4(
            m11 - m.m11, m12 - m.m12, m13 - m.m13, m14 - m.m14,
            m21 - m.m21, m22 - m.m22, m23 - m.m23, m24 - m.m24,
            m31 - m.m31, m32 - m.m32, m33 - m.m33, m34 - m.m34,
            m41 - m.m41, m42 - m.m42, m43 - m.m43, m44 - m.m44);
    }

    Mat4 operator+(float s) const
    {
        return Mat4(
            m11 + s, m12 + s, m13 + s, m14 + s,
            m21 + s, m22 + s, m23 + s, m24 + s,
            m31 + s, m32 + s, m33 + s, m34 + s,
            m41 + s, m42 + s, m43 + s, m44 + s);
    }

    Mat4 operator-(float s) const
    {
        return Mat4(
            m11 - s, m12 - s, m13 - s, m14 - s,
            m21 - s, m22 - s, m23 - s, m24 - s,
            m31 - s, m32 - s, m33 - s, m34 - s,
            m41 - s, m42 - s, m43 - s, m44 - s);
    }

    Mat4 operator*(float s) const
    {
        return Mat4(
            m11 * s, m12 * s, m13 * s, m14 * s,
            m21 * s, m22 * s, m23 * s, m24 * s,
            m31 * s, m32 * s, m33 * s, m34 * s,
            m41 * s, m42 * s, m43 * s, m44 * s);
    }

    Mat4 operator/(float s) const
    {
        float ss = 1.0f / s;
        return Mat4(
            m11 * ss, m12 * ss, m13 * ss, m14 * ss,
            m21 * ss, m22 * ss, m23 * ss, m24 * ss,
            m31 * ss, m32 * ss, m33 * ss, m34 * ss,
            m41 * ss, m42 * ss, m43 * ss, m44 * ss);
    }

    Mat4 operator*(const Mat4& m) const
    {
        return Mat4(
            m11 * m.m11 + m12 * m.m21 + m13 * m.m31 + m14 * m.m41,
            m11 * m.m12 + m12 * m.m22 + m13 * m.m32 + m14 * m.m42,
            m11 * m.m13 + m12 * m.m23 + m13 * m.m33 + m14 * m.m43,
            m11 * m.m14 + m12 * m.m24 + m13 * m.m34 + m14 * m.m44,

            m21 * m.m11 + m22 * m.m21 + m23 * m.m31 + m24 * m.m41,
            m21 * m.m12 + m22 * m.m22 + m23 * m.m32 + m24 * m.m42,
            m21 * m.m13 + m22 * m.m23 + m23 * m.m33 + m24 * m.m43,
            m21 * m.m14 + m22 * m.m24 + m23 * m.m34 + m24 * m.m44,

            m31 * m.m11 + m32 * m.m21 + m33 * m.m31 + m34 * m.m41,
            m31 * m.m12 + m32 * m.m22 + m33 * m.m32 + m34 * m.m42,
            m31 * m.m13 + m32 * m.m23 + m33 * m.m33 + m34 * m.m43,
            m31 * m.m14 + m32 * m.m24 + m33 * m.m34 + m34 * m.m44,

            m41 * m.m11 + m42 * m.m21 + m43 * m.m31 + m44 * m.m41,
            m41 * m.m12 + m42 * m.m22 + m43 * m.m32 + m44 * m.m42,
            m41 * m.m13 + m42 * m.m23 + m43 * m.m33 + m44 * m.m43,
            m41 * m.m14 + m42 * m.m24 + m43 * m.m34 + m44 * m.m44);
    }

    Mat4& operator+=(const Mat4& m) {
        return (*this = *this + m);
    }

    Mat4& operator-=(const Mat4& m) {
        return (*this = *this - m);
    }

    Mat4& operator+=(float s) {
        return (*this = *this + s);
    }

    Mat4& operator-=(float s) {
        return (*this = *this - s);
    }

    Mat4& operator*=(float s) {
        return (*this = *this * s);
    }

    Mat4& operator/=(float s) {
        return (*this = *this / s);
    }

    Mat4& operator*=(const Mat4& m) {
        return (*this = *this * m);
    }

    bool operator==(const Mat4& other) const
    {
        return
            m11 == other.m11 && m12 == other.m12 && m13 == other.m13 && m14 == other.m14 &&
            m21 == other.m21 && m22 == other.m22 && m23 == other.m23 && m24 == other.m24 &&
            m31 == other.m31 && m32 == other.m32 && m33 == other.m33 && m34 == other.m34 &&
            m41 == other.m41 && m42 == other.m42 && m43 == other.m43 && m44 == other.m44;
    }

    bool operator!=(const Mat4& other) const
    {
        return
            m11 != other.m11 || m12 != other.m12 || m13 != other.m13 || m14 != other.m14 ||
            m21 != other.m21 || m22 != other.m22 || m23 != other.m23 || m24 != other.m24 ||
            m31 != other.m31 || m32 != other.m32 || m33 != other.m33 || m34 != other.m34 ||
            m41 != other.m41 || m42 != other.m42 || m43 != other.m43 || m44 != other.m44;
    }

    float operator[](int i) const {
        return ((float*)this)[i];
    }

    void Transpose() {
        std::swap(m12, m21);
        std::swap(m13, m31);
        std::swap(m14, m41);
        std::swap(m23, m32);
        std::swap(m24, m42);
        std::swap(m34, m43);
    }

    Mat4 Transposed() const {
        return Mat4(
            m11, m21, m31, m41,
            m12, m22, m32, m42,
            m13, m23, m33, m43,
            m14, m24, m34, m44);
    }

    Mat4 Invert() {
        return (*this = Inverse());
    }

    Mat4 Inverse() const
    {
        Mat4 adj;

        adj.m11 = m22 * m33 * m44 - m22 * m34 * m43 - m32 * m23 * m44 + m32 * m24 * m43 + m42 * m23 * m34 - m42 * m24 * m33;
        adj.m21 = -m21 * m33 * m44 + m21 * m34 * m43 + m31 * m23 * m44 - m31 * m24 * m43 - m41 * m23 * m34 + m41 * m24 * m33;
        adj.m31 = m21 * m32 * m44 - m21 * m34 * m42 - m31 * m22 * m44 + m31 * m24 * m42 + m41 * m22 * m34 - m41 * m24 * m32;
        adj.m41 = -m21 * m32 * m43 + m21 * m33 * m42 + m31 * m22 * m43 - m31 * m23 * m42 - m41 * m22 * m33 + m41 * m23 * m32;
        adj.m12 = -m12 * m33 * m44 + m12 * m34 * m43 + m32 * m13 * m44 - m32 * m14 * m43 - m42 * m13 * m34 + m42 * m14 * m33;
        adj.m22 = m11 * m33 * m44 - m11 * m34 * m43 - m31 * m13 * m44 + m31 * m14 * m43 + m41 * m13 * m34 - m41 * m14 * m33;
        adj.m32 = -m11 * m32 * m44 + m11 * m34 * m42 + m31 * m12 * m44 - m31 * m14 * m42 - m41 * m12 * m34 + m41 * m14 * m32;
        adj.m42 = m11 * m32 * m43 - m11 * m33 * m42 - m31 * m12 * m43 + m31 * m13 * m42 + m41 * m12 * m33 - m41 * m13 * m32;
        adj.m13 = m12 * m23 * m44 - m12 * m24 * m43 - m22 * m13 * m44 + m22 * m14 * m43 + m42 * m13 * m24 - m42 * m14 * m23;
        adj.m23 = -m11 * m23 * m44 + m11 * m24 * m43 + m21 * m13 * m44 - m21 * m14 * m43 - m41 * m13 * m24 + m41 * m14 * m23;
        adj.m33 = m11 * m22 * m44 - m11 * m24 * m42 - m21 * m12 * m44 + m21 * m14 * m42 + m41 * m12 * m24 - m41 * m14 * m22;
        adj.m43 = -m11 * m22 * m43 + m11 * m23 * m42 + m21 * m12 * m43 - m21 * m13 * m42 - m41 * m12 * m23 + m41 * m13 * m22;
        adj.m14 = -m12 * m23 * m34 + m12 * m24 * m33 + m22 * m13 * m34 - m22 * m14 * m33 - m32 * m13 * m24 + m32 * m14 * m23;
        adj.m24 = m11 * m23 * m34 - m11 * m24 * m33 - m21 * m13 * m34 + m21 * m14 * m33 + m31 * m13 * m24 - m31 * m14 * m23;
        adj.m34 = -m11 * m22 * m34 + m11 * m24 * m32 + m21 * m12 * m34 - m21 * m14 * m32 - m31 * m12 * m24 + m31 * m14 * m22;
        adj.m44 = m11 * m22 * m33 - m11 * m23 * m32 - m21 * m12 * m33 + m21 * m13 * m32 + m31 * m12 * m23 - m31 * m13 * m22;

        float det = m11 * adj.m11 + m12 * adj.m21 + m13 * adj.m31 + m14 * adj.m41;
        if (!(abs(det) > 0))
            return *this;

        return adj * (1.0f / det);
    }

    static Mat4 Scale(float x, float y, float z)
    {
        return Mat4(
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1);
    }

    static Mat4 Translation(float x, float y, float z)
    {
        return Mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1);
    }

    Vec3 GetTranslation() const {
        return Vec3(m41, m42, m43);
    }

    static Mat4 Rotation(float x, float y, float z)
    {
        float rx = x * math::DegToRad;
        float ry = y * math::DegToRad;
        float rz = z * math::DegToRad;

        float sx = sin(rx);
        float sy = sin(ry);
        float sz = sin(rz);
        float cx = cos(rx);
        float cy = cos(ry);
        float cz = cos(rz);

        return Mat4(
            (cy * cz + sx * sy * sz), (cx * sz), (sx * cy * sz - sy * cz), 0,
            (sx * sy * cz - cy * sz), (cx * cz), (sx * cy * cz + sy * sz), 0,
            (cx * sy), (-sx), (cx * cy), 0,
            0, 0, 0, 1);
    }

    static Mat4 Scale(const Vec3& scale) {
        return Scale(scale.x, scale.y, scale.z);
    }

    static Mat4 Translation(const Vec3& offset) {
        return Translation(offset.x, offset.y, offset.z);
    }

    static Mat4 Rotation(const Vec3& eulerAngles) {
        return Rotation(eulerAngles.x, eulerAngles.y, eulerAngles.z);
    }

    static Mat4 XRotation(float angle)
    {
        float ha = angle * math::DegToRad;
        float s = sin(ha);
        float c = cos(ha);

        return Mat4(1, 0, 0, 0,
            0, c, s, 0,
            0, -s, c, 0,
            0, 0, 0, 1);
    }

    static Mat4 YRotation(float angle)
    {
        float ha = angle * math::DegToRad;
        float s = sin(ha);
        float c = cos(ha);

        return Mat4(c, 0, -s, 0,
            0, 1, 0, 0,
            s, 0, c, 0,
            0, 0, 0, 1);
    }

    static Mat4 ZRotation(float angle)
    {
        float ha = angle * math::DegToRad;
        float s = sin(ha);
        float c = cos(ha);

        return Mat4(c, s, 0, 0,
            -s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);
    }

    static Mat4 LookRotation(const Vec3& forward, const Vec3& upward)
    {
        Vec3 zaxis = forward.Normalized();
        Vec3 xaxis = upward.Cross(zaxis).Normalized();
        Vec3 yaxis = zaxis.Cross(xaxis);

        return Mat4(xaxis.x, xaxis.y, xaxis.z, 0,
            yaxis.x, yaxis.y, yaxis.z, 0,
            zaxis.x, zaxis.y, zaxis.z, 0,
            0, 0, 0, 1);
    }

    static Mat4 Transform(const Vec3& pos, const Vec3& scale, const Quat& rot)
    {
        Mat4 mtx = rot.ToMatrix();

        mtx.m11 *= scale.x;
        mtx.m12 *= scale.x;
        mtx.m13 *= scale.x;

        mtx.m21 *= scale.y;
        mtx.m22 *= scale.y;
        mtx.m23 *= scale.y;

        mtx.m31 *= scale.z;
        mtx.m32 *= scale.z;
        mtx.m33 *= scale.z;

        mtx.m41 = pos.x;
        mtx.m42 = pos.y;
        mtx.m43 = pos.z;

        return mtx;
    }

    static Mat4 InverseTransform(const Vec3& pos, const Vec3& scale, const Quat& rotation)
    {
        Vec3 invPos = -pos;
        Quat invRot = rotation.Inverse();
        Vec3 invScale = Vec3::One().Divide(scale);

        Mat4 mtx = invRot.ToMatrix();

        mtx.m11 *= invScale.x;
        mtx.m12 *= invScale.x;
        mtx.m13 *= invScale.x;

        mtx.m21 *= invScale.y;
        mtx.m22 *= invScale.y;
        mtx.m23 *= invScale.y;

        mtx.m31 *= invScale.z;
        mtx.m32 *= invScale.z;
        mtx.m33 *= invScale.z;

        mtx.m41 = (invPos.x * mtx.m11) + (invPos.y * mtx.m21) + (invPos.z * mtx.m31);
        mtx.m42 = (invPos.x * mtx.m12) + (invPos.y * mtx.m22) + (invPos.z * mtx.m32);
        mtx.m43 = (invPos.x * mtx.m13) + (invPos.y * mtx.m23) + (invPos.z * mtx.m33);

        return mtx;
    }

    static Mat4 OrthoD3D(float left, float right, float bottom, float top, float znear, float zfar)
    {
        float w = right - left;
        float h = top - bottom;
        float d = zfar - znear;

        if (abs(w) < math::FloatTolerance ||
            abs(h) < math::FloatTolerance ||
            abs(d) < math::FloatTolerance)
        {
            return Identity();
        }

        float sx = 2.0f / w;
        float sy = 2.0f / h;
        float sz = 1.0f / d;

        float tx = -(right + left) / w;
        float ty = -(top + bottom) / h;
        float tz = -znear / d;

        return Mat4(
            sx, 0, 0, 0,
            0, sy, 0, 0,
            0, 0, sz, 0,
            tx, ty, tz, 1);
    }

    static Mat4 OrthoOpenGL(float left, float right, float bottom, float top, float znear, float zfar)
    {
        float w = right - left;
        float h = top - bottom;
        float d = zfar - znear;

        if (abs(w) < math::FloatTolerance ||
            abs(h) < math::FloatTolerance ||
            abs(d) < math::FloatTolerance)
        {
            return Identity();
        }

        float sx = 2.0f / w;
        float sy = 2.0f / h;
        float sz = 2.0f / d; // **

        float tx = -(right + left) / w;
        float ty = -(top + bottom) / h;
        float tz = -(zfar + znear) / d; // **

        return Mat4(
            sx, 0, 0, 0,
            0, sy, 0, 0,
            0, 0, sz, 0,
            tx, ty, tz, 1);
    }

    static Mat4 PerspectiveD3D(float fovY, float aspect, float znear, float zfar)
    {
        float f = 1.0f / tan(math::DegToRad * fovY / 2.0f);
        float sx = f / aspect;
        float sy = f;
        float sz = zfar / (zfar - znear);
        float tz = -(zfar * znear) / (zfar - znear);
        float w = 1;

        return Mat4(
            sx, 0, 0, 0,
            0, sy, 0, 0,
            0, 0, sz, w,
            0, 0, tz, 0);
    }

    static Mat4 PerspectiveOpenGL(float fovY, float aspect, float znear, float zfar)
    {
        float f = 1.0f / tan(math::DegToRad * fovY / 2.0f);
        float sx = f / aspect;
        float sy = f;
        float sz = (zfar + znear) / (zfar - znear);
        float tz = -(2.0f * zfar * znear) / (zfar - znear);
        float w = 1;

        return Mat4(
            sx, 0, 0, 0,
            0, sy, 0, 0,
            0, 0, sz, w,
            0, 0, tz, 0);
    }

    std::ostream& operator<<(std::ostream& os) const {
        os << "[" << m11 << ", " << m12 << ", " << m13 << ", " << m14 << "]" << std::endl;
        os << "[" << m21 << ", " << m22 << ", " << m23 << ", " << m24 << "]" << std::endl;
        os << "[" << m31 << ", " << m32 << ", " << m33 << ", " << m34 << "]" << std::endl;
        os << "[" << m41 << ", " << m42 << ", " << m43 << ", " << m44 << "]";
        return os;
    }
};

} // math
} // mw
