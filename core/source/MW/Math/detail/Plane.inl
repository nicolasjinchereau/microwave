/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/Plane.h>
#include <MW/Math/detail/Vec3.h>
#include <MW/Math/detail/Vec4.h>
#include <MW/Math/detail/Sphere.h>
#include <cmath>

namespace mw {
inline namespace math {

inline Plane::Plane()
    : Plane(0, 0, 0, 0) {}

inline Plane::Plane(float a, float b, float c, float d)
    : a(a), b(b), c(c), d(d) {}

inline Plane::Plane(const Vec3& normal, const Vec3& point)
    : a(normal.x), b(normal.y), c(normal.z), d(-point.Dot(normal)) {}

inline Plane::Plane(const Vec3& a, const Vec3& b, const Vec3& c)
    : Plane((c - b).Cross(a - b).Normalized(), b) {}

inline void Plane::Normalize()
{
    float length = sqrt(a * a + b * b + c * c);

    if (length > 0.0f)
    {
        length = 1.0f / length;
        a *= length;
        b *= length;
        c *= length;
        d *= length;
    }
}

inline Vec3 Plane::Normal() const
{
    return Vec3(a, b, c).Normalized();
}

inline bool Plane::InFront(const Vec3& point) const
{
    return (point.x * a + point.y * b + point.z * c + d) > 0.0f;
}

inline bool Plane::InBack(const Vec3& point) const
{
    return (point.x * a + point.y * b + point.z * c + d) < 0.0f;
}

inline bool Plane::InFront(const Sphere& sphere) const
{
    return (sphere.center.x * a +
        sphere.center.y * b +
        sphere.center.z * c + d) > sphere.radius;
}

inline bool Plane::InBack(const Sphere& sphere) const
{
    return (sphere.center.x * a +
        sphere.center.y * b +
        sphere.center.z * c + d) < -sphere.radius;
}

inline float Plane::Distance(const Vec3& p) const {
    return (a * p.x + b * p.y + c * p.z + d);
}

inline Plane::operator Vec4() const
{
    return Vec4(a, b, c, d);
}

}
}
