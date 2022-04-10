/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Mat3_p2;
import :Mat3_p1;
import :Vec3_p1;
import :Vec3_p2;
import :Quat_p1;
import <algorithm>;
import <ostream>;
import <cmath>;
import <limits>;

export namespace mw {
inline namespace math {

Mat3::Mat3(
    float m11, float m12, float m13,
    float m21, float m22, float m23,
    float m31, float m32, float m33)
    : m11(m11), m12(m12), m13(m13),
      m21(m21), m22(m22), m23(m23),
      m31(m31), m32(m32), m33(m33) {}

Mat3::Mat3()
    : Mat3(0, 0, 0, 0, 0, 0, 0, 0, 0) {}

Mat3 Mat3::Zero() {
    return Mat3(
        0, 0, 0,
        0, 0, 0,
        0, 0, 0);
}

Mat3 Mat3::Identity() {
    return Mat3(
        1, 0, 0,
        0, 1, 0,
        0, 0, 1);
}

Mat3 Mat3::operator+(const Mat3& m) const
{
    return Mat3(
        m11 + m.m11, m12 + m.m12, m13 + m.m13,
        m21 + m.m21, m22 + m.m22, m23 + m.m23,
        m31 + m.m31, m32 + m.m32, m33 + m.m33);
}

Mat3 Mat3::operator-(const Mat3& m) const
{
    return Mat3(
        m11 - m.m11, m12 - m.m12, m13 - m.m13,
        m21 - m.m21, m22 - m.m22, m23 - m.m23,
        m31 - m.m31, m32 - m.m32, m33 - m.m33);
}

Mat3 Mat3::operator+(float s) const
{
    return Mat3(
        m11 + s, m12 + s, m13 + s,
        m21 + s, m22 + s, m23 + s,
        m31 + s, m32 + s, m33 + s);
}

Mat3 Mat3::operator-(float s) const
{
    return Mat3(
        m11 - s, m12 - s, m13 - s,
        m21 - s, m22 - s, m23 - s,
        m31 - s, m32 - s, m33 - s);
}

Mat3 Mat3::operator*(float s) const
{
    return Mat3(
        m11 * s, m12 * s, m13 * s,
        m21 * s, m22 * s, m23 * s,
        m31 * s, m32 * s, m33 * s);
}

Mat3 Mat3::operator/(float s) const
{
    float ss = 1.0f / s;
    return Mat3(
        m11 * ss, m12 * ss, m13 * ss,
        m21 * ss, m22 * ss, m23 * ss,
        m31 * ss, m32 * ss, m33 * ss);
}

Mat3 Mat3::operator*(const Mat3& m) const
{
    return Mat3(
        m11 * m.m11 + m12 * m.m21 + m13 * m.m31,
        m11 * m.m12 + m12 * m.m22 + m13 * m.m32,
        m11 * m.m13 + m12 * m.m23 + m13 * m.m33,

        m21 * m.m11 + m22 * m.m21 + m23 * m.m31,
        m21 * m.m12 + m22 * m.m22 + m23 * m.m32,
        m21 * m.m13 + m22 * m.m23 + m23 * m.m33,

        m31 * m.m11 + m32 * m.m21 + m33 * m.m31,
        m31 * m.m12 + m32 * m.m22 + m33 * m.m32,
        m31 * m.m13 + m32 * m.m23 + m33 * m.m33);
}

Mat3& Mat3::operator+=(const Mat3& m) {
    return (*this = *this + m);
}

Mat3& Mat3::operator-=(const Mat3& m) {
    return (*this = *this - m);
}

Mat3& Mat3::operator+=(float s) {
    return (*this = *this + s);
}

Mat3& Mat3::operator-=(float s) {
    return (*this = *this - s);
}

Mat3& Mat3::operator*=(float s) {
    return (*this = *this * s);
}

Mat3& Mat3::operator/=(float s) {
    return (*this = *this / s);
}

Mat3& Mat3::operator*=(const Mat3& m) {
    return (*this = *this * m);
}

bool Mat3::operator==(const Mat3& other) const
{
    return
        m11 == other.m11 && m12 == other.m12 && m13 == other.m13 &&
        m21 == other.m21 && m22 == other.m22 && m23 == other.m23 &&
        m31 == other.m31 && m32 == other.m32 && m33 == other.m33;
}

bool Mat3::operator!=(const Mat3& other) const
{
    return
        m11 != other.m11 || m12 != other.m12 || m13 != other.m13 ||
        m21 != other.m21 || m22 != other.m22 || m23 != other.m23 ||
        m31 != other.m31 || m32 != other.m32 || m33 != other.m33;
}

void Mat3::Transpose() {
    std::swap(m12, m21);
    std::swap(m13, m31);
    std::swap(m23, m32);
}

Mat3 Mat3::Transposed() const {
    return Mat3(
        m11, m21, m31,
        m12, m22, m32,
        m13, m23, m33);
}

Mat3 Mat3::Invert() {
    return (*this = Inverse());
}

Mat3 Mat3::Inverse() const
{
    Mat3 adj(
        (m22 * m33 - m23 * m32), -(m12 * m33 - m13 * m32), (m12 * m23 - m13 * m22),
        -(m21 * m33 - m23 * m31), (m11 * m33 - m13 * m31), -(m11 * m23 - m13 * m21),
        (m21 * m32 - m22 * m31), -(m11 * m32 - m12 * m31), (m11 * m22 - m12 * m21)
    );

    float det = m11 * adj.m11 + m12 * adj.m21 + m13 * adj.m31;
    if (abs(det) < std::numeric_limits<float>::epsilon())
        return *this;

    return adj * (1.0f / det);
}

Quat Mat3::GetRotation() const
{
    float trace = 1.0f + m11 + m22 + m33;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    if (trace > 0.000001f)
    {
        float s = sqrt(trace) * 2.0f;
        float inv_s = 1.0f / s;

        x = (m23 - m32) * inv_s;
        y = (m31 - m13) * inv_s;
        z = (m12 - m21) * inv_s;
        w = 0.25f * s;
    }
    else
    {
        if (m11 > m22 && m11 > m33)
        {
            // Column 0:
            float s = sqrt(1.0f + m11 - m22 - m33) * 2.0f;
            float inv_s = 1.0f / s;

            x = 0.25f * s;
            y = (m12 + m21) * inv_s;
            z = (m31 + m13) * inv_s;
            w = (m23 - m32) * inv_s;
        }
        else if (m22 > m33)
        {
            // Column 1:
            float s = sqrt(1.0f + m22 - m11 - m33) * 2.0f;
            float inv_s = 1.0f / s;

            x = (m12 + m21) * inv_s;
            y = 0.25f * s;
            z = (m23 + m32) * inv_s;
            w = (m31 - m13) * inv_s;
        }
        else
        {
            // Column 2:
            float s = sqrt(1.0f + m33 - m11 - m22) * 2.0f;
            float inv_s = 1.0f / s;

            x = (m31 + m13) * inv_s;
            y = (m23 + m32) * inv_s;
            z = 0.25f * s;
            w = (m12 - m21) * inv_s;
        }
    }

    return Quat(x, y, z, w).Normalized();
}

Mat3 Mat3::Scale(float x, float y, float z)
{
    return Mat3(
        x, 0, 0,
        0, y, 0,
        0, 0, z);
}

Mat3 Mat3::Translation(float x, float y)
{
    return Mat3(
        1, 0, 0,
        0, 1, 0,
        x, y, 1);
}

Mat3 Mat3::Ortho2D(float left, float right, float bottom, float top)
{
    float w = right - left;
    float h = top - bottom;

    if (abs(w) < 1e-6 || abs(h) < 1e-6)
        return Mat3::Identity();

    float sx = 2.0f / w;
    float sy = 2.0f / h;
    float tx = -(left + right) / w;
    float ty = -(top + bottom) / h;

    return Mat3(sx, 0, 0,
        0, sy, 0,
        tx, ty, 1);
}

Mat3 Mat3::LookRotation(const Vec3& forward, const Vec3& upward)
{
    Vec3 zaxis = forward.Normalized();
    Vec3 xaxis = upward.Cross(zaxis).Normalized();
    Vec3 yaxis = zaxis.Cross(xaxis);

    return Mat3(xaxis.x, xaxis.y, xaxis.z,
        yaxis.x, yaxis.y, yaxis.z,
        zaxis.x, zaxis.y, zaxis.z);
}

std::ostream& Mat3::operator<<(std::ostream& os) const {
    os << "[" << m11 << ", " << m12 << ", " << m13 << "]" << std::endl;
    os << "[" << m21 << ", " << m22 << ", " << m23 << "]" << std::endl;
    os << "[" << m31 << ", " << m32 << ", " << m33 << "]";
    return os;
}

}
}
