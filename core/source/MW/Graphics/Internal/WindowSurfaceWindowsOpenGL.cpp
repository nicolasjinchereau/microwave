/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/WindowSurfaceWindowsOpenGL.h>
#include <MW/Graphics/Internal/GraphicsContextWindowsOpenGL.h>

namespace mw {
inline namespace gfx {

WindowSurfaceWindowsOpenGL::WindowSurfaceWindowsOpenGL(
    const sptr<GraphicsContextWindowsOpenGL>& graphics,
    const sptr<WindowWindows>& window)
    : graphics(graphics), window(window)
{

}

WindowSurfaceWindowsOpenGL::~WindowSurfaceWindowsOpenGL()
{
    ReleaseGraphicsResource();
}

void WindowSurfaceWindowsOpenGL::Bind(const sptr<GraphicsContext>& graphics) {
    graphics->SetRenderTarget(This<RenderTarget>());
}

void WindowSurfaceWindowsOpenGL::AllocateGraphicsResource() {
    
}

void WindowSurfaceWindowsOpenGL::ReleaseGraphicsResource() {
    
}

bool WindowSurfaceWindowsOpenGL::IsGraphicsResourceAllocated() const {
    return false;
}

GraphicsDriverType WindowSurfaceWindowsOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

void WindowSurfaceWindowsOpenGL::UpdateSize()
{
    ReleaseGraphicsResource();
    AllocateGraphicsResource();
}

IVec2 WindowSurfaceWindowsOpenGL::GetSize() {
    return window->GetSize();
}

}
}
