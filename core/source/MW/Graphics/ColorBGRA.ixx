/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.ColorBGRA;
import Microwave.System.Json;
import std;

export namespace mw {
inline namespace gfx {

struct ColorBGRA
{
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
    std::uint8_t a;

    ColorBGRA()
        : ColorBGRA(0, 0, 0, 0) { }

    ColorBGRA(std::uint8_t b, std::uint8_t g, std::uint8_t r, std::uint8_t a)
        : b(b), g(g), r(r), a(a) {}

    ColorBGRA(std::uint32_t c) {
        *(std::uint32_t*)this = c;
    }

    operator std::uint32_t() const {
        return *(std::uint32_t*)this;
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
