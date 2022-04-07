/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/WindowSurfaceIOSOpenGL.h>
#include <MW/Graphics/Internal/GraphicsContextIOSOpenGL.h>

namespace mw::gfx {

WindowSurfaceIOSOpenGL::WindowSurfaceIOSOpenGL(
    const sptr<GraphicsContextIOSOpenGL>& graphics,
    const sptr<WindowIOS>& window)
    : graphics(graphics), window(window)
{
    AllocateGraphicsResource();
}

WindowSurfaceIOSOpenGL::~WindowSurfaceIOSOpenGL()
{
    ReleaseGraphicsResource();
}

void WindowSurfaceIOSOpenGL::Bind(const sptr<GraphicsContext>& graphics) {
    graphics->SetRenderTarget(shared_from_this());
}

void WindowSurfaceIOSOpenGL::UpdateSize()
{
    if(frameBuffer)
    {
        gl::BindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        gl::BindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
        [graphics->context renderbufferStorage:GL_RENDERBUFFER fromDrawable:window->mainWindow->eaglLayer];
        
        int width = 0;
        int height = 0;
        gl::GetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
        gl::GetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
        
        gl::BindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        gl::RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, width, height);
        
        GLenum status = gl::CheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("failed to update framebuffer");
    }
}

void WindowSurfaceIOSOpenGL::AllocateGraphicsResource()
{
    auto eaglLayer = window->mainWindow->eaglLayer;
    
    gl::GenFramebuffers(1, &frameBuffer);
    gl::BindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    
    // create color buffer
    gl::GenRenderbuffers(1, &colorBuffer);
    gl::BindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
    gl::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);
    [graphics->context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

    int width = 0;
    int height = 0;
    gl::GetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    gl::GetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

    gl::GenRenderbuffers(1, &depthBuffer);
    gl::BindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    gl::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    gl::RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, width, height);
    
    GLenum status = gl::CheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("failed to create framebuffer");
}

void WindowSurfaceIOSOpenGL::ReleaseGraphicsResource()
{
    if(colorBuffer) {
        gl::DeleteRenderbuffers(1, &colorBuffer);
        colorBuffer = 0;
    }
    
    if(depthBuffer) {
        gl::DeleteRenderbuffers(1, &depthBuffer);
        depthBuffer = 0;
    }
    
    if(frameBuffer) {
        gl::DeleteFramebuffers(1, &frameBuffer);
        frameBuffer = 0;
    }
}

bool WindowSurfaceIOSOpenGL::IsGraphicsResourceAllocated() const {
    return !!frameBuffer;
}

GraphicsDriverType WindowSurfaceIOSOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

}
