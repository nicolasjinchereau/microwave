/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Mat3_p1;
import <ostream>;

export namespace mw {
inline namespace math {

class Vec3;
class Quat;

class Mat3
{
public:
    float m11, m12, m13, // row 1
          m21, m22, m23, // row 2
          m31, m32, m33; // row 3

    static Mat3 Zero();
    static Mat3 Identity();

    Mat3();
    Mat3(float m11, float m12, float m13,
         float m21, float m22, float m23,
         float m31, float m32, float m33);

    Mat3 operator+(const Mat3& m) const;
    Mat3 operator-(const Mat3& m) const;
    Mat3 operator+(float s) const;
    Mat3 operator-(float s) const;
    Mat3 operator*(float s) const;
    Mat3 operator/(float s) const;
    Mat3 operator*(const Mat3& m) const;
    Mat3& operator+=(const Mat3& m);
    Mat3& operator-=(const Mat3& m);
    Mat3& operator+=(float s);
    Mat3& operator-=(float s);
    Mat3& operator*=(float s);
    Mat3& operator/=(float s);
    Mat3& operator*=(const Mat3& m);
    bool operator==(const Mat3& other) const;
    bool operator!=(const Mat3& other) const;

    void Transpose();
    Mat3 Transposed() const;
    Mat3 Invert();
    Mat3 Inverse() const;
    Quat GetRotation() const;

    static Mat3 Scale(float x, float y, float z);
    static Mat3 Translation(float x, float y);
    static Mat3 Ortho2D(float left, float right, float bottom, float top);
    static Mat3 LookRotation(const Vec3& forward, const Vec3& upward);

    std::ostream& operator<<(std::ostream& os) const;
};

}
}
