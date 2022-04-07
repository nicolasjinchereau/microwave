/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/Math.h>
#include <MW/System/Pointers.h>
#include <cstdint>

namespace mw {
inline namespace gfx {

class Color32;

class Color
{
public:
    float r;
    float g;
    float b;
    float a;

    static Color Red();
    static Color Green();
    static Color Blue();
    static Color Yellow();
    static Color Orange();
    static Color Purple();
    static Color Magenta();
    static Color Cyan();
    static Color Black();
    static Color White();
    static Color Gray();
    static Color Clear();

    Color();
    Color(float r, float g, float b, float a);
    Color(const Vec4& v);
    Color(const Color32& c);
    Color(uint32_t c);

    Color& operator=(const Vec4& v);
    Color operator+(const Color& c) const;
    Color operator-(const Color& c) const;
    Color operator*(float s) const;
    Color operator*(const Color& c) const;
    Color& operator*=(float s);
    Color& operator*=(const Color& c);
    Color& operator+=(const Color& c);
    Color& operator-=(const Color& c);
    bool operator==(const Color& c) const;
    bool operator!=(const Color& c) const;
    Color Blend(const Color& dst) const;
    Color Inverse() const;
    static Color Lerp(const Color& a, const Color& b, float t);
    static Color Clamp(const Color& c, float lower = 0.0f, float upper = 1.0f);
    operator uint32_t();
    operator Color32();
    operator Vec4& ();
    operator float* ();
};

}
}
