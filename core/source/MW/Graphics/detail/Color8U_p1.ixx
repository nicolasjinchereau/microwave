/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Color:Color8U_p1;
import :ColorBGRA_p1;
import <cstdint>;

export namespace mw {
inline namespace gfx {

class Color32
{
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color32();
    Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    Color32(uint32_t c);
    Color32(const ColorBGRA& c);
    operator uint32_t() const;
    operator ColorBGRA() const;
    uint32_t ToUIntRGBA() const;
};

}
}
