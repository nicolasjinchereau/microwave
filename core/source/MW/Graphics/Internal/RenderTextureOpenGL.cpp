/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.RenderTextureOpenGL;
import Microwave.Graphics.GraphicsContext;

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
