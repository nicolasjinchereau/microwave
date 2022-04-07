/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/Functions.h>
#include <MW/Math/detail/Quat.h>

namespace mw {
inline namespace math {

inline float ToRadians(float degrees) {
    return degrees * DegToRad;
}

inline float ToDegrees(float radians) {
    return radians * RadToDeg;
}
    
template<typename T>
inline T Min(T a) {
    return a;
}

template<typename T, typename... Ts>
inline T Min(T a, T b, Ts... ts) {
    return Min((a < b) ? a : b, ts...);
}

template<typename T>
inline T Max(T a) {
    return a;
}

template<typename T, typename... Ts>
inline T Max(T a, T b, Ts... ts) {
    return Max((a > b) ? a : b, ts...);
}

template<typename T>
inline T Clamp(T x, T lower, T upper) {
    x = x > upper ? upper : x;
    return x < lower ? lower : x;
}

inline float Clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

template<typename T>
inline T NormalizedClamp(T x, T lower, T upper)
{
    float range = upper - lower;
    return (range < std::numeric_limits<float>::epsilon()) ?
        Clamp01(x - lower) : Clamp01((x - lower) / range);
}

inline int RoundToInt(double num) {
    return (int)(num + 0.5);
}

inline float Snap(float n, float span)
{
    float ret;

    if(n < 0)
    {
        ret = -n + (0.5f * span);
        ret = -ret + fmod(ret, span);
    }
    else
    {
        ret = n + (0.5f * span);
        ret = ret - fmod(ret, span);
    }

    return ret;
}

template<class T>
inline T Loop(T a, T Min, T Max)
{
    T range = Max - Min;
    while(a > Max) a -= range;
    while(a < Min) a += range;
    return a;
}

template<typename T, typename M>
inline T Lerp(const T& a, const T& b, M t) {
    return a + (b - a) * t;
}

template<>
inline Quat Lerp<Quat, float>(const Quat& a, const Quat& b, float t) {
    return Quat::Slerp(a, b, t);
}

inline int NextPowerOfTwo(int num)
{
    int ret = 1;

    while(ret < num)
        ret <<= 1;

    return ret;
}

inline bool IsPowerOfTwo(unsigned int num) {
    return (num != 0) && ((num & (num - 1)) == 0);
}

inline bool AlmostEqual(float a, float b, float maxDelta) {
    return abs(b - a) <= maxDelta;
}

} // math
} // mw
