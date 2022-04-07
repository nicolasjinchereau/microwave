/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/detail/Color8U.h>
#include <MW/Graphics/detail/ColorBGRA.h>

namespace mw {
inline namespace gfx {

inline Color32::Color32()
    : Color32(0, 0, 0, 0) { }

inline Color32::Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : r(r), g(g), b(b), a(a) {}

inline Color32::Color32(uint32_t c)
{
    ColorBGRA bgra = c;
    b = bgra.b;
    g = bgra.g;
    r = bgra.r;
    a = bgra.a;
}

inline Color32::Color32(const ColorBGRA& c)
{
    b = c.b;
    g = c.g;
    r = c.r;
    a = c.a;
}

inline Color32::operator uint32_t() const {
    return ColorBGRA(b, g, r, a);
}

inline Color32::operator ColorBGRA() const {
    return ColorBGRA(b, g, r, a);
}

inline uint32_t Color32::ToUIntRGBA() const {
    return *(uint32_t*)this;
}

}
}
