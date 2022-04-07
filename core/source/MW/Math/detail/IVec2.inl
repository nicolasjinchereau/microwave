/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/IVec2.h>
#include <cassert>

namespace mw {
inline namespace math {

inline IVec2 IVec2::Zero() { return IVec2(0, 0); }
inline IVec2 IVec2::One() { return IVec2(1, 1); }
inline IVec2 IVec2::Right() { return IVec2(1, 0); }
inline IVec2 IVec2::Left() { return IVec2(-1, 0); }
inline IVec2 IVec2::Up() { return IVec2(0, 1); }
inline IVec2 IVec2::Down() { return IVec2(0, -1); }

inline IVec2::IVec2() : IVec2(0, 0) { }
inline IVec2::IVec2(int x, int y) : x(x), y(y) { }

inline int& IVec2::operator[](size_t index) {
    assert(index >= 0 && index < 2);
    return ((int*)this)[index];
}

inline int IVec2::operator[](size_t index) const {
    return (*(IVec2*)this)[index];
}

inline IVec2 IVec2::operator-() const {
    return IVec2(-x, -y);
}

inline IVec2 IVec2::operator+(const IVec2& v) const {
    return IVec2(x + v.x, y + v.y);
}

inline IVec2 IVec2::operator-(const IVec2& v) const {
    return IVec2(x - v.x, y - v.y);
}

inline IVec2 IVec2::operator*(int s) const {
    return IVec2(x * s, y * s);
}

inline IVec2 IVec2::operator/(int s) const {
    return IVec2(x / s, y / s);
}

inline IVec2& IVec2::operator+=(const IVec2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}

inline IVec2& IVec2::operator-=(const IVec2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

inline IVec2& IVec2::operator*=(int s)
{
    x *= s;
    y *= s;
    return *this;
}

inline IVec2& IVec2::operator/=(int s)
{
    x /= s;
    y /= s;
    return *this;
}

inline bool IVec2::operator==(const IVec2& v) const {
    return (x == v.x && y == v.y);
}

inline bool IVec2::operator!=(const IVec2& v) const {
    return (x != v.x || y != v.y);
}

inline IVec2 IVec2::PerpCW() const {
    return IVec2(y, -x);
}

inline IVec2 IVec2::PerpCCW() const {
    return IVec2(-y, x);
}

inline std::ostream& IVec2::operator<<(std::ostream& os) {
    os << "(" << x << ", " << y << ")";
    return os;
}

}
}
