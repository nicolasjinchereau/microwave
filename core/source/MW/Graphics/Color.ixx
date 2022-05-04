/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Color;
import Microwave.Graphics.Color32;
import Microwave.Math;
import Microwave.System.Json;
import <cstdint>;

export namespace mw {
inline namespace gfx {

struct Color
{
    float r;
    float g;
    float b;
    float a;

    static Color Red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
    static Color Green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
    static Color Blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
    static Color Yellow() { return Color(1.0f, 1.0f, 0.0f, 1.0f); }
    static Color Orange() { return Color(1.0f, 0.5f, 0.0f, 1.0f); }
    static Color Purple() { return Color(0.5f, 0.0f, 0.5f, 1.0f); }
    static Color Magenta() { return Color(1.0f, 0.0f, 1.0f, 1.0f); }
    static Color Cyan() { return Color(0.0f, 1.0f, 1.0f, 1.0f); }
    static Color Black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
    static Color White() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
    static Color Gray() { return Color(0.5f, 0.5f, 0.5f, 1.0f); }
    static Color Clear() { return Color(0.0f, 0.0f, 0.0f, 0.0f); }

    Color()
        : Color(0, 0, 0, 0) {}

    Color(float r, float g, float b, float a)
        : r(r), g(g), b(b), a(a) {}

    Color(const Vec4& v)
        : r(v.x), g(v.y), b(v.z), a(v.w) {}

    Color(const Color32& c)
    {
        float invChanMax = 1.0f / 255.0f;
        r = invChanMax * (float)c.r;
        g = invChanMax * (float)c.g;
        b = invChanMax * (float)c.b;
        a = invChanMax * (float)c.a;
    }

    Color(uint32_t c)
    {
        float invChanMax = 1.0f / 255.0f;
        b = invChanMax * (float)(c & 0xFF);
        g = invChanMax * (float)((c & 0xFF00) >> 8);
        r = invChanMax * (float)((c & 0xFF0000) >> 16);
        a = invChanMax * (float)((c & 0xFF000000) >> 24);
    }

    Color& operator=(const Vec4& v)
    {
        r = v.x;
        g = v.y;
        b = v.z;
        a = v.w;
        return *this;
    }

    Color Blend(const Color& dst) const
    {
        Color ret;
        float invA = 1.0f - a;
        ret.r = r * a + dst.r * invA;
        ret.g = g * a + dst.g * invA;
        ret.b = b * a + dst.b * invA;
        ret.a = a + dst.a * invA;
        return ret;
    }

    Color Inverse() const
    {
        return Color(1.0f - r, 1.0f - g, 1.0f - b, a);
    }

    Color Lerp(const Color& a, const Color& b, float t)
    {
        return a + (b - a) * t;
    }

    Color Clamp(const Color& c, float lower, float upper)
    {
        Color ret;
        ret.r = math::Clamp(c.r, 0.0f, 1.0f);
        ret.g = math::Clamp(c.g, 0.0f, 1.0f);
        ret.b = math::Clamp(c.b, 0.0f, 1.0f);
        ret.a = math::Clamp(c.a, 0.0f, 1.0f);
        return ret;
    }

    operator uint32_t()
    {
        uint32_t bb = ((uint32_t)(b * 255.0f) & 0xFF);
        uint32_t gg = ((uint32_t)(g * 255.0f) & 0xFF) << 8;
        uint32_t rr = ((uint32_t)(r * 255.0f) & 0xFF) << 16;
        uint32_t aa = ((uint32_t)(a * 255.0f) & 0xFF) << 24;
        return bb | gg | rr | aa;
    }

    operator Color32()
    {
        uint32_t bb = ((uint32_t)(b * 255.0f) & 0xFF);
        uint32_t gg = ((uint32_t)(g * 255.0f) & 0xFF) << 8;
        uint32_t rr = ((uint32_t)(r * 255.0f) & 0xFF) << 16;
        uint32_t aa = ((uint32_t)(a * 255.0f) & 0xFF) << 24;
        return bb | gg | rr | aa;
    }

    operator Vec4& () {
        return *(Vec4*)this;
    }

    operator float* () {
        return (float*)this;
    }

    Color operator+(const Color& c) const
    {
        return Color(r + c.r, g + c.g, b + c.b, a + c.a);
    }

    Color operator-(const Color& c) const
    {
        return Color(r - c.r, g - c.g, b - c.b, a - c.a);
    }

    Color operator*(float s) const
    {
        return Color(r * s, g * s, b * s, a * s);
    }

    Color operator*(const Color& c) const
    {
        return Color(r * c.r, g * c.g, b * c.b, a * c.a);
    }

    Color& operator*=(float s)
    {
        r *= s;
        g *= s;
        b *= s;
        a *= s;
        return *this;
    }

    Color& operator*=(const Color& c)
    {
        r *= c.r;
        g *= c.g;
        b *= c.b;
        a *= c.a;
        return *this;
    }

    Color& operator+=(const Color& c)
    {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }

    Color& operator-=(const Color& c)
    {
        r -= c.r;
        g -= c.g;
        b -= c.b;
        a -= c.a;
        return *this;
    }


    bool operator==(const Color& c) const
    {
        auto epsilon = std::numeric_limits<float>::epsilon();
        return
            abs(r - c.r) <= epsilon &&
            abs(g - c.g) <= epsilon &&
            abs(b - c.b) <= epsilon &&
            abs(a - c.a) <= epsilon;
    }

    bool operator!=(const Color& c) const {
        return !operator==(c);
    }
};

void to_json(json& obj, const Color& c)
{
    obj["r"] = c.r;
    obj["g"] = c.g;
    obj["b"] = c.b;
    obj["a"] = c.a;
}

void from_json(const json& obj, Color& c)
{
    c.r = obj.value("r", c.r);
    c.g = obj.value("g", c.g);
    c.b = obj.value("b", c.b);
    c.a = obj.value("a", c.a);
}

} // gfx
} // mw
