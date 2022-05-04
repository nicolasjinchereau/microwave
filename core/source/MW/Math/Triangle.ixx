/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Triangle;
import Microwave.Math.Vec3;

export namespace mw {
inline namespace math {

class Triangle
{
public:
    Vec3 a;
    Vec3 b;
    Vec3 c;

    Triangle()
        : a(Vec3::Zero()), b(Vec3::Zero()), c(Vec3::Zero()) {}

    Triangle(const Vec3& a, const Vec3& b, const Vec3& c)
        : a(a), b(b), c(c) {}
};

} // math
} // mw
