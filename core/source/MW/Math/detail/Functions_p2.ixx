/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Functions_p2;
import :Functions_p1;
import :Quat_p1;

import <limits>;

export namespace mw {
inline namespace math {

template<>
Quat Lerp<Quat, float>(const Quat& a, const Quat& b, float t) {
    return Quat::Slerp(a, b, t);
}

} // math
} // mw
