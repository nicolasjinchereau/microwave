/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <algorithm>
#include <ostream>
#include <sstream>

namespace mw {
inline namespace math {

class Vec3;
class Quat;
class Mat3;

class Mat4
{
public:
    static Mat4 Zero();
    static Mat4 Identity();

    float m11, m12, m13, m14, //row 1
          m21, m22, m23, m24, //row 2
          m31, m32, m33, m34, //row 3
          m41, m42, m43, m44; //row 4

    Mat4();
    Mat4(float m11, float m12, float m13, float m14,
         float m21, float m22, float m23, float m24,
         float m31, float m32, float m33, float m34,
         float m41, float m42, float m43, float m44);
    Mat4(const Mat3& m);

    Mat4 operator+(const Mat4& m) const;
    Mat4 operator-(const Mat4& m) const;
    Mat4 operator+(float s) const;
    Mat4 operator-(float s) const;
    Mat4 operator*(float s) const;
    Mat4 operator/(float s) const;
    Mat4 operator*(const Mat4& m) const;
    Mat4& operator+=(const Mat4& m);
    Mat4& operator-=(const Mat4& m);
    Mat4& operator+=(float s);
    Mat4& operator-=(float s);
    Mat4& operator*=(float s);
    Mat4& operator/=(float s);
    Mat4& operator*=(const Mat4& m);
    bool operator==(const Mat4& m) const;
    bool operator!=(const Mat4& m) const;
    float operator[](int i) const;

    void Transpose();
    Mat4 Transposed() const;
    Mat4 Invert();
    Mat4 Inverse() const;
    //Quat GetRotation() const;
    Vec3 GetTranslation() const;

    static Mat4 Scale(float x, float y, float z);
    static Mat4 Translation(float x, float y, float z);
    static Mat4 Rotation(float x, float y, float z);
    static Mat4 Scale(const Vec3& scale);
    static Mat4 Translation(const Vec3& offset);
    static Mat4 Rotation(const Vec3& eulerAngles);
    static Mat4 XRotation(float angle);
    static Mat4 YRotation(float angle);
    static Mat4 ZRotation(float angle);
    static Mat4 LookRotation(const Vec3& forward, const Vec3& upward);
    static Mat4 Transform(const Vec3& pos, const Vec3& scale, const Quat& rot);
    static Mat4 InverseTransform(const Vec3& pos, const Vec3& scale, const Quat& rot);
    static Mat4 OrthoD3D(float left, float right, float bottom, float top, float znear, float zfar);
    static Mat4 OrthoOpenGL(float left, float right, float bottom, float top, float znear, float zfar);
    static Mat4 PerspectiveD3D(float fovY, float aspect, float znear, float zfar);
    static Mat4 PerspectiveOpenGL(float fovY, float aspect, float znear, float zfar);

    std::ostream& operator<<(std::ostream& os) const;
};

}
}
