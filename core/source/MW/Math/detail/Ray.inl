/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/Ray.h>
#include <MW/Math/detail/Vec3.h>
#include <MW/Math/detail/Plane.h>
#include <MW/Math/detail/Triangle.h>
#include <MW/Math/detail/Sphere.h>
#include <cmath>
#include <limits>

namespace mw {
inline namespace math {

inline Ray::Ray() {}

inline Ray::Ray(const Vec3& orig, const Vec3& dir)
    : origin(orig), direction(dir) {}

inline bool Ray::Cast(const Plane& p) const
{
    float dot = p.Normal().Dot(direction);

    if (dot > -std::numeric_limits<float>::epsilon() && dot < std::numeric_limits<float>::epsilon())
        return false;

    return true;
}

inline bool Ray::Cast(const Plane& p, Vec3& hitpoint) const
{
    Plane pn = p;
    pn.Normalize();

    Vec3 normal(pn.a, pn.b, pn.c);

    float dot = normal.Dot(direction);

    if (dot > -std::numeric_limits<float>::epsilon() && dot < std::numeric_limits<float>::epsilon())
        return false;

    float t = (normal.Dot(origin) + p.d) / -normal.Dot(direction);

    hitpoint = origin + direction * t;

    return true;
}

inline bool Ray::Cast(const Sphere& s) const
{
    Vec3 sp(s.center.x, s.center.y, s.center.z);

    float t = (sp - origin).Dot(direction) / direction.Dot(direction);

    if (t < 0.0f)
        return false;

    Vec3 worldPt = origin + (direction * t);

    return sp.DistanceSq(worldPt) <= (s.radius * s.radius);
}

inline bool Ray::Cast(const Sphere& s, Vec3& hitpoint) const
{
    Vec3 loc_orig = origin - s.center;

    float a = direction.Dot(direction);
    float b = 2.0f * direction.Dot(loc_orig);
    float c = loc_orig.Dot(loc_orig) - (s.radius * s.radius);

    float discr = b * b - 4.0f * a * c;

    if (discr < -std::numeric_limits<float>::epsilon()) // < 0
    {
        return false;
    }
    else if (discr < std::numeric_limits<float>::epsilon()) // == 0
    {
        float t = -b / (2 * a);
        hitpoint = origin + direction * t;
    }
    else // > 0
    {
        discr = sqrt(discr);

        float q = (b > 0) ? (0.5f * (-b + discr)) : (0.5f * (-b - discr));
        float t0 = q / a;
        float t1 = c / q;

        if (t0 > t1)
        {
            float tmp = t0;
            t0 = t1;
            t1 = tmp;
        }

        hitpoint = origin + direction * t0;
    }

    return true;
}

inline bool Ray::Cast(const Triangle& tri) const
{
    Vec3 side_1 = tri.b - tri.a;
    Vec3 side_2 = tri.c - tri.a;

    Vec3 pvec = direction.Cross(side_2);
    float det = side_1.Dot(pvec);

    if (det < 0.000001f)
        return false;

    Vec3 tvec = origin - tri.a;
    float u = tvec.Dot(pvec);

    if (u < 0.0f || u > det)
        return false;

    Vec3 qvec = tvec.Cross(side_1);
    float v = direction.Dot(qvec);

    if (v < 0 || u + v > det)
        return false;

    return true;
}

inline bool Ray::Cast(const Triangle& tri, Vec3& hitpoint) const
{
    Vec3 side_1 = tri.b - tri.a;
    Vec3 side_2 = tri.c - tri.a;

    Vec3 pvec = direction.Cross(side_2);
    float det = side_1.Dot(pvec);

    if (det < 0.000001f)
        return false;

    Vec3 tvec = origin - tri.a;
    float u = tvec.Dot(pvec);

    if (u < 0.0f || u > det)
        return false;

    Vec3 qvec = tvec.Cross(side_1);
    float v = direction.Dot(qvec);

    if (v < 0 || u + v > det)
        return false;

    float t = side_2.Dot(qvec) / det;

    //det = 1.0f / det;
    //t *= det;
    //u *= det;
    //v *= det;

    /* calculate the point of intersection between the ray and the triangle */
    //Vec3 intersection = tri.a + (side_1 * u) + (side_2 * v);

    /* or use t to find the intersection */
    hitpoint = direction * t + origin;

    return true;
}

inline bool Ray::CastNoCull(const Triangle& tri) const
{
    Vec3 side_1 = tri.b - tri.a;
    Vec3 side_2 = tri.c - tri.a;

    Vec3 pvec = direction.Cross(side_2);
    float det = side_1.Dot(pvec);

    if (fabs(det) < 0.000001f)
        return false;

    det = 1.0f / det;

    Vec3 tvec = origin - tri.a;
    float u = tvec.Dot(pvec) * det;

    if (u < 0.0f || u > 1.0f)
        return false;

    Vec3 qvec = tvec.Cross(side_1);
    float v = direction.Dot(qvec) * det;

    if (v < 0.0f || u + v > 1.0f)
        return false;

    return true;
}

inline bool Ray::CastNoCull(const Triangle& tri, Vec3& hitpoint) const
{
    Vec3 side_1 = tri.b - tri.a;
    Vec3 side_2 = tri.c - tri.a;

    Vec3 pvec = direction.Cross(side_2);
    float det = side_1.Dot(pvec);

    if (fabs(det) < 0.000001f)
        return false;

    det = 1.0f / det;

    Vec3 tvec = origin - tri.a;
    float u = tvec.Dot(pvec) * det;

    if (u < 0.0f || u > 1.0f)
        return false;

    Vec3 qvec = tvec.Cross(side_1);
    float v = direction.Dot(qvec) * det;

    if (v < 0.0f || u + v > 1.0f)
        return false;

    float t = side_2.Dot(qvec) * det;

    /* calculate the point of intersection between the ray and the triangle */
    //Vec3 intersection = tri.a + (side_1 * u) + (side_2 * v);

    /* or use t to find the intersection */
    hitpoint = direction * t + origin;

    return true;
}

}
}
