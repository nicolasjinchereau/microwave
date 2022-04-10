/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Ray_p1;
import :Vec3_p1;

export namespace mw {
inline namespace math {

class Plane;
class Sphere;
class Triangle;

class Ray
{
public:
    Vec3 origin;
    Vec3 direction;

    Ray();
    Ray(const Vec3& orig, const Vec3& dir);

    bool Cast(const Plane& p) const;
    bool Cast(const Plane& p, Vec3& hitpoint) const;
    bool Cast(const Sphere& s) const;
    bool Cast(const Sphere& s, Vec3& hitpoint) const;
    bool Cast(const Triangle& tri) const;
    bool Cast(const Triangle& tri, Vec3& hitpoint) const;
    bool CastNoCull(const Triangle& tri) const; // no backface culling
    bool CastNoCull(const Triangle& tri, Vec3& hitpoint) const;
};

}
}
