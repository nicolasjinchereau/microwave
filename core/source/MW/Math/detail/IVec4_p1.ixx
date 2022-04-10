/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:IVec4_p1;
import <ostream>;
import <cstdint>;

export namespace mw {
inline namespace math {

class IVec4
{
public:
    int x;
    int y;
    int z;
    int w;

    static IVec4 Zero();
    static IVec4 One();
    static IVec4 Right();
    static IVec4 Left();
    static IVec4 Up();
    static IVec4 Down();
    static IVec4 Forward();
    static IVec4 Backward();

    IVec4();
    IVec4(int x, int y, int z, int w);

    int& operator[](size_t index);
    int operator[](size_t index) const;
    IVec4 operator-() const;
    IVec4 operator+(const IVec4& v) const;
    IVec4 operator-(const IVec4& v) const;
    IVec4 operator*(int s) const;
    IVec4 operator/(int s) const;
    IVec4& operator+=(const IVec4& v);
    IVec4& operator-=(const IVec4& v);
    IVec4& operator*=(int s);
    IVec4& operator/=(int s);
    bool operator==(const IVec4& v) const;
    bool operator!=(const IVec4& v) const;
    std::ostream& operator<<(std::ostream& os) const;
};

}
}
