/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWDriverContextEGL;
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

class HWDriverContextEGL : public HWDriverContext
{
public:
    EGLConfig config = nullptr;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLContext context = EGL_NO_CONTEXT;
    EGLSurface dummySurface = EGL_NO_SURFACE;
    EGLSurface boundSurface = EGL_NO_SURFACE;

    HWDriverContextEGL()
    {
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if(display == EGL_NO_DISPLAY)
            throw Exception("failed to get default display");

        if(!eglInitialize(display, 0, 0))
            throw Exception("failed to initialize display");

        config = ChooseConfig(display);
        if(!config)
            throw Exception("supported display configuration not found");

        EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
        if(context == EGL_NO_CONTEXT)
            throw Exception("failed to create EGL context");

        EGLint dummySurfaceAttribs[] = {
            EGL_WIDTH, 1,
            EGL_HEIGHT, 1,
            EGL_TEXTURE_TARGET, EGL_NO_TEXTURE,
            EGL_TEXTURE_FORMAT, EGL_NO_TEXTURE,
            EGL_NONE
        };

        dummySurface = eglCreatePbufferSurface(display, config, dummySurfaceAttribs);
        if(dummySurface == EGL_NO_SURFACE)
            throw Exception("failed to create dummy surface for context");

        boundSurface = dummySurface;
        if(!eglMakeCurrent(display, boundSurface, boundSurface, context))
            throw Exception("failed to activate graphics context");
    }
    
    ~HWDriverContextEGL()
    {
        if(display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglTerminate(display);
        }

        if(context != EGL_NO_CONTEXT)
            eglDestroyContext(display, context);

        if(dummySurface != EGL_NO_SURFACE)
            eglDestroySurface(display, dummySurface);
    }
    
    virtual void SetActive(bool active) override
    {
        if (active)
            eglMakeCurrent(display, boundSurface, boundSurface, context);
        else
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    virtual void SetRenderTarget(const gptr<HWRenderTarget>& target) override
    {
        if (auto surf = gpcast<HWSurfaceEGL>(target))
        {
            boundSurface = surf->surface;
            eglMakeCurrent(display, boundSurface, boundSurface, context);
            gl::BindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        else if (auto tex = gpcast<HWRenderTextureOpenGL>(target))
        {
            boundSurface = dummySurface;
            eglMakeCurrent(display, boundSurface, boundSurface, context);
            gl::BindFramebuffer(GL_FRAMEBUFFER, tex->frameBuffer);
        }
        else
        {
            boundSurface = dummySurface;
            eglMakeCurrent(display, boundSurface, boundSurface, context);
            gl::BindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    virtual void Flip(const gptr<HWRenderTarget>& target) override
    {
        if (auto surf = gpcast<HWSurfaceEGL>(target)) {
            eglSwapBuffers(display, surf->surface);
        }
    }

    virtual void SetSwapInterval(int interval) override
    {
        if (wglSwapIntervalEXT)
            wglSwapIntervalEXT(interval);
    }

    virtual gptr<HWSurface> CreateSurface(const gptr<Window>& window) override
    {
        auto win = gpcast<WindowAndroid>(window);
        assert(win);
        return gpnew<HWSurfaceEGL>(win);
    }

    EGLConfig ChooseConfig(EGLDisplay display)
    {
        EGLint numConfigs = 0;

        if(!eglGetConfigs(display, nullptr, 0, &numConfigs) || numConfigs == 0)
            throw Exception("no graphics configurations available for this device");

        std::unique_ptr<EGLConfig[]> configs = std::make_unique<EGLConfig[]>(numConfigs);
        eglGetConfigs(display, configs.get(), numConfigs, &numConfigs);

        EGLConfig bestConfig = nullptr;
        EGLint bestSurfaceType = 0;
        EGLint bestRenderableType = 0;
        EGLint bestRedSize = 0;
        EGLint bestGreenSize = 0;
        EGLint bestBlueSize = 0;
        EGLint bestAlphaSize = 0;
        EGLint bestBufferSize = 0;
        EGLint bestDepthSize = 0;

        for(int i = 0; i < numConfigs; ++i)
        {
            EGLint surfaceType;
            EGLint renderableType;
            EGLint redSize;
            EGLint greenSize;
            EGLint blueSize;
            EGLint alphaSize;
            EGLint bufferSize;
            EGLint depthSize;

            eglGetConfigAttrib(display, configs[i], EGL_SURFACE_TYPE, &surfaceType);
            eglGetConfigAttrib(display, configs[i], EGL_RENDERABLE_TYPE, &renderableType);
            eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &redSize);
            eglGetConfigAttrib(display, configs[i], EGL_GREEN_SIZE, &greenSize);
            eglGetConfigAttrib(display, configs[i], EGL_BLUE_SIZE, &blueSize);
            eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &alphaSize);
            eglGetConfigAttrib(display, configs[i], EGL_BUFFER_SIZE, &bufferSize);
            eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &depthSize);

            if((surfaceType & (EGL_WINDOW_BIT || EGL_PBUFFER_BIT)) == 0)
                continue;

            if((renderableType & EGL_OPENGL_ES2_BIT) == 0)
                continue;

            if(redSize == 0 || greenSize == 0 || blueSize == 0 || alphaSize == 0)
                continue;

            if(bufferSize == 0 || depthSize == 0)
                continue;

            if(bestConfig == nullptr
                || renderableType > bestRenderableType
                || redSize > bestRedSize
                || greenSize > bestGreenSize
                || blueSize > bestBlueSize
                || alphaSize > bestAlphaSize
                || bufferSize > bestBufferSize
                || depthSize > bestDepthSize)
            {
                bestConfig = configs[i];
                bestSurfaceType = surfaceType;
                bestRenderableType = renderableType;
                bestRedSize = redSize;
                bestGreenSize = greenSize;
                bestBlueSize = blueSize;
                bestAlphaSize = alphaSize;
                bestBufferSize = bufferSize;
                bestDepthSize = depthSize;
            }
        }

        // std::stringstream ss;
        // ss << "Using Display Configuration:" << std::endl;
        // ss << "EGL_SURFACE_TYPE    - " << bestSurfaceType << std::endl;
        // ss << "EGL_RENDERABLE_TYPE - " << bestRenderableType << std::endl;
        // ss << "EGL_RED_SIZE        - " << bestRedSize << std::endl;
        // ss << "EGL_GREEN_SIZE      - " << bestGreenSize << std::endl;
        // ss << "EGL_BLUE_SIZE       - " << bestBlueSize << std::endl;
        // ss << "EGL_ALPHA_SIZE      - " << bestAlphaSize << std::endl;
        // ss << "EGL_BUFFER_SIZE     - " << bestBufferSize << std::endl;
        // ss << "EGL_DEPTH_SIZE      - " << bestDepthSize << std::endl;

        return bestConfig;
    }
};

} // gfx
} // mw
