/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/Triangle.h>
#include <MW/Math/detail/Vec3.h>

namespace mw {
inline namespace math {

inline Triangle::Triangle() : a(Vec3::Zero()), b(Vec3::Zero()), c(Vec3::Zero()) {}
inline Triangle::Triangle(const Vec3& a, const Vec3& b, const Vec3& c) : a(a), b(b), c(c) {}

}
}
