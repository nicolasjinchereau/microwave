/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWDriverContextEAGL;
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

class HWDriverContextEAGL : public HWDriverContext
{
public:
    EAGLContext *context = nullptr;

    HWDriverContextEAGL()
    {
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if(!context)
            throw std::runtime_error("failed to create EAGLContext");

        [EAGLContext setCurrentContext:context];
    }

    ~HWDriverContextEAGL()
    {
        context = nil;
    }
    
    virtual void SetActive(bool active) override
    {
        if (active)
            [EAGLContext setCurrentContext:context];
        else
            [EAGLContext setCurrentContext:nil];
    }

    virtual void SetRenderTarget(const sptr<HWRenderTarget>& target) override
    {
        if (auto surf = spcast<HWSurfaceEAGL>(target))
        {
            gl::BindFramebuffer(GL_FRAMEBUFFER, surf->frameBuffer);
            gl::BindRenderbuffer(GL_RENDERBUFFER, surf->colorBuffer);
        }
        else if (auto tex = spcast<HWRenderTextureOpenGL>(target))
        {
            gl::BindFramebuffer(GL_FRAMEBUFFER, tex->frameBuffer);
            gl::BindRenderbuffer(GL_RENDERBUFFER, 0);
        }
        else
        {
            gl::BindFramebuffer(GL_FRAMEBUFFER, 0);
            gl::BindRenderbuffer(GL_RENDERBUFFER, 0);
        }
    }

    virtual void Flip(const sptr<HWRenderTarget>& target) override
    {
        if (auto surf = spcast<HWSurfaceEAGL>(target))
        {
            gl::BindFramebuffer(GL_FRAMEBUFFER, surf->frameBuffer);
            gl::BindRenderbuffer(GL_RENDERBUFFER, surf->colorBuffer);
            gl::Flush();
            [context presentRenderbuffer:GL_RENDERBUFFER];
        }
    }

    virtual void SetSwapInterval(int interval) override
    {
    }

    virtual sptr<HWSurface> CreateSurface(const sptr<Window>& window) override
    {
        auto win = spcast<WindowIOS>(window);
        assert(win);
        return spnew<HWSurfaceEAGL>(win);
    }
};

} // gfx
} // mw
