/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Color32;
import Microwave.Graphics.ColorBGRA;
import Microwave.System.Json;
import <cstdint>;

export namespace mw {
inline namespace gfx {

struct Color32
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color32()
        : Color32(0, 0, 0, 0) { }

    Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : r(r), g(g), b(b), a(a) {}

    Color32(uint32_t c)
    {
        ColorBGRA bgra = c;
        b = bgra.b;
        g = bgra.g;
        r = bgra.r;
        a = bgra.a;
    }

    Color32(const ColorBGRA& c)
    {
        b = c.b;
        g = c.g;
        r = c.r;
        a = c.a;
    }

    operator uint32_t() const {
        return ColorBGRA(b, g, r, a);
    }

    operator ColorBGRA() const {
        return ColorBGRA(b, g, r, a);
    }

    uint32_t ToUIntRGBA() const {
        return *(uint32_t*)this;
    }
};

void to_json(json& obj, const Color32& c)
{
    obj["r"] = c.r;
    obj["g"] = c.g;
    obj["b"] = c.b;
    obj["a"] = c.a;
}

void from_json(const json& obj, Color32& c)
{
    c.r = obj.value("r", c.r);
    c.g = obj.value("g", c.g);
    c.b = obj.value("b", c.b);
    c.a = obj.value("a", c.a);
}

} // gfx
} // mw
