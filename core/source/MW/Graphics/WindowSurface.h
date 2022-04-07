/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/RenderTarget.h>
#include <MW/Graphics/GraphicsResource.h>
#include <MW/System/Pointers.h>

namespace mw {
inline namespace gfx {

class WindowSurface : public RenderTarget
{
public:
    virtual ~WindowSurface() {}
    virtual void UpdateSize() = 0;
};

}
}
