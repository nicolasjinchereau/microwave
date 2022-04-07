/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/WindowIOS.h>
#include <MW/Graphics/GraphicsTypes.h>
#include <MW/Graphics/Internal/GraphicsContextIOSOpenGL.h>
#include <MW/Graphics/Internal/WindowSurfaceIOSOpenGL.h>
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
    GL_MIN_EXT,
    GL_MAX_EXT
};

#define GL_SRC1_COLOR                     0x88F9
#define GL_ONE_MINUS_SRC1_COLOR           0x88FA
#define GL_SRC1_ALPHA                     0x8589
#define GL_ONE_MINUS_SRC1_ALPHA           0x88FB
    
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
    
GraphicsContextIOSOpenGL::GraphicsContextIOSOpenGL()
{
    _dispatcher = mw::Dispatcher::GetCurrent();
    AllocateGraphicsResource();
}

GraphicsContextIOSOpenGL::~GraphicsContextIOSOpenGL()
{
    ReleaseGraphicsResource();
}

sptr<Dispatcher> GraphicsContextIOSOpenGL::GetDispatcher() {
    return _dispatcher;
}

Vec2 GraphicsContextIOSOpenGL::GetDepthRangeNDC() {
    return Vec2(-1.0f, 1.0f);
}

void GraphicsContextIOSOpenGL::MakeCurrent() {
    auto res = [EAGLContext setCurrentContext:context];
}
    
IntRect GraphicsContextIOSOpenGL::GetViewport() const {
    return _viewportRect;
}

void GraphicsContextIOSOpenGL::SetViewport(const IntRect& rect)
{
    _viewportRect = rect;
    gl::Viewport(rect.x, rect.y, rect.w, rect.h);
}

bool GraphicsContextIOSOpenGL::IsDepthBufferEnabled() const {
    return _depthEnabled;
}

void GraphicsContextIOSOpenGL::SetDepthBufferEnabled(bool enabled) {
    _depthEnabled = enabled;
    auto fun = enabled ? &gl::Enable : &gl::Disable;
    fun(GL_DEPTH_TEST);
}

bool GraphicsContextIOSOpenGL::IsBlendingEnabled() const {
    return _blendingEnabled;
}

void GraphicsContextIOSOpenGL::SetBlendingEnabled(bool enabled) {
    _blendingEnabled = enabled;
    auto fun = enabled ? &gl::Enable : &gl::Disable;
    fun(GL_BLEND);
}

bool GraphicsContextIOSOpenGL::IsScissorTestEnabled() const {
    return _scissorTestEnabled;
}

void GraphicsContextIOSOpenGL::SetScissorTestEnabled(bool enabled) {
    _scissorTestEnabled = enabled;
    auto fun = enabled ? &gl::Enable : &gl::Disable;
    fun(GL_SCISSOR_TEST);
}

IntRect GraphicsContextIOSOpenGL::GetScissorRect() const {
    return _scissorRect;
}

void GraphicsContextIOSOpenGL::SetScissorRect(const IntRect& rect) {
    _scissorRect = rect;
    gl::Scissor(rect.x, rect.y, rect.w, rect.h);
}

Color GraphicsContextIOSOpenGL::GetClearColor() const {
    return _clearColor;
}

void GraphicsContextIOSOpenGL::SetClearColor(const Color& color) {
    _clearColor = color;
    gl::ClearColor(color.r, color.g, color.b, color.a);
}

void GraphicsContextIOSOpenGL::SetColorMask(bool red, bool green, bool blue, bool alpha) {
    _colorMask[0] = red;
    _colorMask[1] = green;
    _colorMask[2] = blue;
    _colorMask[3] = alpha;
    gl::ColorMask((GLboolean)red, (GLboolean)green, (GLboolean)blue, (GLboolean)alpha);
}

void GraphicsContextIOSOpenGL::SetBlendFactors(BlendFactor source, BlendFactor dest) {
    _srcColorFactor = source;
    _dstColorFactor = dest;
    _srcAlphaFactor = source;
    _dstAlphaFactor = dest;
    gl::BlendFunc(factors[(int)source], factors[(int)dest]);
}

