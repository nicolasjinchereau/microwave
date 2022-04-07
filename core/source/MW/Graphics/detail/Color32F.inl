/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <cstdint>
#include <MW/System/Json.h>
#include <MW/Graphics/detail/Color32F.h>
#include <MW/Graphics/detail/Color8U.h>

namespace mw {
inline namespace gfx {

inline Color Color::Red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
inline Color Color::Green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
inline Color Color::Blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
inline Color Color::Yellow() { return Color(1.0f, 1.0f, 0.0f, 1.0f); }
inline Color Color::Orange() { return Color(1.0f, 0.5f, 0.0f, 1.0f); }
inline Color Color::Purple() { return Color(0.5f, 0.0f, 0.5f, 1.0f); }
inline Color Color::Magenta() { return Color(1.0f, 0.0f, 1.0f, 1.0f); }
inline Color Color::Cyan() { return Color(0.0f, 1.0f, 1.0f, 1.0f); }
inline Color Color::Black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
inline Color Color::White() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
inline Color Color::Gray() { return Color(0.5f, 0.5f, 0.5f, 1.0f); }
inline Color Color::Clear() { return Color(0.0f, 0.0f, 0.0f, 0.0f); }

inline Color::Color()
    : Color(0, 0, 0, 0) {}

inline Color::Color(float r, float g, float b, float a)
    : r(r), g(g), b(b), a(a) {}

inline Color::Color(const Vec4& v)
    : r(v.x), g(v.y), b(v.z), a(v.w) {}

inline Color::Color(const Color32& c)
{
    float invChanMax = 1.0f / 255.0f;
    r = invChanMax * (float)c.r;
    g = invChanMax * (float)c.g;
    b = invChanMax * (float)c.b;
    a = invChanMax * (float)c.a;
}

inline Color::Color(uint32_t c)
{
    float invChanMax = 1.0f / 255.0f;
    b = invChanMax * (float)(c & 0xFF);
    g = invChanMax * (float)((c & 0xFF00) >> 8);
    r = invChanMax * (float)((c & 0xFF0000) >> 16);
    a = invChanMax * (float)((c & 0xFF000000) >> 24);
}

inline Color& Color::operator=(const Vec4& v)
{
    r = v.x;
    g = v.y;
    b = v.z;
    a = v.w;
    return *this;
}

inline Color Color::Blend(const Color& dst) const
{
    Color ret;
    float invA = 1.0f - a;
    ret.r = r * a + dst.r * invA;
    ret.g = g * a + dst.g * invA;
    ret.b = b * a + dst.b * invA;
    ret.a = a + dst.a * invA;
    return ret;
}

inline Color Color::Inverse() const
{
    return Color(1.0f - r, 1.0f - g, 1.0f - b, a);
}

inline Color Color::Lerp(const Color& a, const Color& b, float t)
{
    return a + (b - a) * t;
}

inline Color Color::Clamp(const Color& c, float lower, float upper)
{
    Color ret;
    ret.r = math::Clamp(c.r, 0.0f, 1.0f);
    ret.g = math::Clamp(c.g, 0.0f, 1.0f);
    ret.b = math::Clamp(c.b, 0.0f, 1.0f);
    ret.a = math::Clamp(c.a, 0.0f, 1.0f);
    return ret;
}

inline Color::operator uint32_t()
{
    uint32_t bb = ((uint32_t)(b * 255.0f) & 0xFF);
    uint32_t gg = ((uint32_t)(g * 255.0f) & 0xFF) << 8;
    uint32_t rr = ((uint32_t)(r * 255.0f) & 0xFF) << 16;
    uint32_t aa = ((uint32_t)(a * 255.0f) & 0xFF) << 24;
    return bb | gg | rr | aa;
}

inline Color::operator Color32()
{
    uint32_t bb = ((uint32_t)(b * 255.0f) & 0xFF);
    uint32_t gg = ((uint32_t)(g * 255.0f) & 0xFF) << 8;
    uint32_t rr = ((uint32_t)(r * 255.0f) & 0xFF) << 16;
    uint32_t aa = ((uint32_t)(a * 255.0f) & 0xFF) << 24;
    return bb | gg | rr | aa;
}

inline Color::operator Vec4& () {
    return *(Vec4*)this;
}

inline Color::operator float* () {
    return (float*)this;
}

inline Color Color::operator+(const Color& c) const
{
    return Color(r + c.r, g + c.g, b + c.b, a + c.a);
}

inline Color Color::operator-(const Color& c) const
{
    return Color(r - c.r, g - c.g, b - c.b, a - c.a);
}

inline Color Color::operator*(float s) const
{
    return Color(r * s, g * s, b * s, a * s);
}

inline Color Color::operator*(const Color& c) const
{
    return Color(r * c.r, g * c.g, b * c.b, a * c.a);
}

inline Color& Color::operator*=(float s)
{
    r *= s;
    g *= s;
    b *= s;
    a *= s;
    return *this;
}

inline Color& Color::operator*=(const Color& c)
{
    r *= c.r;
    g *= c.g;
    b *= c.b;
    a *= c.a;
    return *this;
}

inline Color& Color::operator+=(const Color& c)
{
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
    return *this;
}

inline Color& Color::operator-=(const Color& c)
{
    r -= c.r;
    g -= c.g;
    b -= c.b;
    a -= c.a;
    return *this;
}


inline bool Color::operator==(const Color& c) const
{
    auto epsilon = std::numeric_limits<float>::epsilon();
    return
        abs(r - c.r) <= epsilon &&
        abs(g - c.g) <= epsilon &&
        abs(b - c.b) <= epsilon &&
        abs(a - c.a) <= epsilon;
}

inline bool Color::operator!=(const Color& c) const {
    return !operator==(c);
}

inline void to_json(json& obj, const Color& c)
{
    obj["r"] = c.r;
    obj["g"] = c.g;
    obj["b"] = c.b;
    obj["a"] = c.a;
}

inline void from_json(const json& obj, Color& c)
{
    c.r = obj.value("r", c.r);
    c.g = obj.value("g", c.g);
    c.b = obj.value("b", c.b);
    c.a = obj.value("a", c.a);
}

}
}
