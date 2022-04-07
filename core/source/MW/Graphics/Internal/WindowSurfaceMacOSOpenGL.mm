/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/WindowSurfaceMacOSOpenGL.h>
#include <MW/Graphics/Internal/GraphicsContextMacOSOpenGL.h>

namespace mw::gfx {

WindowSurfaceMacOSOpenGL::WindowSurfaceMacOSOpenGL(
    const sptr<GraphicsContextMacOSOpenGL>& graphics,
    const sptr<WindowMacOS>& window)
    : graphics(graphics), window(window)
{

}

WindowSurfaceMacOSOpenGL::~WindowSurfaceMacOSOpenGL()
{
    ReleaseGraphicsResource();
}

void WindowSurfaceMacOSOpenGL::Bind(const sptr<GraphicsContext>& graphics) {
    graphics->SetRenderTarget(shared_from_this());
}

void WindowSurfaceMacOSOpenGL::AllocateGraphicsResource() {
    
}

void WindowSurfaceMacOSOpenGL::ReleaseGraphicsResource() {
    
}

bool WindowSurfaceMacOSOpenGL::IsGraphicsResourceAllocated() const {
    return false;
}

GraphicsDriverType WindowSurfaceMacOSOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

void WindowSurfaceMacOSOpenGL::UpdateSize()
{
    ReleaseGraphicsResource();
    AllocateGraphicsResource();
}
    
}
