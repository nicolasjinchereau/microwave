/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/OpenGLAPI.h>
#include <MW/System/WindowMacOS.h>
#include <MW/Graphics/GraphicsTypes.h>
#include <MW/Graphics/Internal/GraphicsContextMacOSOpenGL.h>
#include <MW/Graphics/Internal/WindowSurfaceMacOSOpenGL.h>
#include <MW/Graphics/Internal/RenderTextureOpenGL.h>
#include <MW/Graphics/Internal/ShaderOpenGL.h>
#include <MW/Graphics/Internal/DrawBufferOpenGL.h>
#include <MW/Graphics/Internal/TextureOpenGL.h>
#include <stdexcept>

namespace mw::gfx {

static GLenum ops[] = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_MIN,
    GL_MAX
};

static GLenum factors[17] = {
    GL_ZERO, // Zero
    GL_ONE, // One
    GL_SRC_COLOR, // SrcColor
    GL_ONE_MINUS_SRC_COLOR, // OneMinusSrcColor
    GL_DST_COLOR, // DstColor
    GL_ONE_MINUS_DST_COLOR, // OneMinusDstColor
    GL_SRC_ALPHA, // SrcAlpha
    GL_ONE_MINUS_SRC_ALPHA, // OneMinusSrcAlpha
    GL_DST_ALPHA, // DstAlpha
    GL_ONE_MINUS_DST_ALPHA, // OneMinusDstAlpha
    GL_SRC_ALPHA_SATURATE, // SrcAlphaSaturate
    GL_CONSTANT_COLOR, // ConstColor
    GL_ONE_MINUS_CONSTANT_COLOR, // OneMinusConstColor
    GL_SRC1_COLOR, // Src1Color
    GL_ONE_MINUS_SRC1_COLOR, // OneMinusSrc1Color
    GL_SRC1_ALPHA, // Src1Alpha
    GL_ONE_MINUS_SRC1_ALPHA // OneMinusSrc1Alpha
};

static GLenum drawModes[] = {
    GL_POINTS,
    GL_LINES,
    GL_LINE_STRIP,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP
};

GraphicsContextMacOSOpenGL::GraphicsContextMacOSOpenGL()
{
    _dispatcher = mw::Dispatcher::GetCurrent();
    AllocateGraphicsResource();
}

GraphicsContextMacOSOpenGL::~GraphicsContextMacOSOpenGL()
{
    ReleaseGraphicsResource();
}

sptr<Dispatcher> GraphicsContextMacOSOpenGL::GetDispatcher() {
    return _dispatcher;
}

Vec2 GraphicsContextMacOSOpenGL::GetDepthRangeNDC() {
    return Vec2(-1.0f, 1.0f);
}

void GraphicsContextMacOSOpenGL::MakeCurrent() {
    [_context makeCurrentContext];
}
    
IntRect GraphicsContextMacOSOpenGL::GetViewport() const {
    return _viewportRect;
}

void GraphicsContextMacOSOpenGL::SetViewport(const IntRect& rect)
{
    _viewportRect = rect;
    glViewport(rect.x, rect.y, rect.w, rect.h);
}

bool GraphicsContextMacOSOpenGL::IsDepthBufferEnabled() const {
    return _depthEnabled;
}

void GraphicsContextMacOSOpenGL::SetDepthBufferEnabled(bool enabled) {
    _depthEnabled = enabled;
    auto fun = enabled ? &glEnable : &glDisable;
    fun(GL_DEPTH_TEST);
}

bool GraphicsContextMacOSOpenGL::IsBlendingEnabled() const {
    return _blendingEnabled;
}

void GraphicsContextMacOSOpenGL::SetBlendingEnabled(bool enabled) {
    _blendingEnabled = enabled;
    auto fun = enabled ? &glEnable : &glDisable;
    fun(GL_BLEND);
}

bool GraphicsContextMacOSOpenGL::IsScissorTestEnabled() const {
    return _scissorTestEnabled;
}

void GraphicsContextMacOSOpenGL::SetScissorTestEnabled(bool enabled) {
    _scissorTestEnabled = enabled;
    auto fun = enabled ? &glEnable : &glDisable;
    fun(GL_SCISSOR_TEST);
}

IntRect GraphicsContextMacOSOpenGL::GetScissorRect() const {
    return _scissorRect;
}

void GraphicsContextMacOSOpenGL::SetScissorRect(const IntRect& rect) {
    _scissorRect = rect;
    glScissor(rect.x, rect.y, rect.w, rect.h);
}

Color GraphicsContextMacOSOpenGL::GetClearColor() const {
    return _clearColor;
}

void GraphicsContextMacOSOpenGL::SetClearColor(const Color& color) {
    _clearColor = color;
    glClearColor(color.r, color.g, color.b, color.a);
}

