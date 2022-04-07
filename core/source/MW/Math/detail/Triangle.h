/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once

namespace mw {
inline namespace math {

class Vec3;

class Triangle
{
public:
    Vec3 a;
    Vec3 b;
    Vec3 c;

    Triangle();
    Triangle(const Vec3& a, const Vec3& b, const Vec3& c);
};

}
}
