/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Plane;
import Microwave.Math.Vec3;
import Microwave.Math.Vec4;
import Microwave.Math.Sphere;
import <cmath>;

export namespace mw {
inline namespace math {

class Plane
{
public:
    float a;
    float b;
    float c;
    float d;

    Plane()
        : Plane(0, 0, 0, 0) {}

    Plane(float a, float b, float c, float d)
        : a(a), b(b), c(c), d(d) {}

    Plane(const Vec3& normal, const Vec3& point)
        : a(normal.x), b(normal.y), c(normal.z), d(-point.Dot(normal)) {}

    Plane(const Vec3& a, const Vec3& b, const Vec3& c)
        : Plane((c - b).Cross(a - b).Normalized(), b) {}

    void Normalize()
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

    Vec3 Normal() const {
        return Vec3(a, b, c).Normalized();
    }

    float Dot(const Vec3& v) const {
        return v.x * a + v.y * b + v.z * c + d;
    }

    float Dot(const Vec4& v) const {
        return v.x * a + v.y * b + v.z * c + v.w * d;
    }

    bool InFront(const Vec3& point) const {
        return (point.x * a + point.y * b + point.z * c + d) > 0.0f;
    }

    bool InBack(const Vec3& point) const {
        return (point.x * a + point.y * b + point.z * c + d) < 0.0f;
    }

    bool InFront(const Sphere& sphere) const
    {
        return (sphere.center.x * a +
            sphere.center.y * b +
            sphere.center.z * c + d) > sphere.radius;
    }

    bool InBack(const Sphere& sphere) const
    {
        return (sphere.center.x * a +
            sphere.center.y * b +
            sphere.center.z * c + d) < -sphere.radius;
    }

    float Distance(const Vec3& p) const {
        return (a * p.x + b * p.y + c * p.z + d);
    }

    operator Vec4() const {
        return Vec4(a, b, c, d);
    }
};

} // math
} // mw
