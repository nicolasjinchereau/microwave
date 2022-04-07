/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once

namespace mw {
inline namespace math {

class Vec3;
class Vec4;
class Sphere;

class Plane
{
public:
    float a;
    float b;
    float c;
    float d;

    Plane();
    Plane(float a, float b, float c, float d);
    Plane(const Vec3& normal, const Vec3& point);

    // clockwise winding order
    Plane(const Vec3& a, const Vec3& b, const Vec3& c);

    void Normalize();
    Vec3 Normal() const;
    bool InFront(const Vec3& point) const;
    bool InBack(const Vec3& point) const;
    bool InFront(const Sphere& sphere) const;
    bool InBack(const Sphere& sphere) const;
    float Distance(const Vec3& p) const;
    operator Vec4() const;
};

}
}
