/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <ostream>
#include <sstream>

namespace mw {
inline namespace math {

class IVec2
{
public:
    int x;
    int y;

    static IVec2 Zero();
    static IVec2 One();
    static IVec2 Right();
    static IVec2 Left();
    static IVec2 Up();
    static IVec2 Down();

    IVec2();
    IVec2(int x, int y);

    int& operator[](size_t index);
    int operator[](size_t index) const;
    IVec2 operator-() const;
    IVec2 operator+(const IVec2& v) const;
    IVec2 operator-(const IVec2& v) const;
    IVec2 operator*(int s) const;
    IVec2 operator/(int s) const;
    IVec2& operator+=(const IVec2& v);
    IVec2& operator-=(const IVec2& v);
    IVec2& operator*=(int s);
    IVec2& operator/=(int s);
    bool operator==(const IVec2& v) const;
    bool operator!=(const IVec2& v) const;
    IVec2 PerpCW() const;
    IVec2 PerpCCW() const;
    std::ostream& operator<<(std::ostream& os);
};

}
}
