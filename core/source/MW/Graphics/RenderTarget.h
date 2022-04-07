/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/GraphicsResource.h>
#include <MW/Graphics/GraphicsContext.h>
#include <MW/System/Pointers.h>

namespace mw {
inline namespace gfx {

class RenderTarget : public GraphicsResource
{
public:
    virtual ~RenderTarget(){}
    virtual void Bind(const sptr<GraphicsContext>& graphics) = 0;
    virtual IVec2 GetSize() = 0;
};

}
}
