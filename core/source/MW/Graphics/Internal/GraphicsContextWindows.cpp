/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

import Microwave.Graphics.Internal.GraphicsContextWindowsD3D11;
import Microwave.Graphics.Internal.GraphicsContextWindowsOpenGL;
import Microwave.Graphics;
import Microwave.System.Pointers;
import <stdexcept>;

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

} // gfx
} // mw
