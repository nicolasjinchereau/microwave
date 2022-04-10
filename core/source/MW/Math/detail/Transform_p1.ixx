/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Transform_p1;
import :Vec3_p1;
import :Quat_p1;

export namespace mw {
inline namespace math {

struct Transform
{
	Vec3 position;
	Quat rotation;
	Vec3 scale;
};

} // math
} // mw