void GraphicsContextMacOSOpenGL::SetColorMask(bool red, bool green, bool blue, bool alpha) {
    _colorMask[0] = red;
    _colorMask[1] = green;
    _colorMask[2] = blue;
    _colorMask[3] = alpha;
    glColorMask((GLboolean)red, (GLboolean)green, (GLboolean)blue, (GLboolean)alpha);
}

void GraphicsContextMacOSOpenGL::SetBlendFactors(BlendFactor source, BlendFactor dest) {
    _srcColorFactor = source;
    _dstColorFactor = dest;
    _srcAlphaFactor = source;
    _dstAlphaFactor = dest;
    glBlendFunc(factors[(int)source], factors[(int)dest]);
}

void GraphicsContextMacOSOpenGL::SetBlendFactors(
    BlendFactor sourceColor, BlendFactor destColor,
    BlendFactor sourceAlpha, BlendFactor destAlpha)
{
    _srcColorFactor = sourceColor;
    _dstColorFactor = destColor;
    _srcAlphaFactor = sourceAlpha;
    _dstAlphaFactor = destAlpha;

    glBlendFuncSeparate(factors[(int)sourceColor], factors[(int)destColor],
        factors[(int)sourceAlpha], factors[(int)destAlpha]);
}

BlendFactor GraphicsContextMacOSOpenGL::GetSourceColorBlendFactor() const {
    return _srcColorFactor;
}

BlendFactor GraphicsContextMacOSOpenGL::GetDestColorBlendFactor() const {
    return _dstColorFactor;
}

BlendFactor GraphicsContextMacOSOpenGL::GetSourceAlphaBlendFactor() const {
    return _srcAlphaFactor;
}

BlendFactor GraphicsContextMacOSOpenGL::GetDestAlphaBlendFactor() const {
    return _dstAlphaFactor;
}

BlendOperation GraphicsContextMacOSOpenGL::GetBlendOperation() const {
    return _blendOperation;
}

void GraphicsContextMacOSOpenGL::SetBlendOperation(BlendOperation operation)
{
    _blendOperation = operation;
    glBlendEquation(ops[(int)operation]);
}

Color GraphicsContextMacOSOpenGL::GetBlendColor() const {
    return _blendColor;
}

void GraphicsContextMacOSOpenGL::SetBlendColor(Color color)
{
    _blendColor = color;
    glBlendColor(color.r, color.g, color.b, color.a);
}

CullMode GraphicsContextMacOSOpenGL::GetCullMode() const {
    return _cullMode;
}

void GraphicsContextMacOSOpenGL::SetCullMode(CullMode mode) {
    _cullMode = mode;

    switch (mode)
    {
    case CullMode::Off:
        glDisable(GL_CULL_FACE);
        break;

    case CullMode::Front:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;

    case CullMode::Back:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    }
}

int GraphicsContextMacOSOpenGL::GetSwapInterval() const {
    return _swapInterval;
}

void GraphicsContextMacOSOpenGL::SetSwapInterval(int interval) {
    _swapInterval = interval;
    if(_context) {
        GLint swapInterval = (GLint)interval;
        [_context setValues:&swapInterval forParameter:NSOpenGLContextParameterSwapInterval];
    }
}

sptr<RenderTarget> GraphicsContextMacOSOpenGL::GetRenderTarget() const {
    return _targetSurface ? (sptr<RenderTarget>)_targetSurface : (sptr<RenderTarget>)_targetTexture;
}

void GraphicsContextMacOSOpenGL::SetRenderTarget(const sptr<RenderTarget>& target) {
    target->Bind(shared_from_this());
}

