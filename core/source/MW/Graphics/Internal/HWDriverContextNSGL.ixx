/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWDriverContextNSGL;
import Microwave.Graphics.RenderTarget;
import Microwave.Graphics.Internal.HWDriverContext;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Graphics.Internal.RenderTextureOpenGL;
import Microwave.Graphics.Internal.WindowSurfaceAndroidOpenGL;
import Microwave.System.Pointers;
import <MW/System/Internal/PlatformHeaders.h>;
import <memory>;

export namespace mw {
inline namespace gfx {

class HWDriverContextNSGL : public HWDriverContext
{
public:
    NSOpenGLContext* context = nil;

    HWDriverContextNSGL()
    {
        NSOpenGLPixelFormatAttribute attribs[] = {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core, // NSOpenGLProfileVersion4_1Core
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAAccelerated,
            0
        };

        NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
        context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
        if(!context)
            throw std::runtime_error("error: failed to create context");

        [context makeCurrentContext];

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    ~HWDriverContextNSGL()
    {
        if(context) {
            [context setView:nil];
            context = nil;
        }
    }
    
    virtual void SetActive(bool active) override
    {
        if (active)
            [context makeCurrentContext];
        else
            [context clearCurrentContext];
    }

    virtual void SetRenderTarget(const sptr<HWRenderTarget>& target) override
    {
        if (auto surf = spcast<HWSurfaceNSGL>(target))
        {
            if (auto win = surf->window.lock())
            {
                [context setView : win->mainWindow.contentView] ;
                gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
            }
        }
        else if (auto tex = spcast<HWRenderTextureOpenGL>(target))
        {
            [context setView: nil];
            gl::BindFramebuffer(gl::FRAMEBUFFER, tex->frameBuffer);
        }
        else
        {
            [context setView: nil];
            gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        }
    }

    virtual void Flip(const sptr<HWRenderTarget>& target) override
    {
        if (auto surf = spcast<HWSurfaceNSGL>(target))
        {
            gl::Flush();
            [context flushBuffer];
        }
    }

    virtual void SetSwapInterval(int interval) override
    {
        if(context)
        {
            GLint swapInterval = (GLint)interval;
            [context setValues: &swapInterval
                  forParameter: NSOpenGLContextParameterSwapInterval];
        }
    }

    virtual sptr<HWSurface> CreateSurface(const sptr<Window>& window) override
    {
        auto win = spcast<WindowMacOS>(window);
        assert(win);
        return spnew<HWSurfaceNSGL>(win);
    }
};

} // gfx
} // mw