void GraphicsContextIOSOpenGL::SetBlendFactors(
    BlendFactor sourceColor, BlendFactor destColor,
    BlendFactor sourceAlpha, BlendFactor destAlpha)
{
    _srcColorFactor = sourceColor;
    _dstColorFactor = destColor;
    _srcAlphaFactor = sourceAlpha;
    _dstAlphaFactor = destAlpha;

    gl::BlendFuncSeparate(factors[(int)sourceColor], factors[(int)destColor],
        factors[(int)sourceAlpha], factors[(int)destAlpha]);
}

BlendFactor GraphicsContextIOSOpenGL::GetSourceColorBlendFactor() const {
    return _srcColorFactor;
}

BlendFactor GraphicsContextIOSOpenGL::GetDestColorBlendFactor() const {
    return _dstColorFactor;
}

BlendFactor GraphicsContextIOSOpenGL::GetSourceAlphaBlendFactor() const {
    return _srcAlphaFactor;
}

BlendFactor GraphicsContextIOSOpenGL::GetDestAlphaBlendFactor() const {
    return _dstAlphaFactor;
}

BlendOperation GraphicsContextIOSOpenGL::GetBlendOperation() const {
    return _blendOperation;
}

void GraphicsContextIOSOpenGL::SetBlendOperation(BlendOperation operation)
{
    _blendOperation = operation;
    gl::BlendEquation(ops[(int)operation]);
}

Color GraphicsContextIOSOpenGL::GetBlendColor() const {
    return _blendColor;
}

void GraphicsContextIOSOpenGL::SetBlendColor(Color color)
{
    _blendColor = color;
    gl::BlendColor(color.r, color.g, color.b, color.a);
}

CullMode GraphicsContextIOSOpenGL::GetCullMode() const {
    return _cullMode;
}

