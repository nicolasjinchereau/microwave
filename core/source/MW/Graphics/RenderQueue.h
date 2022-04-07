/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once

namespace mw {
inline namespace gfx {

struct RenderQueue
{
    constexpr static const uint32_t Background = 1000;
    constexpr static const uint32_t Opaque = 2000;
    constexpr static const uint32_t Transparent = 3000;
    constexpr static const uint32_t Overlay = 4000;
};

} // gfx
} // mw
