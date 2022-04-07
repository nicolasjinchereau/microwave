/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/RenderTextureWindowsD3D11.h>
#include <MW/Graphics/Internal/GraphicsContextWindowsD3D11.h>

namespace mw {
inline namespace gfx {

RenderTextureWindowsD3D11::RenderTextureWindowsD3D11(const Vec2& size)
{

}

void RenderTextureWindowsD3D11::Bind(const sptr<GraphicsContext>& graphics) {
    graphics->SetRenderTarget(This<RenderTarget>());
}

void RenderTextureWindowsD3D11::AllocateGraphicsResource() {
    
}

void RenderTextureWindowsD3D11::ReleaseGraphicsResource() {
    
}

bool RenderTextureWindowsD3D11::IsGraphicsResourceAllocated() const {
    return false;
}

GraphicsDriverType RenderTextureWindowsD3D11::GetGraphicsDriverType() const {
    return GraphicsDriverType::Direct3D11;
}

IVec2 RenderTextureWindowsD3D11::GetSize() {
    return size;
}

}
}
