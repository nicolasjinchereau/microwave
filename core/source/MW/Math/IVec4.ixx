/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.IVec4;
import Microwave.System.Exception;
import <MW/System/Debug.h>;
import <cstdint>;
import <ostream>;

export namespace mw {
inline namespace math {

class IVec4
{
public:
    int x;
    int y;
    int z;
    int w;

    static IVec4 Zero() { return IVec4(0, 0, 0, 0); }
    static IVec4 One() { return IVec4(1, 1, 0, 0); }
    static IVec4 Right() { return IVec4(1, 0, 0, 0); }
    static IVec4 Left() { return IVec4(-1, 0, 0, 0); }
    static IVec4 Up() { return IVec4(0, 1, 0, 0); }
    static IVec4 Down() { return IVec4(0, -1, 0, 0); }
    static IVec4 Forward() { return IVec4(0, 0, 1, 0); }
    static IVec4 Backward() { return IVec4(0, 0, -1, 0); }

    IVec4() : IVec4(0, 0, 0, 0) { }
    IVec4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) { }

    int& operator[](size_t index) {
        Assert(index >= 0 && index < 4);
        return ((int*)this)[index];
    }

    int operator[](size_t index) const {
        return (*(IVec4*)this)[index];
    }

    IVec4 operator-() const {
        return IVec4(-x, -y, -z, -w);
    }

    IVec4 operator+(const IVec4& v) const {
        return IVec4(x + v.x, y + v.y, z + v.z, w + v.w);
    }

    IVec4 operator-(const IVec4& v) const {
        return IVec4(x - v.x, y - v.y, z - v.z, w - v.w);
    }

    IVec4 operator*(int s) const {
        return IVec4(x * s, y * s, z * s, w * s);
    }

    IVec4 operator/(int s) const {
        return IVec4(x / s, y / s, z / s, w / s);
    }

    IVec4& operator+=(const IVec4& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    IVec4& operator-=(const IVec4& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    IVec4& operator*=(int s)
    {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }

    IVec4& operator/=(int s)
    {
        x /= s;
        y /= s;
        z /= s;
        w /= s;
        return *this;
    }

    bool operator==(const IVec4& v) const {
        return (x == v.x && y == v.y && z == v.z && w == v.w);
    }

    bool operator!=(const IVec4& v) const {
        return (x != v.x || y != v.y || z != v.z || w != v.w);
    }

    std::ostream& operator<<(std::ostream& os) const {
        os << "(" << x << ", " << y << ", " << z << ", " << w << ")";
        return os;
    }
};

} // math
} // mw
