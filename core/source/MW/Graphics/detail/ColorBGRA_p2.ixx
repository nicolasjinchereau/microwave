/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Color:ColorBGRA_p2;
import :ColorBGRA_p1;

export namespace mw {
inline namespace gfx {

ColorBGRA::ColorBGRA()
    : ColorBGRA(0, 0, 0, 0) { }

ColorBGRA::ColorBGRA(uint8_t b, uint8_t g, uint8_t r, uint8_t a)
    : b(b), g(g), r(r), a(a) {}

ColorBGRA::ColorBGRA(uint32_t c) {
    *(uint32_t*)this = c;
}

ColorBGRA::operator uint32_t() const {
    return *(uint32_t*)this;
}

}
}
