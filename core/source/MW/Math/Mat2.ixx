/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Mat2;
import <ostream>;
import <algorithm>;
import <cmath>;
import <limits>;

export namespace mw {
inline namespace math {

class Mat2
{
public:
    float m11, m12, // row 1
          m21, m22; // row 2

    static Mat2 Zero() {
        return Mat2(
            0, 0,
            0, 0);
    }

    static Mat2 Identity() {
        return Mat2(
            1, 0,
            0, 1);
    }

    Mat2()
        : Mat2(0, 0, 0, 0) {}

    Mat2(
        float m11, float m12,
        float m21, float m22)
        : m11(m11), m12(m12),
        m21(m21), m22(m22) {}


    Mat2 operator+(const Mat2& m) const
    {
        return Mat2(
            m11 + m.m11, m12 + m.m12,
            m21 + m.m21, m22 + m.m22);
    }

    Mat2 operator-(const Mat2& m) const
    {
        return Mat2(
            m11 - m.m11, m12 - m.m12,
            m21 - m.m21, m22 - m.m22);
    }

    Mat2 operator*(const Mat2& m) const
    {
        return Mat2(
            m11 * m.m11 + m12 * m.m21,
            m11 * m.m12 + m12 * m.m22,

            m21 * m.m11 + m22 * m.m21,
            m21 * m.m12 + m22 * m.m22);
    }

    Mat2 operator+(float s) const
    {
        return Mat2(
            m11 + s, m12 + s,
            m21 + s, m22 + s);
    }

    Mat2 operator-(float s) const
    {
        return Mat2(
            m11 - s, m12 - s,
            m21 - s, m22 - s);
    }

    Mat2 operator*(float s) const
    {
        return Mat2(
            m11 * s, m12 * s,
            m21 * s, m22 * s);
    }

    Mat2 operator/(float s) const
    {
        float ss = 1.0f / s;
        return Mat2(
            m11 * ss, m12 * ss,
            m21 * ss, m22 * ss);
    }

    Mat2& operator+=(const Mat2& m) {
        return (*this = *this + m);
    }

    Mat2& operator-=(const Mat2& m) {
        return (*this = *this - m);
    }

    Mat2& operator*=(const Mat2& m) {
        return (*this = *this * m);
    }

    Mat2& operator+=(float s) {
        return (*this = *this + s);
    }

    Mat2& operator-=(float s) {
        return (*this = *this - s);
    }

    Mat2& operator*=(float s) {
        return (*this = *this * s);
    }

    Mat2& operator/=(float s) {
        return (*this = *this / s);
    }

    bool operator==(const Mat2& other) const
    {
        return
            m11 == other.m11 && m12 == other.m12 &&
            m21 == other.m21 && m22 == other.m22;
    }

    bool operator!=(const Mat2& other) const
    {
        return
            m11 != other.m11 || m12 != other.m12 ||
            m21 != other.m21 || m22 != other.m22;
    }

    void Transpose() {
        std::swap(m12, m21);
    }

    Mat2 Transposed() const {
        return Mat2(
            m11, m21,
            m12, m22);
    }

    float Determinant() const {
        return m11 * m22 - m12 * m21;
    }

    Mat2 Invert() {
        return (*this = Inverse());
    }

    Mat2 Inverse() const
    {
        Mat2 adj(
            m22, -m12,
            -m21, m11);

        float det = Determinant();
        if (abs(det) < std::numeric_limits<float>::epsilon())
            return *this;

        return adj * (1.0f / det);
    }

    std::ostream& operator<<(std::ostream& os) const {
        os << "[" << m11 << ", " << m12 << "]" << std::endl;
        os << "[" << m21 << ", " << m22 << "]";
        return os;
    }
};

} // math
} // mw
