/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/Sphere.h>
#include <MW/Math/detail/Vec3.h>
#include <MW/Math/detail/Mat4.h>
#include <cmath>
#include <limits>

namespace mw {
inline namespace math {

inline Sphere::Sphere()
    : center(Vec3::Zero()), radius(0) {}

inline Sphere::Sphere(const Vec3& center, float radius)
    : center(center), radius(radius) {}

inline Sphere::Sphere(float x, float y, float z, float radius)
    : center(x, y, z), radius(radius) {}

inline void Sphere::Union(const Vec3& point)
{
    float dist = center.Distance(point);
    if (dist > radius)
    {
        Vec3 toPoint = point - center;
        float pointDist = toPoint.Length();
        Vec3 toEdge = -toPoint / pointDist;
        Vec3 edge = center + toEdge * radius;

        center = point + (edge - point) * 0.5f;
        radius = (radius + pointDist) * 0.5f;
    }
}

inline void Sphere::Union(const Sphere& other)
{
    Vec3 toOther = other.center - center;
    float dist = toOther.Length();

    if (dist < 1e-6f)
    {
        radius = std::max(radius, other.radius);
    }
    else if (radius > dist + other.radius)
    {
        // use this sphere
    }
    else if (other.radius > dist + radius)
    {
        center = other.center;
        radius = other.radius;
    }
    else
    {
        toOther /= dist;

        Vec3 limit1 = other.center + toOther * other.radius;
        Vec3 limit2 = this->center - toOther * this->radius;

        center = limit1 + (limit2 - limit1) * 0.5f;
        radius = (radius + dist + other.radius) * 0.5f;
    }
}

inline Sphere Sphere::Transform(const Mat4& mtx) const
{
    Vec3 edge = center + Vec3::Right() * radius;

    Vec3 c = Vec4(center, 1) * mtx;
    Vec3 e = Vec4(edge, 1) * mtx;
    float r = (e - c).Length();

    return { c, r };
}

}
}
