/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/GraphicsContextAndroidOpenGL.h>
#include <MW/Graphics/Graphics.h>
#include <MW/System/Pointers.h>

namespace mw::gfx {

sptr<GraphicsContext> GraphicsContext::New(GraphicsDriverType type)
{
    switch (type)
    {
    case GraphicsDriverType::Default:
    case GraphicsDriverType::OpenGL:
        return spnew<GraphicsContextAndroidOpenGL>();

    default:
        throw std::runtime_error("requested driver type is not available on this platform");
    }
}

}
