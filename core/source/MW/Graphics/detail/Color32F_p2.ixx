/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Color:Color32F_p2;
import :Color32F_p1;
import :Color8U_p1;
import :Color8U_p2;
import <cstdint>;

export namespace mw {
inline namespace gfx {

Color Color::Red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
Color Color::Green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
Color Color::Blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
Color Color::Yellow() { return Color(1.0f, 1.0f, 0.0f, 1.0f); }
Color Color::Orange() { return Color(1.0f, 0.5f, 0.0f, 1.0f); }
Color Color::Purple() { return Color(0.5f, 0.0f, 0.5f, 1.0f); }
Color Color::Magenta() { return Color(1.0f, 0.0f, 1.0f, 1.0f); }
Color Color::Cyan() { return Color(0.0f, 1.0f, 1.0f, 1.0f); }
Color Color::Black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
Color Color::White() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
Color Color::Gray() { return Color(0.5f, 0.5f, 0.5f, 1.0f); }
Color Color::Clear() { return Color(0.0f, 0.0f, 0.0f, 0.0f); }

Color::Color()
    : Color(0, 0, 0, 0) {}

Color::Color(float r, float g, float b, float a)
    : r(r), g(g), b(b), a(a) {}

Color::Color(const Vec4& v)
    : r(v.x), g(v.y), b(v.z), a(v.w) {}

Color::Color(const Color32& c)
{
    float invChanMax = 1.0f / 255.0f;
    r = invChanMax * (float)c.r;
    g = invChanMax * (float)c.g;
    b = invChanMax * (float)c.b;
    a = invChanMax * (float)c.a;
}

Color::Color(uint32_t c)
{
    float invChanMax = 1.0f / 255.0f;
    b = invChanMax * (float)(c & 0xFF);
    g = invChanMax * (float)((c & 0xFF00) >> 8);
    r = invChanMax * (float)((c & 0xFF0000) >> 16);
    a = invChanMax * (float)((c & 0xFF000000) >> 24);
}

Color& Color::operator=(const Vec4& v)
{
    r = v.x;
    g = v.y;
    b = v.z;
    a = v.w;
    return *this;
}

Color Color::Blend(const Color& dst) const
{
    Color ret;
    float invA = 1.0f - a;
    ret.r = r * a + dst.r * invA;
    ret.g = g * a + dst.g * invA;
    ret.b = b * a + dst.b * invA;
    ret.a = a + dst.a * invA;
    return ret;
}

Color Color::Inverse() const
{
    return Color(1.0f - r, 1.0f - g, 1.0f - b, a);
}

Color Color::Lerp(const Color& a, const Color& b, float t)
{
    return a + (b - a) * t;
}

Color Color::Clamp(const Color& c, float lower, float upper)
{
    Color ret;
    ret.r = math::Clamp(c.r, 0.0f, 1.0f);
    ret.g = math::Clamp(c.g, 0.0f, 1.0f);
    ret.b = math::Clamp(c.b, 0.0f, 1.0f);
    ret.a = math::Clamp(c.a, 0.0f, 1.0f);
    return ret;
}

Color::operator uint32_t()
{
    uint32_t bb = ((uint32_t)(b * 255.0f) & 0xFF);
    uint32_t gg = ((uint32_t)(g * 255.0f) & 0xFF) << 8;
    uint32_t rr = ((uint32_t)(r * 255.0f) & 0xFF) << 16;
    uint32_t aa = ((uint32_t)(a * 255.0f) & 0xFF) << 24;
    return bb | gg | rr | aa;
}

Color::operator Color32()
{
    uint32_t bb = ((uint32_t)(b * 255.0f) & 0xFF);
    uint32_t gg = ((uint32_t)(g * 255.0f) & 0xFF) << 8;
    uint32_t rr = ((uint32_t)(r * 255.0f) & 0xFF) << 16;
    uint32_t aa = ((uint32_t)(a * 255.0f) & 0xFF) << 24;
    return bb | gg | rr | aa;
}

Color::operator Vec4& () {
    return *(Vec4*)this;
}

Color::operator float* () {
    return (float*)this;
}

Color Color::operator+(const Color& c) const
{
    return Color(r + c.r, g + c.g, b + c.b, a + c.a);
}

Color Color::operator-(const Color& c) const
{
    return Color(r - c.r, g - c.g, b - c.b, a - c.a);
}

Color Color::operator*(float s) const
{
    return Color(r * s, g * s, b * s, a * s);
}

Color Color::operator*(const Color& c) const
{
    return Color(r * c.r, g * c.g, b * c.b, a * c.a);
}

Color& Color::operator*=(float s)
{
    r *= s;
    g *= s;
    b *= s;
    a *= s;
    return *this;
}

Color& Color::operator*=(const Color& c)
{
    r *= c.r;
    g *= c.g;
    b *= c.b;
    a *= c.a;
    return *this;
}

Color& Color::operator+=(const Color& c)
{
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
    return *this;
}

Color& Color::operator-=(const Color& c)
{
    r -= c.r;
    g -= c.g;
    b -= c.b;
    a -= c.a;
    return *this;
}


bool Color::operator==(const Color& c) const
{
    auto epsilon = std::numeric_limits<float>::epsilon();
    return
        abs(r - c.r) <= epsilon &&
        abs(g - c.g) <= epsilon &&
        abs(b - c.b) <= epsilon &&
        abs(a - c.a) <= epsilon;
}

bool Color::operator!=(const Color& c) const {
    return !operator==(c);
}

} // gfx
} // mw
