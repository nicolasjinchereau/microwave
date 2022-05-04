/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Transform;
import Microwave.Math.Vec3;
import Microwave.Math.Quat;

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
