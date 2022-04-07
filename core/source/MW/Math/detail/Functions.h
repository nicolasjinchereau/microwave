/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <limits>
#include <cmath>

namespace mw {
inline namespace math {

class Quat;

#pragma float_control(except, off, push)
#pragma fenv_access(off)
constexpr float Pi = 3.141592654f;
constexpr float HalfPi = Pi / 2.0f;
constexpr float TwoPi = Pi * 2.0f;
constexpr float DegToRad = Pi / 180.0f;
constexpr float RadToDeg = 180.0f / Pi;
constexpr float FloatTolerance = std::numeric_limits<float>::epsilon() * 3;
constexpr float InvColorMax = 1.0f / 255.0f;
#pragma float_control(pop)

inline float ToRadians(float degrees);
inline float ToDegrees(float radians);
    
template<typename T>
inline T Min(T a);

template<typename T, typename... Ts>
inline T Min(T a, T b, Ts... ts);

template<typename T>
inline T Max(T a);

template<typename T, typename... Ts>
inline T Max(T a, T b, Ts... ts);

template<typename T>
inline T Clamp(T x, T lower, T upper);

inline float Clamp01(float value);

template<typename T>
inline T NormalizedClamp(T x, T lower, T upper);

inline int RoundToInt(double num);

inline float Snap(float n, float span);

template<class T>
inline T Loop(T a, T Min, T Max);

template<typename T, typename M>
inline T Lerp(const T& a, const T& b, M t);

template<>
inline Quat Lerp<Quat, float>(const Quat& a, const Quat& b, float t);

inline int NextPowerOfTwo(int num);

inline bool IsPowerOfTwo(unsigned int num);

inline bool AlmostEqual(float a, float b, float maxDelta = 1e-6f);

} // math
} // mw
