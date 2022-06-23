/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWSurfaceEAGL;
import Microwave.Graphics.Internal.HWContextEAGL;
import Microwave.System.Exception;
import <MW/System/Internal/PlatformHeaders.h>;
import <stdexcept>;

namespace mw {
inline namespace gfx {

HWSurfaceEAGL::HWSurfaceEAGL(
    const gptr<HWContextEAGL>& context,
    const gptr<WindowIOS>& window)
    : context(context)
    , window(window)
{
    auto eaglLayer = window->mainWindow->eaglLayer;
    
    gl::GenFramebuffers(1, &frameBuffer);
    gl::BindFramebuffer(gl::FRAMEBUFFER, frameBuffer);
    
    // create color buffer
    gl::GenRenderbuffers(1, &colorBuffer);
    gl::BindRenderbuffer(gl::RENDERBUFFER, colorBuffer);
    gl::FramebufferRenderbuffer(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::RENDERBUFFER, colorBuffer);
    [context->context renderbufferStorage:gl::RENDERBUFFER fromDrawable:eaglLayer];

    int width = 0;
    int height = 0;
    gl::GetRenderbufferParameteriv(gl::RENDERBUFFER, gl::RENDERBUFFER_WIDTH, &width);
    gl::GetRenderbufferParameteriv(gl::RENDERBUFFER, gl::RENDERBUFFER_HEIGHT, &height);

    gl::GenRenderbuffers(1, &depthBuffer);
    gl::BindRenderbuffer(gl::RENDERBUFFER, depthBuffer);
    gl::FramebufferRenderbuffer(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::RENDERBUFFER, depthBuffer);
    gl::RenderbufferStorage(gl::RENDERBUFFER, gl::DEPTH_COMPONENT24_OES, width, height);
    
    GLenum status = gl::CheckFramebufferStatus(gl::FRAMEBUFFER);
    if(status != gl::FRAMEBUFFER_COMPLETE)
        throw Exception("failed to create framebuffer");

    size = IVec2(width, height);
}

HWSurfaceEAGL::~HWSurfaceEAGL()
{
    gl::DeleteRenderbuffers(1, &colorBuffer);
    gl::DeleteRenderbuffers(1, &depthBuffer);
    gl::DeleteFramebuffers(1, &frameBuffer);
}

void HWSurfaceEAGL::UpdateSize()
{
    auto win = window.lock();
    assert(win);

    gl::BindFramebuffer(gl::FRAMEBUFFER, frameBuffer);
    gl::BindRenderbuffer(gl::RENDERBUFFER, colorBuffer);
    [context->context renderbufferStorage:gl::RENDERBUFFER fromDrawable:win->mainWindow->eaglLayer];
    
    int width = 0;
    int height = 0;
    gl::GetRenderbufferParameteriv(gl::RENDERBUFFER, gl::RENDERBUFFER_WIDTH, &width);
    gl::GetRenderbufferParameteriv(gl::RENDERBUFFER, gl::RENDERBUFFER_HEIGHT, &height);
    
    gl::BindRenderbuffer(gl::RENDERBUFFER, depthBuffer);
    gl::RenderbufferStorage(gl::RENDERBUFFER, gl::DEPTH_COMPONENT24_OES, width, height);
    
    gl::Enum status = gl::CheckFramebufferStatus(gl::FRAMEBUFFER);
    if(status != gl::FRAMEBUFFER_COMPLETE)
        throw Exception("failed to update framebuffer");
    
    size = IVec2(width, height);
}

IVec2 HWSurfaceEAGL::GetSize() const
{
    return size;
}

} // gfx
} // mw
