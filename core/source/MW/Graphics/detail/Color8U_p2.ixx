/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Color:Color8U_p2;
import :Color8U_p1;
import :ColorBGRA_p1;
import :ColorBGRA_p2;
import <cstdint>;

export namespace mw {
inline namespace gfx {

Color32::Color32()
    : Color32(0, 0, 0, 0) { }

Color32::Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : r(r), g(g), b(b), a(a) {}

Color32::Color32(uint32_t c)
{
    ColorBGRA bgra = c;
    b = bgra.b;
    g = bgra.g;
    r = bgra.r;
    a = bgra.a;
}

Color32::Color32(const ColorBGRA& c)
{
    b = c.b;
    g = c.g;
    r = c.r;
    a = c.a;
}

Color32::operator uint32_t() const {
    return ColorBGRA(b, g, r, a);
}

Color32::operator ColorBGRA() const {
    return ColorBGRA(b, g, r, a);
}

uint32_t Color32::ToUIntRGBA() const {
    return *(uint32_t*)this;
}

}
}
