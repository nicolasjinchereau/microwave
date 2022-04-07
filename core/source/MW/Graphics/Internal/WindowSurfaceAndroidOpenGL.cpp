/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/WindowSurfaceAndroidOpenGL.h>
#include <MW/Graphics/Internal/GraphicsContextAndroidOpenGL.h>
#include <unordered_map>

namespace mw {
    extern android_app* androidApp;
}

namespace mw::gfx {

WindowSurfaceAndroidOpenGL::WindowSurfaceAndroidOpenGL(
    const sptr<GraphicsContextAndroidOpenGL>& graphics,
    const sptr<WindowAndroid>& window)
    : graphics(graphics), window(window)
{
    AllocateGraphicsResource();
}

WindowSurfaceAndroidOpenGL::~WindowSurfaceAndroidOpenGL()
{
    ReleaseGraphicsResource();
}

void WindowSurfaceAndroidOpenGL::Bind(const sptr<GraphicsContext>& graphics) {
    graphics->SetRenderTarget(shared_from_this());
}

void WindowSurfaceAndroidOpenGL::UpdateSize()
{
    try {
        AllocateGraphicsResource();
    }
    catch(std::exception& ex) {
        Console::WriteLine("%", ex.what());
    }
}

void WindowSurfaceAndroidOpenGL::AllocateGraphicsResource()
{
    ReleaseGraphicsResource();

    EGLint format;
    eglGetConfigAttrib(graphics->display, graphics->config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(androidApp->window, 0, 0, format);

    surface = eglCreateWindowSurface(graphics->display, graphics->config, androidApp->window, nullptr);
    if(surface == EGL_NO_SURFACE)
        throw std::runtime_error("failed to create window surface");

    if(graphics->_targetSurface.get() == this)
    {
        if(!eglMakeCurrent(graphics->display, surface, surface, graphics->context))
            throw std::runtime_error("failed to activate target surface");
    }
}

void WindowSurfaceAndroidOpenGL::ReleaseGraphicsResource()
{
    if(graphics->_targetSurface.get() == this)
    {
        if(!eglMakeCurrent(graphics->display, graphics->dummySurface, graphics->dummySurface, graphics->context))
            throw std::runtime_error("failed to make egl dummy-surface current");
    }

    if(surface != EGL_NO_SURFACE)
        eglDestroySurface(graphics->display, surface);

    surface = EGL_NO_SURFACE;
}

bool WindowSurfaceAndroidOpenGL::IsGraphicsResourceAllocated() const {
    return surface != EGL_NO_SURFACE;
}

GraphicsDriverType WindowSurfaceAndroidOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

}
