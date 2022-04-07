/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/IVec4.h>
#include <cassert>

namespace mw {
inline namespace math {

inline IVec4 IVec4::Zero() { return IVec4(0, 0, 0, 0); }
inline IVec4 IVec4::One() { return IVec4(1, 1, 0, 0); }
inline IVec4 IVec4::Right() { return IVec4(1, 0, 0, 0); }
inline IVec4 IVec4::Left() { return IVec4(-1, 0, 0, 0); }
inline IVec4 IVec4::Up() { return IVec4(0, 1, 0, 0); }
inline IVec4 IVec4::Down() { return IVec4(0, -1, 0, 0); }
inline IVec4 IVec4::Forward() { return IVec4(0, 0, 1, 0); }
inline IVec4 IVec4::Backward() { return IVec4(0, 0, -1, 0); }

inline IVec4::IVec4() : IVec4(0, 0, 0, 0) { }
inline IVec4::IVec4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) { }

inline int& IVec4::operator[](size_t index) {
    assert(index >= 0 && index < 4);
    return ((int*)this)[index];
}

inline int IVec4::operator[](size_t index) const {
    return (*(IVec4*)this)[index];
}

inline IVec4 IVec4::operator-() const {
    return IVec4(-x, -y, -z, -w);
}

inline IVec4 IVec4::operator+(const IVec4& v) const {
    return IVec4(x + v.x, y + v.y, z + v.z, w + v.w);
}

inline IVec4 IVec4::operator-(const IVec4& v) const {
    return IVec4(x - v.x, y - v.y, z - v.z, w - v.w);
}

inline IVec4 IVec4::operator*(int s) const {
    return IVec4(x * s, y * s, z * s, w * s);
}

inline IVec4 IVec4::operator/(int s) const {
    return IVec4(x / s, y / s, z / s, w / s);
}

inline IVec4& IVec4::operator+=(const IVec4& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

inline IVec4& IVec4::operator-=(const IVec4& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

inline IVec4& IVec4::operator*=(int s)
{
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}

inline IVec4& IVec4::operator/=(int s)
{
    x /= s;
    y /= s;
    z /= s;
    w /= s;
    return *this;
}

inline bool IVec4::operator==(const IVec4& v) const {
    return (x == v.x && y == v.y && z == v.z && w == v.w);
}

inline bool IVec4::operator!=(const IVec4& v) const {
    return (x != v.x || y != v.y || z != v.z || w != v.w);
}

inline std::ostream& IVec4::operator<<(std::ostream& os) {
    os << "(" << x << ", " << y << ", " << z << ", " << w << ")";
    return os;
}

}
}
