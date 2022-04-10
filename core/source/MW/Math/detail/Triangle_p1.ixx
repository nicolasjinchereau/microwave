/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Triangle_p1;
import :Vec3_p1;

export namespace mw {
inline namespace math {

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
