/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Constants;
import <climits>;
import <cmath>;

export namespace mw {
inline namespace math {

constexpr float Pi = 3.141592654f;
constexpr float HalfPi = Pi / 2.0f;
constexpr float TwoPi = Pi * 2.0f;
constexpr float DegToRad = Pi / 180.0f;
constexpr float RadToDeg = 180.0f / Pi;
constexpr float FloatTolerance = std::numeric_limits<float>::epsilon() * 3;
constexpr float InvColorMax = 1.0f / 255.0f;

} // math
} // mw
