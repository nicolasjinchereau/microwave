/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.Functions;
import Microwave.Math.Constants;
import Microwave.Math.Quat;
import <limits>;
import <cmath>;

export namespace mw {
inline namespace math {

float ToRadians(float degrees) {
    return degrees * DegToRad;
}

float ToDegrees(float radians) {
    return radians * RadToDeg;
}
    
template<typename T>
T Min(T a) {
    return a;
}

template<typename T, typename... Ts>
T Min(T a, T b, Ts... ts) {
    return Min((a < b) ? a : b, ts...);
}

template<typename T>
T Max(T a) {
    return a;
}

template<typename T, typename... Ts>
T Max(T a, T b, Ts... ts) {
    return Max((a > b) ? a : b, ts...);
}

template<typename T>
T Clamp(T x, T lower, T upper) {
    return x < lower ? lower : (x > upper ? upper : x);
}

float Clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

template<typename T>
T NormalizedClamp(T x, T lower, T upper)
{
    float range = upper - lower;
    return (range < std::numeric_limits<float>::epsilon()) ?
        Clamp01(x - lower) : Clamp01((x - lower) / range);
}

int RoundToInt(double num) {
    return (int)(num + 0.5);
}

float Snap(float n, float span)
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
T Loop(T a, T Min, T Max)
{
    T range = Max - Min;
    while(a > Max) a -= range;
    while(a < Min) a += range;
    return a;
}

template<typename T, typename M>
T Lerp(const T& a, const T& b, M t) {
    return a + (b - a) * t;
}

template<>
Quat Lerp<Quat, float>(const Quat& a, const Quat& b, float t) {
    return Quat::Slerp(a, b, t);
}

int NextPowerOfTwo(int num)
{
    int ret = 1;

    while(ret < num)
        ret <<= 1;

    return ret;
}

bool IsPowerOfTwo(unsigned int num) {
    return (num != 0) && ((num & (num - 1)) == 0);
}

bool AlmostEqual(float a, float b, float maxDelta) {
    return abs(b - a) <= maxDelta;
}

} // math
} // mw
