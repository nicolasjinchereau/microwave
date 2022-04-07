/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/detail/ColorBGRA.h>

namespace mw {
inline namespace gfx {

inline ColorBGRA::ColorBGRA()
    : ColorBGRA(0, 0, 0, 0) { }

inline ColorBGRA::ColorBGRA(uint8_t b, uint8_t g, uint8_t r, uint8_t a)
    : b(b), g(g), r(r), a(a) {}

inline ColorBGRA::ColorBGRA(uint32_t c) {
    *(uint32_t*)this = c;
}

inline ColorBGRA::operator uint32_t() const {
    return *(uint32_t*)this;
}

}
}
