/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Color:ColorBGRA_p1;
import <cstdint>;

export namespace mw {
inline namespace gfx {

class ColorBGRA
{
public:
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;

    ColorBGRA();
    ColorBGRA(uint8_t b, uint8_t g, uint8_t r, uint8_t a);
    ColorBGRA(uint32_t c);
    operator uint32_t() const;
};

}
}
