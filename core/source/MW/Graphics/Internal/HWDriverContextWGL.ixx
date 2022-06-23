/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWDriverContextWGL;
import Microwave.Graphics.RenderTarget;
import Microwave.Graphics.Internal.HWDriverContext;
import Microwave.Graphics.Internal.HWRenderTextureOpenGL;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Graphics.Internal.HWSurfaceWGL;
import Microwave.System.Exception;
import Microwave.System.Pointers;
import Microwave.System.Internal.WindowWindows;
import <MW/System/Internal/PlatformHeaders.h>;
import <MW/System/Debug.h>;
import <memory>;

export namespace mw {
inline namespace gfx {

class HWDriverContextWGL : public HWDriverContext
{
public:
    HWND hWnd = NULL;
    HDC hDC = NULL;
    HGLRC hGLRC = NULL;
    int pixelFormat = 0;
    typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
    PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = NULL;
    HDC boundDC = NULL;

    HWDriverContextWGL()
    {
        int style = WS_DISABLED | CS_OWNDC;
        const char* name = "GraphicsSurrogate";
        HINSTANCE hInst = GetModuleHandle(NULL);
        int width = 400;
        int height = 300;

        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = style;
        wcex.lpfnWndProc = &DefWindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInst;
        wcex.hIcon = NULL;
        wcex.hCursor = NULL;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = name;
        wcex.hIconSm = NULL;

        if (!RegisterClassEx(&wcex))
            throw Exception("failed to register window class for surrogate window");

        hWnd = CreateWindow(name, name, style, 0, 0, width, height, NULL, NULL, hInst, NULL);
        if (!hWnd)
            throw Exception("failed to create surrogate window for graphics");

        hDC = GetDC(hWnd);
        if (!hDC)
            throw Exception("failed to get device context for window");

        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32, // bit depth
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            24, // z-buffer depth
            0, // stencil-buffer depth
            0, 0, 0, 0, 0, 0,
        };

        pixelFormat = ChoosePixelFormat(hDC, &pfd);
        SetPixelFormat(hDC, pixelFormat, &pfd);

        hGLRC = wglCreateContext(hDC);
        if (!hGLRC)
            throw Exception("couldn't create an OpenGL context");

        boundDC = hDC;
        wglMakeCurrent(boundDC, hGLRC);
        
        if (glewInit() != GLEW_OK)
            throw Exception("couldn't initialize OpenGL extensions");

        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
    }

    ~HWDriverContextWGL()
    {
        if (hGLRC) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(hGLRC);
        }

        if (hWnd) {
            DestroyWindow(hWnd);
        }
    }
    
    virtual void SetActive(bool active) override
    {
        if (active)
            wglMakeCurrent(boundDC, hGLRC);
        else
            wglMakeCurrent(nullptr, nullptr);
    }

    virtual void SetRenderTarget(const gptr<HWRenderTarget>& target) override
    {
        if (auto surf = gpcast<HWSurfaceWGL>(target))
        {
            if (auto win = surf->window.lock())
            {
                boundDC = win->hDC;
                wglMakeCurrent(boundDC, hGLRC);
                gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
            }
        }
        else if (auto tex = gpcast<HWRenderTextureOpenGL>(target))
        {
            boundDC = hDC;
            wglMakeCurrent(boundDC, hGLRC);
            gl::BindFramebuffer(gl::FRAMEBUFFER, tex->frameBuffer);
        }
        else
        {
            boundDC = hDC;
            wglMakeCurrent(boundDC, hGLRC);
            gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
        }
    }

    virtual void Flip(const gptr<HWRenderTarget>& target) override
    {
        if (auto surf = gpcast<HWSurfaceWGL>(target))
        {
            if (auto win = surf->window.lock())
                SwapBuffers(win->hDC);
        }
    }

    virtual void SetSwapInterval(int interval) override
    {
        if (wglSwapIntervalEXT)
            wglSwapIntervalEXT(interval);
    }

    virtual gptr<HWSurface> CreateSurface(const gptr<Window>& window) override
    {
        auto win = gpcast<WindowWindows>(window);
        Assert(win);
        return gpnew<HWSurfaceWGL>(win);
    }
};

} // gfx
} // mw
