/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:IVec2_p2;
import :IVec2_p1;
import <cassert>;
import <ostream>;
import <cstdint>;

export namespace mw {
inline namespace math {

IVec2 IVec2::Zero() { return IVec2(0, 0); }
IVec2 IVec2::One() { return IVec2(1, 1); }
IVec2 IVec2::Right() { return IVec2(1, 0); }
IVec2 IVec2::Left() { return IVec2(-1, 0); }
IVec2 IVec2::Up() { return IVec2(0, 1); }
IVec2 IVec2::Down() { return IVec2(0, -1); }

IVec2::IVec2() : IVec2(0, 0) { }
IVec2::IVec2(int x, int y) : x(x), y(y) { }

int& IVec2::operator[](size_t index) {
    //assert(index >= 0 && index < 2);
    return ((int*)this)[index];
}

int IVec2::operator[](size_t index) const {
    return (*(IVec2*)this)[index];
}

IVec2 IVec2::operator-() const {
    return IVec2(-x, -y);
}

IVec2 IVec2::operator+(const IVec2& v) const {
    return IVec2(x + v.x, y + v.y);
}

IVec2 IVec2::operator-(const IVec2& v) const {
    return IVec2(x - v.x, y - v.y);
}

IVec2 IVec2::operator*(int s) const {
    return IVec2(x * s, y * s);
}

IVec2 IVec2::operator/(int s) const {
    return IVec2(x / s, y / s);
}

IVec2& IVec2::operator+=(const IVec2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}

IVec2& IVec2::operator-=(const IVec2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

IVec2& IVec2::operator*=(int s)
{
    x *= s;
    y *= s;
    return *this;
}

IVec2& IVec2::operator/=(int s)
{
    x /= s;
    y /= s;
    return *this;
}

bool IVec2::operator==(const IVec2& v) const {
    return (x == v.x && y == v.y);
}

bool IVec2::operator!=(const IVec2& v) const {
    return (x != v.x || y != v.y);
}

IVec2 IVec2::PerpCW() const {
    return IVec2(y, -x);
}

IVec2 IVec2::PerpCCW() const {
    return IVec2(-y, x);
}

std::ostream& IVec2::operator<<(std::ostream& os) const {
    os << "(" << x << ", " << y << ")";
    return os;
}

}
}
