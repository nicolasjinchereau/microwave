/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Plane_p2;
import :Plane_p1;
import :Vec3_p1;
import :Vec4_p1;
import :Sphere_p1;
import <cmath>;

export namespace mw {
inline namespace math {

Plane::Plane()
    : Plane(0, 0, 0, 0) {}

Plane::Plane(float a, float b, float c, float d)
    : a(a), b(b), c(c), d(d) {}

Plane::Plane(const Vec3& normal, const Vec3& point)
    : a(normal.x), b(normal.y), c(normal.z), d(-point.Dot(normal)) {}

Plane::Plane(const Vec3& a, const Vec3& b, const Vec3& c)
    : Plane((c - b).Cross(a - b).Normalized(), b) {}

void Plane::Normalize()
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

Vec3 Plane::Normal() const
{
    return Vec3(a, b, c).Normalized();
}

bool Plane::InFront(const Vec3& point) const
{
    return (point.x * a + point.y * b + point.z * c + d) > 0.0f;
}

bool Plane::InBack(const Vec3& point) const
{
    return (point.x * a + point.y * b + point.z * c + d) < 0.0f;
}

bool Plane::InFront(const Sphere& sphere) const
{
    return (sphere.center.x * a +
        sphere.center.y * b +
        sphere.center.z * c + d) > sphere.radius;
}

bool Plane::InBack(const Sphere& sphere) const
{
    return (sphere.center.x * a +
        sphere.center.y * b +
        sphere.center.z * c + d) < -sphere.radius;
}

float Plane::Distance(const Vec3& p) const {
    return (a * p.x + b * p.y + c * p.z + d);
}

Plane::operator Vec4() const
{
    return Vec4(a, b, c, d);
}

}
}
