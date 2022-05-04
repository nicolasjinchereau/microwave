/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.ColorBGRA;
import Microwave.System.Json;
import <cstdint>;

export namespace mw {
inline namespace gfx {

struct ColorBGRA
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;

    ColorBGRA()
        : ColorBGRA(0, 0, 0, 0) { }

    ColorBGRA(uint8_t b, uint8_t g, uint8_t r, uint8_t a)
        : b(b), g(g), r(r), a(a) {}

    ColorBGRA(uint32_t c) {
        *(uint32_t*)this = c;
    }

    operator uint32_t() const {
        return *(uint32_t*)this;
    }
};

void to_json(json& obj, const ColorBGRA& c)
{
    obj["b"] = c.b;
    obj["g"] = c.g;
    obj["r"] = c.r;
    obj["a"] = c.a;
}

void from_json(const json& obj, ColorBGRA& c)
{
    c.b = obj.value("b", c.b);
    c.g = obj.value("g", c.g);
    c.r = obj.value("r", c.r);
    c.a = obj.value("a", c.a);
}

} // gfx
} // mw