void GraphicsContextIOSOpenGL::SetCullMode(CullMode mode) {
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

int GraphicsContextIOSOpenGL::GetSwapInterval() const {
    return _swapInterval;
}

void GraphicsContextIOSOpenGL::SetSwapInterval(int interval) {
    _swapInterval = interval;
}

sptr<RenderTarget> GraphicsContextIOSOpenGL::GetRenderTarget() const {
    return _targetSurface ? (sptr<RenderTarget>)_targetSurface : (sptr<RenderTarget>)_targetTexture;
}

void GraphicsContextIOSOpenGL::SetRenderTarget(const sptr<RenderTarget>& target) {
    target->Bind(shared_from_this());
}

void GraphicsContextIOSOpenGL::SetRenderTarget(const sptr<WindowSurface>& target)
{
    _targetTexture = nullptr;
    _targetSurface = std::dynamic_pointer_cast<WindowSurfaceIOSOpenGL>(target);

    if (_targetSurface)
    {
        gl::BindFramebuffer(GL_FRAMEBUFFER, _targetSurface->frameBuffer);
        gl::BindRenderbuffer(GL_RENDERBUFFER, _targetSurface->colorBuffer);
        
        auto rc = IntRect(0, 0, _targetSurface->window->size.x, _targetSurface->window->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        gl::BindFramebuffer(GL_FRAMEBUFFER, 0);
        gl::BindRenderbuffer(GL_RENDERBUFFER, 0);
        
        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextIOSOpenGL::SetRenderTarget(const sptr<RenderTexture>& target)
{
    _targetSurface = nullptr;
    _targetTexture = std::dynamic_pointer_cast<RenderTextureOpenGL>(target);

    if (_targetTexture)
    {
        gl::BindFramebuffer(GL_FRAMEBUFFER, _targetTexture->frameBuffer);
        gl::BindRenderbuffer(GL_RENDERBUFFER, 0);
        
        auto rc = IntRect(0, 0, _targetTexture->size.x, _targetTexture->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        gl::BindFramebuffer(GL_FRAMEBUFFER, 0);
        gl::BindRenderbuffer(GL_RENDERBUFFER, 0);
        
        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextIOSOpenGL::RebindRenderTarget()
{
    if (_targetSurface)
    {
        gl::BindFramebuffer(GL_FRAMEBUFFER, _targetSurface->frameBuffer);
        gl::BindRenderbuffer(GL_RENDERBUFFER, _targetSurface->colorBuffer);
        
        auto rc = IntRect(0, 0, _targetSurface->window->size.x, _targetSurface->window->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else if (_targetTexture)
    {
        gl::BindFramebuffer(GL_FRAMEBUFFER, _targetTexture->frameBuffer);
        gl::BindRenderbuffer(GL_RENDERBUFFER, 0);
        
        auto rc = IntRect(0, 0, _targetTexture->size.x, _targetTexture->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        gl::BindFramebuffer(GL_FRAMEBUFFER, 0);
        gl::BindRenderbuffer(GL_RENDERBUFFER, 0);
        
        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextIOSOpenGL::Clear(bool depth, bool color)
{
    if (_targetSurface || _targetTexture)
    {
        GLuint flags = 0;
        if (depth) flags |= GL_DEPTH_BUFFER_BIT;
        if (color) flags |= GL_COLOR_BUFFER_BIT;
        gl::Clear(flags);
    }
}

void GraphicsContextIOSOpenGL::Flush()
{
    gl::Flush();
}

void GraphicsContextIOSOpenGL::Flip()
{
    if (_targetSurface)
    {
        gl::BindFramebuffer(GL_FRAMEBUFFER, _targetSurface->frameBuffer);
        gl::BindRenderbuffer(GL_RENDERBUFFER, _targetSurface->colorBuffer);
        gl::Flush();
        [context presentRenderbuffer:GL_RENDERBUFFER];
    }
}

void GraphicsContextIOSOpenGL::DrawArray(int start, int count, DrawMode mode) {
    auto drawMode = drawModes[(int)mode];
    gl::DrawArrays(drawMode, start, count);
}

void GraphicsContextIOSOpenGL::DrawIndexed(int start, int count, DrawMode mode) {
    gl::DrawElements(drawModes[(int)mode], count, GL_UNSIGNED_INT, nullptr);
}

sptr<RenderTexture> GraphicsContextIOSOpenGL::CreateRenderTexture(const Vec2& size) {
    return spnew<RenderTextureOpenGL>(size);
}

void GraphicsContextIOSOpenGL::CreateWindowSurface(const sptr<Window>& window) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextIOSOpenGL>(shared_from_this());
    auto win = std::dynamic_pointer_cast<WindowIOS>(window);
    win->surface = spnew<WindowSurfaceIOSOpenGL>(gfx, win);
}

sptr<Shader> GraphicsContextIOSOpenGL::CreateShader(const std::string& source) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextIOSOpenGL>(shared_from_this());
    return spnew<ShaderOpenGL>(source);
}

sptr<DrawBuffer> GraphicsContextIOSOpenGL::CreateBuffer(uint32_t capacity, DrawBufferType type, bool dynamic, const mw::span<std::byte>& data) {
    return spnew<DrawBufferOpenGL>(capacity, type, dynamic, data);
}

sptr<Texture> GraphicsContextIOSOpenGL::CreateTexture(const IVec2& size, PixelDataFormat format, bool dynamic, const mw::span<std::byte>& data) {
    return spnew<TextureOpenGL>(size, format, dynamic, data);
}

Mat4 GraphicsContextIOSOpenGL::GetOrthoMatrix(float left, float right, float bottom, float top, float znear, float zfar) {
    return Mat4::OrthoOpenGL(left, right, bottom, top, znear, zfar);
}

Mat4 GraphicsContextIOSOpenGL::GetPerspectiveMatrix(float fovY, float aspect, float znear, float zfar) {
    return Mat4::PerspectiveOpenGL(fovY, aspect, znear, zfar);
}

void GraphicsContextIOSOpenGL::AllocateGraphicsResource()
{
    if (_alive)
        return;

    try
    {
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        
        if(!context)
            throw std::runtime_error("failed to create EAGLContext");
        
//        auxContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:[context sharegroup]];
//
//        if(!auxContext)
//            throw std::runtime_error("failed to create secondary EAGLContext");
        
        [EAGLContext setCurrentContext:context];
        
        gl::FrontFace(GL_CW);
        gl::Disable(GL_DITHER);
        
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

void GraphicsContextIOSOpenGL::ReleaseGraphicsResource()
{
    _alive = false;
    _targetSurface = nullptr;
    _targetTexture = nullptr;
    context = nil;
    auxContext = nil;
}

bool GraphicsContextIOSOpenGL::IsGraphicsResourceAllocated() const {
    return _alive;
}

GraphicsDriverType GraphicsContextIOSOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

}
