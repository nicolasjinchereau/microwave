/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWDriverContextNSGL;
import Microwave.Graphics.RenderTarget;
import Microwave.Graphics.Internal.HWDriverContext;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Graphics.Internal.RenderTextureOpenGL;
import Microwave.Graphics.Internal.WindowSurfaceAndroidOpenGL;
import Microwave.System.Exception;
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
            throw Exception("error: failed to create context");

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

    virtual void SetRenderTarget(const gptr<HWRenderTarget>& target) override
    {
        if (auto surf = gpcast<HWSurfaceNSGL>(target))
        {
            if (auto win = surf->window.lock())
            {
                [context setView : win->mainWindow.contentView] ;
                gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
            }
        }
        else if (auto tex = gpcast<HWRenderTextureOpenGL>(target))
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

    virtual void Flip(const gptr<HWRenderTarget>& target) override
    {
        if (auto surf = gpcast<HWSurfaceNSGL>(target))
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

    virtual gptr<HWSurface> CreateSurface(const gptr<Window>& window) override
    {
        auto win = gpcast<WindowMacOS>(window);
        assert(win);
        return gpnew<HWSurfaceNSGL>(win);
    }
};

} // gfx
} // mw
