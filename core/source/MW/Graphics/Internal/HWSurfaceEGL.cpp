/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWSurfaceEGL;
import Microwave.Graphics.Internal.HWContextEGL;
import <unordered_map>;
import <stdexcept>;

namespace mw {

extern android_app* androidApp{};

inline namespace gfx {

HWSurfaceEGL::HWSurfaceEGL(
    const sptr<HWContextEGL>& context,
    const sptr<WindowAndroid>& window)
    : context(context)
    , window(window)
{
    EGLint format;
    eglGetConfigAttrib(context->display, context->config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(androidApp->window, 0, 0, format);

    surface = eglCreateWindowSurface(context->display, context->config, androidApp->window, nullptr);
    if(surface == EGL_NO_SURFACE)
        throw std::runtime_error("failed to create window surface");
}

HWSurfaceEGL::~HWSurfaceEGL()
{
    if(context->targetSurface.get() == this)
        context->SetRenderTarget(nullptr);

    eglDestroySurface(context->display, surface);
}

void HWSurfaceEGL::UpdateSize()
{
    try {
        bool wasBound{};

        if (context->targetSurface.get() == this) {
            wasBound = true;
            context->SetRenderTarget(nullptr);
        }
        
        eglDestroySurface(context->display, surface);

        EGLint format;
        eglGetConfigAttrib(context->display, context->config, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(androidApp->window, 0, 0, format);

        surface = eglCreateWindowSurface(context->display, context->config, androidApp->window, nullptr);
        if(surface == EGL_NO_SURFACE)
            throw std::runtime_error("failed to create window surface");

        if(wasBound)
            context->SetRenderTarget(This<HWSurfaceEGL>());
    }
    catch(std::exception& ex) {
        Console::WriteLine("%", ex.what());
    }
}

IVec2 HWSurfaceNSGL::GetSize() const
{
    auto win = window.lock();
    return win ? win->size : IVec2::Zero();
}

} // gfx
} // mw
