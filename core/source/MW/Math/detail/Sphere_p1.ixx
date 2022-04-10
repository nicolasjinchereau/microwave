/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Sphere_p1;
import :Vec3_p1;

export namespace mw {
inline namespace math {

class Mat4;

class Sphere
{
public:
    Vec3 center;
    float radius;

    Sphere();
    Sphere(const Vec3& center, float radius);
    Sphere(float x, float y, float z, float radius);

    void Union(const Vec3& point);
    void Union(const Sphere& other);

    Sphere Transform(const Mat4& mtx) const;
};

}
}
