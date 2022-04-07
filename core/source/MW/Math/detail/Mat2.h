/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <ostream>

namespace mw {
inline namespace math {

class Mat2
{
public:
    float m11, m12, // row 1
          m21, m22; // row 2

    static Mat2 Zero();
    static Mat2 Identity();

    Mat2();
    Mat2(float m11, float m12,
         float m21, float m22);

    Mat2 operator+(const Mat2& m) const;
    Mat2 operator-(const Mat2& m) const;
    Mat2 operator*(const Mat2& m) const;
    Mat2 operator+(float s) const;
    Mat2 operator-(float s) const;
    Mat2 operator*(float s) const;
    Mat2 operator/(float s) const;

    Mat2& operator+=(const Mat2& m);
    Mat2& operator-=(const Mat2& m);
    Mat2& operator*=(const Mat2& m);
    Mat2& operator+=(float s);
    Mat2& operator-=(float s);
    Mat2& operator*=(float s);
    Mat2& operator/=(float s);

    bool operator==(const Mat2& other) const;
    bool operator!=(const Mat2& other) const;

    void Transpose();
    Mat2 Transposed() const;

    float Determinant() const;

    Mat2 Invert();
    Mat2 Inverse() const;

    std::ostream& operator<<(std::ostream& os);
};

}
}