void GraphicsContextMacOSOpenGL::SetRenderTarget(const sptr<WindowSurface>& target)
{
    _targetTexture = nullptr;
    _targetSurface = std::dynamic_pointer_cast<WindowSurfaceMacOSOpenGL>(target);

    if (_targetSurface)
    {
        MainWindow* window = _targetSurface->window->mainWindow;
        [_context setView:window.contentView];
        glBindFramebuffer(GL_FRAMEBUFFER, NULL);

        auto rc = IntRect(0, 0, _targetSurface->window->size.x, _targetSurface->window->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        [_context setView:nil];
        glBindFramebuffer(GL_FRAMEBUFFER, NULL);

        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextMacOSOpenGL::SetRenderTarget(const sptr<RenderTexture>& target)
{
    _targetSurface = nullptr;
    _targetTexture = std::dynamic_pointer_cast<RenderTextureOpenGL>(target);

    if (_targetTexture)
    {
        [_context setView:nil];
        glBindFramebuffer(GL_FRAMEBUFFER, _targetTexture->frameBuffer);

        auto rc = IntRect(0, 0, _targetTexture->size.x, _targetTexture->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        [_context setView:nil];
        glBindFramebuffer(GL_FRAMEBUFFER, NULL);

        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextMacOSOpenGL::RebindRenderTarget()
{
    if (_targetSurface)
    {
        MainWindow* window = _targetSurface->window->mainWindow;
        [_context setView:window.contentView];
        glBindFramebuffer(GL_FRAMEBUFFER, NULL);

        auto rc = IntRect(0, 0, _targetSurface->window->size.x, _targetSurface->window->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else if (_targetTexture)
    {
        [_context setView:nil];
        glBindFramebuffer(GL_FRAMEBUFFER, _targetTexture->frameBuffer);

        auto rc = IntRect(0, 0, _targetTexture->size.x, _targetTexture->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        [_context setView:nil];
        glBindFramebuffer(GL_FRAMEBUFFER, NULL);

        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}
    
void GraphicsContextMacOSOpenGL::Clear(bool depth, bool color)
{
    if (_targetSurface || _targetTexture)
    {
        GLuint flags = 0;
        if (depth) flags |= GL_DEPTH_BUFFER_BIT;
        if (color) flags |= GL_COLOR_BUFFER_BIT;
        glClear(flags);
    }
}

void GraphicsContextMacOSOpenGL::Flush()
{
    glFlush();
}

void GraphicsContextMacOSOpenGL::Flip()
{
    if (_targetSurface)
    {
        glFlush();
        [_context flushBuffer];
    }
}

void GraphicsContextMacOSOpenGL::DrawArray(int start, int count, DrawMode mode) {
    glDrawArrays(drawModes[(int)mode], start, count);
}

void GraphicsContextMacOSOpenGL::DrawIndexed(int start, int count, DrawMode mode) {
    glDrawElements(drawModes[(int)mode], count, GL_UNSIGNED_INT, nullptr);
}


sptr<RenderTexture> GraphicsContextMacOSOpenGL::CreateRenderTexture(const Vec2& size) {
    return spnew<RenderTextureOpenGL>(size);
}

void GraphicsContextMacOSOpenGL::CreateWindowSurface(const sptr<Window>& window) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextMacOSOpenGL>(shared_from_this());
    auto win = std::dynamic_pointer_cast<WindowMacOS>(window);
    win->surface = spnew<WindowSurfaceMacOSOpenGL>(gfx, win);
}

sptr<Shader> GraphicsContextMacOSOpenGL::CreateShader(const std::string& source) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextMacOSOpenGL>(shared_from_this());
    return spnew<ShaderOpenGL>(source);
}

sptr<DrawBuffer> GraphicsContextMacOSOpenGL::CreateBuffer(uint32_t capacity, DrawBufferType type, bool dynamic, const mw::span<std::byte>& data) {
    return spnew<DrawBufferOpenGL>(capacity, type, dynamic, data);
}

sptr<Texture> GraphicsContextMacOSOpenGL::CreateTexture(const IVec2& size, PixelDataFormat format, bool dynamic, const mw::span<std::byte>& data) {
    return spnew<TextureOpenGL>(size, format, dynamic, data);
}

Mat4 GraphicsContextMacOSOpenGL::GetOrthoMatrix(float left, float right, float bottom, float top, float znear, float zfar) {
    return Mat4::OrthoOpenGL(left, right, bottom, top, znear, zfar);
}

Mat4 GraphicsContextMacOSOpenGL::GetPerspectiveMatrix(float fovY, float aspect, float znear, float zfar) {
    return Mat4::PerspectiveOpenGL(fovY, aspect, znear, zfar);
}

void GraphicsContextMacOSOpenGL::AllocateGraphicsResource()
{
    if (_alive)
        return;

    try
    {
        NSOpenGLPixelFormatAttribute attribs[] = {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAAccelerated,
            0
        };
        
        NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
        _context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
        [_context makeCurrentContext];
        
        if(!_context)
            throw std::runtime_error("error: failed to create context");
        
//        if (glewInit() != GLEW_OK)
//            throw std::runtime_error("couldn't initialize OpenGL extensions");
        
        gl::FrontFace(GL_CW);
        glDisable(GL_DITHER);
        
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        SetSwapInterval(_swapInterval);
        SetDepthBufferEnabled(_depthEnabled);
        SetScissorTestEnabled(_scissorTestEnabled);
        SetBlendingEnabled(_blendingEnabled);
        SetBlendFactors(_srcColorFactor, _dstColorFactor, _srcAlphaFactor, _dstAlphaFactor);
        SetBlendOperation(_blendOperation);
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

void GraphicsContextMacOSOpenGL::ReleaseGraphicsResource()
{
    _alive = false;
    _targetSurface = nullptr;
    _targetTexture = nullptr;
    
    if(_context) {
        [_context setView:nil];
        _context = nil;
    }
}

bool GraphicsContextMacOSOpenGL::IsGraphicsResourceAllocated() const {
    return _alive;
}

GraphicsDriverType GraphicsContextMacOSOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

}

