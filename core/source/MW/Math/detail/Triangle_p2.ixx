/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Triangle_p2;
import :Triangle_p1;
import :Vec3_p1;

export namespace mw {
inline namespace math {

Triangle::Triangle() : a(Vec3::Zero()), b(Vec3::Zero()), c(Vec3::Zero()) {}
Triangle::Triangle(const Vec3& a, const Vec3& b, const Vec3& c) : a(a), b(b), c(c) {}

}
}
