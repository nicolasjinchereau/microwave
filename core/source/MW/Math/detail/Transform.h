/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once

namespace mw {
inline namespace math {

class Vec3;
class Quat;

struct Transform
{
    Vec3 position;
    Quat rotation;
    Vec3 scale;
};

} // math
} // mw
