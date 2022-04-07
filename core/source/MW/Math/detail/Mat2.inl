/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/Mat2.h>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <limits>

namespace mw {
inline namespace math {

inline Mat2 Mat2::Zero() {
    return Mat2(
        0, 0,
        0, 0);
}

inline Mat2 Mat2::Identity() {
    return Mat2(
        1, 0,
        0, 1);
}

inline Mat2::Mat2()
    : Mat2(0, 0, 0, 0) {}

inline Mat2::Mat2(
    float m11, float m12,
    float m21, float m22)
    : m11(m11), m12(m12),
      m21(m21), m22(m22) {}


inline Mat2 Mat2::operator+(const Mat2& m) const
{
    return Mat2(
        m11 + m.m11, m12 + m.m12,
        m21 + m.m21, m22 + m.m22);
}

inline Mat2 Mat2::operator-(const Mat2& m) const
{
    return Mat2(
        m11 - m.m11, m12 - m.m12,
        m21 - m.m21, m22 - m.m22);
}

inline Mat2 Mat2::operator*(const Mat2& m) const
{
    return Mat2(
        m11 * m.m11 + m12 * m.m21,
        m11 * m.m12 + m12 * m.m22,

        m21 * m.m11 + m22 * m.m21,
        m21 * m.m12 + m22 * m.m22);
}

inline Mat2 Mat2::operator+(float s) const
{
    return Mat2(
        m11 + s, m12 + s,
        m21 + s, m22 + s);
}

inline Mat2 Mat2::operator-(float s) const
{
    return Mat2(
        m11 - s, m12 - s,
        m21 - s, m22 - s);
}

inline Mat2 Mat2::operator*(float s) const
{
    return Mat2(
        m11 * s, m12 * s,
        m21 * s, m22 * s);
}

inline Mat2 Mat2::operator/(float s) const
{
    float ss = 1.0f / s;
    return Mat2(
        m11 * ss, m12 * ss,
        m21 * ss, m22 * ss);
}

inline Mat2& Mat2::operator+=(const Mat2& m) {
    return (*this = *this + m);
}

inline Mat2& Mat2::operator-=(const Mat2& m) {
    return (*this = *this - m);
}

inline Mat2& Mat2::operator*=(const Mat2& m) {
    return (*this = *this * m);
}

inline Mat2& Mat2::operator+=(float s) {
    return (*this = *this + s);
}

inline Mat2& Mat2::operator-=(float s) {
    return (*this = *this - s);
}

inline Mat2& Mat2::operator*=(float s) {
    return (*this = *this * s);
}

inline Mat2& Mat2::operator/=(float s) {
    return (*this = *this / s);
}

inline bool Mat2::operator==(const Mat2& other) const
{
    return
        m11 == other.m11 && m12 == other.m12 &&
        m21 == other.m21 && m22 == other.m22;
}

inline bool Mat2::operator!=(const Mat2& other) const
{
    return
        m11 != other.m11 || m12 != other.m12 ||
        m21 != other.m21 || m22 != other.m22;
}

inline void Mat2::Transpose() {
    std::swap(m12, m21);
}

inline Mat2 Mat2::Transposed() const {
    return Mat2(
        m11, m21,
        m12, m22);
}

inline float Mat2::Determinant() const {
    return m11 * m22 - m12 * m21;
}

inline Mat2 Mat2::Invert() {
    return (*this = Inverse());
}

inline Mat2 Mat2::Inverse() const
{
    Mat2 adj(m22, -m12,
        -m21, m11);

    float det = Determinant();
    if (abs(det) < std::numeric_limits<float>::epsilon())
        return *this;

    return adj * (1.0f / det);
}

inline std::ostream& Mat2::operator<<(std::ostream& os) {
    os << "[" << m11 << ", " << m12 << "]" << std::endl;
    os << "[" << m21 << ", " << m22 << "]";
    return os;
}

}
}
