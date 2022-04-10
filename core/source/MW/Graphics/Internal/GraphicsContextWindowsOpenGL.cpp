/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.GraphicsContextWindowsOpenGL;
import Microwave.Graphics.Internal.GraphicsContextWindowsOpenGL;
import Microwave.Graphics.Internal.WindowSurfaceWindowsOpenGL;
import Microwave.Graphics.Internal.RenderTextureOpenGL;
import Microwave.Graphics.Internal.ShaderOpenGL;
import Microwave.Graphics.Internal.DrawBufferOpenGL;
import Microwave.Graphics.Internal.TextureOpenGL;
import Microwave.System.Console;
import Microwave.System.Internal.WindowWindows;
import <stdexcept>;
import <unordered_map>;
import <MW/System/Internal/PlatformHeaders.h>;

namespace mw {
inline namespace gfx {

std::unordered_map<BlendFactor, GLenum> blendFactors = {
    { BlendFactor::Zero, GL_ZERO },
    { BlendFactor::One, GL_ONE },
    { BlendFactor::SrcColor, GL_SRC_COLOR },
    { BlendFactor::OneMinusSrcColor, GL_ONE_MINUS_SRC_COLOR },
    { BlendFactor::DstColor, GL_DST_COLOR },
    { BlendFactor::OneMinusDstColor, GL_ONE_MINUS_DST_COLOR },
    { BlendFactor::SrcAlpha, GL_SRC_ALPHA },
    { BlendFactor::OneMinusSrcAlpha, GL_ONE_MINUS_SRC_ALPHA },
    { BlendFactor::DstAlpha, GL_DST_ALPHA },
    { BlendFactor::OneMinusDstAlpha, GL_ONE_MINUS_DST_ALPHA },
    { BlendFactor::SrcAlphaSaturate, GL_SRC_ALPHA_SATURATE },
    { BlendFactor::ConstColor, GL_CONSTANT_COLOR },
    { BlendFactor::OneMinusConstColor, GL_ONE_MINUS_CONSTANT_COLOR },
    { BlendFactor::Src1Color, GL_SRC1_COLOR },
    { BlendFactor::OneMinusSrc1Color, GL_ONE_MINUS_SRC1_COLOR },
    { BlendFactor::Src1Alpha, GL_SRC1_ALPHA },
    { BlendFactor::OneMinusSrc1Alpha, GL_ONE_MINUS_SRC1_ALPHA }
};

std::unordered_map<BlendOperation, GLenum> blendOperations = {
    { BlendOperation::Add, GL_FUNC_ADD },
    { BlendOperation::Subtract, GL_FUNC_SUBTRACT },
    { BlendOperation::ReverseSubtract, GL_FUNC_REVERSE_SUBTRACT },
    { BlendOperation::Min, GL_MIN },
    { BlendOperation::Max, GL_MAX }
};

std::unordered_map<DrawMode, GLenum> drawModes = {
    { DrawMode::Points, GL_POINTS },
    { DrawMode::Lines, GL_LINES },
    { DrawMode::LineStrip, GL_LINE_STRIP },
    { DrawMode::Triangles, GL_TRIANGLES },
    { DrawMode::TriangleStrip, GL_TRIANGLE_STRIP }
};

namespace
{
    // https://www.khronos.org/opengl/wiki/Debug_Output
    void APIENTRY OnDebugMessage(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        GLvoid* userParam)
    {
        if (severity == GL_DEBUG_SEVERITY_HIGH ||
            severity == GL_DEBUG_SEVERITY_MEDIUM)
        {
            Console::WriteLine("OpenGL: %", std::string_view(message));
        }
    }
}

GraphicsContextWindowsOpenGL::GraphicsContextWindowsOpenGL()
{
    _dispatcher = Dispatcher::GetCurrent();
    AllocateGraphicsResource();
}

GraphicsContextWindowsOpenGL::~GraphicsContextWindowsOpenGL()
{
    ReleaseGraphicsResource();
}

sptr<Dispatcher> GraphicsContextWindowsOpenGL::GetDispatcher() {
    return _dispatcher;
}

Vec2 GraphicsContextWindowsOpenGL::GetDepthRangeNDC() {
    return Vec2(-1.0f, 1.0f);
}

void GraphicsContextWindowsOpenGL::MakeCurrent() {
    auto hDC = _targetSurface ? _targetSurface->window->hDC : _hDC;
    wglMakeCurrent(hDC, _hGLRC);
}

IntRect GraphicsContextWindowsOpenGL::GetViewport() const {
    return _viewportRect;
}

void GraphicsContextWindowsOpenGL::SetViewport(const IntRect& rect)
{
    _viewportRect = rect;
    gl::Viewport(rect.x, rect.y, rect.w, rect.h);
}

bool GraphicsContextWindowsOpenGL::IsBlendingEnabled() const {
    return _blendingEnabled;
}

void GraphicsContextWindowsOpenGL::SetBlendingEnabled(bool enabled) {
    _blendingEnabled = enabled;
    auto fun = enabled ? &gl::Enable : &gl::Disable;
    fun(GL_BLEND);
}

bool GraphicsContextWindowsOpenGL::IsScissorTestEnabled() const {
    return _scissorTestEnabled;
}

void GraphicsContextWindowsOpenGL::SetScissorTestEnabled(bool enabled) {
    _scissorTestEnabled = enabled;
    auto fun = enabled ? &gl::Enable : &gl::Disable;
    fun(GL_SCISSOR_TEST);
}

IntRect GraphicsContextWindowsOpenGL::GetScissorRect() const {
    return _scissorRect;
}

void GraphicsContextWindowsOpenGL::SetScissorRect(const IntRect& rect) {
    _scissorRect = rect;
    gl::Scissor(rect.x, rect.y, rect.w, rect.h);
}

Color GraphicsContextWindowsOpenGL::GetClearColor() const {
    return _clearColor;
}

void GraphicsContextWindowsOpenGL::SetClearColor(const Color& color) {
    _clearColor = color;
    gl::ClearColor(color.r, color.g, color.b, color.a);
}

void GraphicsContextWindowsOpenGL::SetColorMask(bool red, bool green, bool blue, bool alpha) {
    _colorMask[0] = red;
    _colorMask[1] = green;
    _colorMask[2] = blue;
    _colorMask[3] = alpha;
    gl::ColorMask((GLboolean)red, (GLboolean)green, (GLboolean)blue, (GLboolean)alpha);
}

void GraphicsContextWindowsOpenGL::SetBlendFactors(BlendFactor source, BlendFactor dest) {
    _srcColorFactor = source;
    _dstColorFactor = dest;
    _srcAlphaFactor = source;
    _dstAlphaFactor = dest;
    gl::BlendFunc(blendFactors[source], blendFactors[dest]);
}

void GraphicsContextWindowsOpenGL::SetBlendFactors(
    BlendFactor sourceColor, BlendFactor destColor,
    BlendFactor sourceAlpha, BlendFactor destAlpha)
{
    _srcColorFactor = sourceColor;
    _dstColorFactor = destColor;
    _srcAlphaFactor = sourceAlpha;
    _dstAlphaFactor = destAlpha;

    gl::BlendFuncSeparate(
        blendFactors[sourceColor], blendFactors[destColor],
        blendFactors[sourceAlpha], blendFactors[destAlpha]);
}

BlendFactor GraphicsContextWindowsOpenGL::GetSourceColorBlendFactor() const {
    return _srcColorFactor;
}

BlendFactor GraphicsContextWindowsOpenGL::GetDestColorBlendFactor() const {
    return _dstColorFactor;
}

BlendFactor GraphicsContextWindowsOpenGL::GetSourceAlphaBlendFactor() const {
    return _srcAlphaFactor;
}

BlendFactor GraphicsContextWindowsOpenGL::GetDestAlphaBlendFactor() const {
    return _dstAlphaFactor;
}

void GraphicsContextWindowsOpenGL::SetBlendOperations(BlendOperation blendOp)
{
    if (_colorBlendOperation != blendOp || _alphaBlendOperation != blendOp)
    {
        _colorBlendOperation = blendOp;
        _alphaBlendOperation = blendOp;

        gl::BlendEquationSeparate(
            blendOperations[_colorBlendOperation],
            blendOperations[_alphaBlendOperation]
        );
    }
}

void GraphicsContextWindowsOpenGL::SetBlendOperations(
    BlendOperation colorBlendOp, BlendOperation alphaBlendOp)
{
    if (_colorBlendOperation != colorBlendOp || _alphaBlendOperation != alphaBlendOp)
    {
        _colorBlendOperation = colorBlendOp;
        _alphaBlendOperation = alphaBlendOp;

        gl::BlendEquationSeparate(
            blendOperations[_colorBlendOperation],
            blendOperations[_alphaBlendOperation]
        );
    }
}

BlendOperation GraphicsContextWindowsOpenGL::GetColorBlendOperation() const {
    return _colorBlendOperation;
}

BlendOperation GraphicsContextWindowsOpenGL::GetAlphaBlendOperation() const {
    return _alphaBlendOperation;
}

Color GraphicsContextWindowsOpenGL::GetBlendColor() const {
    return _blendColor;
}

void GraphicsContextWindowsOpenGL::SetBlendColor(Color color)
{
    _blendColor = color;
    gl::BlendColor(color.r, color.g, color.b, color.a);
}

CullMode GraphicsContextWindowsOpenGL::GetCullMode() const {
    return _cullMode;
}

void GraphicsContextWindowsOpenGL::SetCullMode(CullMode mode) {
    _cullMode = mode;

    switch (mode)
    {
    case CullMode::Off:
        gl::Disable(GL_CULL_FACE);
        break;

    case CullMode::Front:
        gl::Enable(GL_CULL_FACE);
        gl::CullFace(GL_FRONT);
        break;

    case CullMode::Back:
        gl::Enable(GL_CULL_FACE);
        gl::CullFace(GL_BACK);
        break;
    }
}

DepthTest GraphicsContextWindowsOpenGL::GetDepthTest() const {
    return _depthTest;
}

void GraphicsContextWindowsOpenGL::SetDepthTest(DepthTest test)
{
    _depthTest = test;

    switch (_depthTest)
    {
    case DepthTest::Always:
        gl::Disable(GL_DEPTH_TEST);
        gl::DepthFunc(GL_ALWAYS);
        break;

    case DepthTest::Never:
        gl::Enable(GL_DEPTH_TEST);
        gl::DepthFunc(GL_NEVER);
        break;

    case DepthTest::Less:
        gl::Enable(GL_DEPTH_TEST);
        gl::DepthFunc(GL_LESS);
        break;

    case DepthTest::Equal:
        gl::Enable(GL_DEPTH_TEST);
        gl::DepthFunc(GL_EQUAL);
        break;

    case DepthTest::LessOrEqual:
        gl::Enable(GL_DEPTH_TEST);
        gl::DepthFunc(GL_LEQUAL);
        break;

    case DepthTest::Greater:
        gl::Enable(GL_DEPTH_TEST);
        gl::DepthFunc(GL_GREATER);
        break;

    case DepthTest::NotEqual:
        gl::Enable(GL_DEPTH_TEST);
        gl::DepthFunc(GL_NOTEQUAL);
        break;

    case DepthTest::GreaterOrEqual:
        gl::Enable(GL_DEPTH_TEST);
        gl::DepthFunc(GL_GEQUAL);
        break;
    }
}

bool GraphicsContextWindowsOpenGL::IsDepthWriteEnabled() const {
    return _depthWriteEnabled;
}

void GraphicsContextWindowsOpenGL::SetDepthWriteEnabled(bool enabled) {
    _depthWriteEnabled = enabled;
    gl::DepthMask(_depthWriteEnabled);
}

int GraphicsContextWindowsOpenGL::GetSwapInterval() const {
    return _swapInterval;
}

void GraphicsContextWindowsOpenGL::SetSwapInterval(int interval) {
    _swapInterval = interval;
    if (wglSwapIntervalEXT)
        wglSwapIntervalEXT(interval);
}

sptr<RenderTarget> GraphicsContextWindowsOpenGL::GetRenderTarget() const {
    return _targetSurface ? (sptr<RenderTarget>)_targetSurface : (sptr<RenderTarget>)_targetTexture;
}

void GraphicsContextWindowsOpenGL::SetRenderTarget(const sptr<RenderTarget>& target) {
    target->Bind(This<GraphicsContext>());
}

void GraphicsContextWindowsOpenGL::SetRenderTarget(const sptr<WindowSurface>& target)
{
    _targetTexture = nullptr;
    _targetSurface = std::dynamic_pointer_cast<WindowSurfaceWindowsOpenGL>(target);

    if (_targetSurface)
    {
        wglMakeCurrent(_targetSurface->window->hDC, this->_hGLRC);
        gl::BindFramebuffer(GL_FRAMEBUFFER, NULL);

        auto rc = IntRect(0, 0, _targetSurface->window->size.x, _targetSurface->window->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        wglMakeCurrent(this->_hDC, this->_hGLRC);
        gl::BindFramebuffer(GL_FRAMEBUFFER, NULL);

        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextWindowsOpenGL::SetRenderTarget(const sptr<RenderTexture>& target)
{
    _targetSurface = nullptr;
    _targetTexture = std::dynamic_pointer_cast<RenderTextureOpenGL>(target);

    if (_targetTexture)
    {
        wglMakeCurrent(this->_hDC, this->_hGLRC);
        gl::BindFramebuffer(GL_FRAMEBUFFER, _targetTexture->frameBuffer);

        auto rc = IntRect(0, 0, _targetTexture->size.x, _targetTexture->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        wglMakeCurrent(this->_hDC, this->_hGLRC);
        gl::BindFramebuffer(GL_FRAMEBUFFER, NULL);

        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextWindowsOpenGL::RebindRenderTarget()
{
    if (_targetSurface)
    {
        wglMakeCurrent(_targetSurface->window->hDC, this->_hGLRC);
        gl::BindFramebuffer(GL_FRAMEBUFFER, NULL);

        auto rc = IntRect(0, 0, _targetSurface->window->size.x, _targetSurface->window->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else if (_targetTexture)
    {
        wglMakeCurrent(this->_hDC, this->_hGLRC);
        gl::BindFramebuffer(GL_FRAMEBUFFER, _targetTexture->frameBuffer);

        auto rc = IntRect(0, 0, _targetTexture->size.x, _targetTexture->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        wglMakeCurrent(this->_hDC, this->_hGLRC);
        gl::BindFramebuffer(GL_FRAMEBUFFER, NULL);

        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextWindowsOpenGL::Clear(bool depth, bool color)
{
    if (_targetSurface || _targetTexture)
    {
        GLuint flags = 0;

        if (depth)
        {
            flags |= GL_DEPTH_BUFFER_BIT;

            if (!_depthWriteEnabled)
                gl::DepthMask(true);
        }

        if (color)
        {
            flags |= GL_COLOR_BUFFER_BIT;
        }

        gl::Clear(flags);

        if (depth && !_depthWriteEnabled)
            gl::DepthMask(false);
    }
}

void GraphicsContextWindowsOpenGL::Flush()
{
    gl::Flush();
}

void GraphicsContextWindowsOpenGL::Flip()
{
    if (_targetSurface) {
        SwapBuffers(_targetSurface->window->hDC);
    }
}

void GraphicsContextWindowsOpenGL::DrawArray(int start, int count, DrawMode mode) {
    gl::DrawArrays(drawModes[mode], start, count);
}

void GraphicsContextWindowsOpenGL::DrawIndexed(int start, int count, DrawMode mode) {
    gl::DrawElements(drawModes[mode], count, GL_UNSIGNED_INT, nullptr);
}

sptr<RenderTexture> GraphicsContextWindowsOpenGL::CreateRenderTexture(const Vec2& size) {
    return spnew<RenderTextureOpenGL>(size);
}

void GraphicsContextWindowsOpenGL::CreateWindowSurface(const sptr<Window>& window) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextWindowsOpenGL>(This());
    auto win = std::dynamic_pointer_cast<WindowWindows>(window);
    win->surface = spnew<WindowSurfaceWindowsOpenGL>(gfx, win);
}

sptr<Shader> GraphicsContextWindowsOpenGL::CreateShader(const std::string& source) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextWindowsOpenGL>(This());
    return spnew<ShaderOpenGL>(source);
}

sptr<DrawBuffer> GraphicsContextWindowsOpenGL::CreateBuffer(uint32_t capacity, DrawBufferType type, bool dynamic, const std::span<std::byte>& data) {
    return spnew<DrawBufferOpenGL>(capacity, type, dynamic, data);
}

sptr<Texture> GraphicsContextWindowsOpenGL::CreateTexture(const IVec2& size, PixelDataFormat format, bool dynamic, const std::span<std::byte>& data) {
    return spnew<TextureOpenGL>(size, format, dynamic, data);
}

Mat4 GraphicsContextWindowsOpenGL::GetOrthoMatrix(float left, float right, float bottom, float top, float znear, float zfar) {
    return Mat4::OrthoOpenGL(left, right, bottom, top, znear, zfar);
}

Mat4 GraphicsContextWindowsOpenGL::GetPerspectiveMatrix(float fovY, float aspect, float znear, float zfar) {
    return Mat4::PerspectiveOpenGL(fovY, aspect, znear, zfar);
}

void GraphicsContextWindowsOpenGL::AllocateGraphicsResource()
{
    if (_alive)
        return;

    try
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
            throw std::runtime_error("failed to register window class for surrogate window");

        _hWnd = CreateWindow(name, name, style, 0, 0, width, height, NULL, NULL, hInst, NULL);
        if (!_hWnd)
            throw std::runtime_error("failed to create surrogate window for graphics");

        _hDC = GetDC(_hWnd);
        if (!_hDC)
            throw std::runtime_error("failed to get device context for window");

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

        _pixelFormat = ChoosePixelFormat(_hDC, &pfd);
        SetPixelFormat(_hDC, _pixelFormat, &pfd);

        _hGLRC = wglCreateContext(_hDC);
        if (!_hGLRC)
            throw std::runtime_error("couldn't create an OpenGL context");

        //_hAuxGLRC = wglCreateContext(_hDC);
        //if(!_hAuxGLRC)
        //    throw runtime_error("couldn't create auxiliary context");

        //if(!wglShareLists(_hGLRC, _hAuxGLRC))
        //    throw runtime_error("couldn't share context");

        wglMakeCurrent(_hDC, _hGLRC);

        if (glewInit() != GLEW_OK)
            throw std::runtime_error("couldn't initialize OpenGL extensions");
        
        gl::Enable(GL_DEBUG_OUTPUT);
        gl::DebugMessageCallback(OnDebugMessage, nullptr);

        gl::FrontFace(GL_CW);
        gl::Disable(GL_DITHER);
        gl::Viewport(0, 0, width, height);
        gl::Scissor(0, 0, width, height);

        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

        SetDepthTest(DepthTest::LessOrEqual);
        SetDepthWriteEnabled(true);
        SetScissorTestEnabled(_scissorTestEnabled);
        SetBlendingEnabled(_blendingEnabled);
        SetBlendFactors(_srcColorFactor, _dstColorFactor, _srcAlphaFactor, _dstAlphaFactor);
        SetBlendOperations(_colorBlendOperation, _alphaBlendOperation);
        SetCullMode(_cullMode);
        SetClearColor(_clearColor);
        SetColorMask(_colorMask[0], _colorMask[1], _colorMask[2], _colorMask[3]);
        SetViewport(_viewportRect);
        SetScissorRect(_scissorRect);

        _alive = true;
    }
    catch (...)
    {
        ReleaseGraphicsResource();
        throw;
    }
}

void GraphicsContextWindowsOpenGL::ReleaseGraphicsResource()
{
    _alive = false;

    _targetSurface = nullptr;
    _targetTexture = nullptr;

    if (_hGLRC) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(_hGLRC);
        _hGLRC = NULL;
    }

    if (_hAuxGLRC) {
        wglDeleteContext(_hAuxGLRC);
        _hAuxGLRC = NULL;
    }

    if (_hWnd) {
        DestroyWindow(_hWnd);
        _hWnd = NULL;
    }

    _hDC = NULL;
    wglSwapIntervalEXT = NULL;
    _pixelFormat = 0;
}

bool GraphicsContextWindowsOpenGL::IsGraphicsResourceAllocated() const {
    return _alive;
}

GraphicsDriverType GraphicsContextWindowsOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

}
}
