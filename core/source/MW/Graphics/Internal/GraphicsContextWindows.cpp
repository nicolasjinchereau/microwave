/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Internal/GraphicsContextWindowsD3D11.h>
#include <MW/Graphics/Internal/GraphicsContextWindowsOpenGL.h>
#include <MW/Graphics/Graphics.h>
#include <MW/System/Pointers.h>

namespace mw {
inline namespace gfx {

sptr<GraphicsContext> GraphicsContext::New(GraphicsDriverType type)
{
    switch (type)
    {
    case GraphicsDriverType::Default:
    case GraphicsDriverType::Direct3D11:
        return spnew<GraphicsContextWindowsD3D11>();

    case GraphicsDriverType::OpenGL:
        return spnew<GraphicsContextWindowsOpenGL>();

    default:
        throw std::runtime_error("requested driver type is not available on this platform");
    }
}

}
}
