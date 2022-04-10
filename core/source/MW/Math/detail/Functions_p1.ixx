/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:Functions_p1;
import <limits>;
import <cmath>;

export namespace mw {
inline namespace math {

class Quat;

constexpr float Pi = 3.141592654f;
constexpr float HalfPi = Pi / 2.0f;
constexpr float TwoPi = Pi * 2.0f;
constexpr float DegToRad = Pi / 180.0f;
constexpr float RadToDeg = 180.0f / Pi;
constexpr float FloatTolerance = std::numeric_limits<float>::epsilon() * 3;
constexpr float InvColorMax = 1.0f / 255.0f;

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
    x = x > upper ? upper : x;
    return x < lower ? lower : x;
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
Quat Lerp<Quat, float>(const Quat& a, const Quat& b, float t);

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
