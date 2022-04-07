/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/RenderTextureOpenGL.h>
#include <MW/Graphics/GraphicsContext.h>

namespace mw {
inline namespace gfx {

RenderTextureOpenGL::RenderTextureOpenGL(const Vec2& size)
{

}

void RenderTextureOpenGL::Bind(const sptr<GraphicsContext>& graphics) {
    graphics->SetRenderTarget(This<RenderTarget>());
}

void RenderTextureOpenGL::AllocateGraphicsResource() {

}

void RenderTextureOpenGL::ReleaseGraphicsResource() {

}

bool RenderTextureOpenGL::IsGraphicsResourceAllocated() const {
    return false;
}

GraphicsDriverType RenderTextureOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

IVec2 RenderTextureOpenGL::GetSize() {
    return size;
}

}
}
