/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/WindowAndroid.h>
#include <MW/Graphics/GraphicsTypes.h>
#include <MW/Graphics/Internal/GraphicsContextAndroidOpenGL.h>
#include <MW/Graphics/Internal/WindowSurfaceAndroidOpenGL.h>
#include <MW/Graphics/Internal/RenderTextureOpenGL.h>
#include <MW/Graphics/Internal/ShaderOpenGL.h>
#include <MW/Graphics/Internal/DrawBufferOpenGL.h>
#include <MW/Graphics/Internal/TextureOpenGL.h>
#include <stdexcept>
#include <sstream>

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
    
GraphicsContextAndroidOpenGL::GraphicsContextAndroidOpenGL()
{
    _dispatcher = mw::Dispatcher::GetCurrent();
    AllocateGraphicsResource();
}

GraphicsContextAndroidOpenGL::~GraphicsContextAndroidOpenGL()
{
    ReleaseGraphicsResource();
}

sptr<Dispatcher> GraphicsContextAndroidOpenGL::GetDispatcher() {
    return _dispatcher;
}

Vec2 GraphicsContextAndroidOpenGL::GetDepthRangeNDC() {
    return Vec2(-1.0f, 1.0f);
}

void GraphicsContextAndroidOpenGL::MakeCurrent()
{
    // fix to respect currently bound surface

    gl::BindFramebuffer(GL_FRAMEBUFFER, 0);

    if(!eglMakeCurrent(display, dummySurface, dummySurface, context))
        throw std::runtime_error("failed to activate target surface");
}

IntRect GraphicsContextAndroidOpenGL::GetViewport() const {
    return _viewportRect;
}

void GraphicsContextAndroidOpenGL::SetViewport(const IntRect& rect)
{
    _viewportRect = rect;
    gl::Viewport(rect.x, rect.y, rect.w, rect.h);
}

bool GraphicsContextAndroidOpenGL::IsDepthBufferEnabled() const {
    return _depthEnabled;
}

void GraphicsContextAndroidOpenGL::SetDepthBufferEnabled(bool enabled) {
    _depthEnabled = enabled;
    auto fun = enabled ? &gl::Enable : &gl::Disable;
    fun(GL_DEPTH_TEST);
}

bool GraphicsContextAndroidOpenGL::IsBlendingEnabled() const {
    return _blendingEnabled;
}

void GraphicsContextAndroidOpenGL::SetBlendingEnabled(bool enabled) {
    _blendingEnabled = enabled;
    auto fun = enabled ? &gl::Enable : &gl::Disable;
    fun(GL_BLEND);
}

bool GraphicsContextAndroidOpenGL::IsScissorTestEnabled() const {
    return _scissorTestEnabled;
}

void GraphicsContextAndroidOpenGL::SetScissorTestEnabled(bool enabled) {
    _scissorTestEnabled = enabled;
    auto fun = enabled ? &gl::Enable : &gl::Disable;
    fun(GL_SCISSOR_TEST);
}

IntRect GraphicsContextAndroidOpenGL::GetScissorRect() const {
    return _scissorRect;
}

void GraphicsContextAndroidOpenGL::SetScissorRect(const IntRect& rect) {
    _scissorRect = rect;
    gl::Scissor(rect.x, rect.y, rect.w, rect.h);
}

Color GraphicsContextAndroidOpenGL::GetClearColor() const {
    return _clearColor;
}

void GraphicsContextAndroidOpenGL::SetClearColor(const Color& color) {
    _clearColor = color;
    gl::ClearColor(color.r, color.g, color.b, color.a);
}

void GraphicsContextAndroidOpenGL::SetColorMask(bool red, bool green, bool blue, bool alpha) {
    _colorMask[0] = red;
    _colorMask[1] = green;
    _colorMask[2] = blue;
    _colorMask[3] = alpha;
    gl::ColorMask((GLboolean)red, (GLboolean)green, (GLboolean)blue, (GLboolean)alpha);
}

void GraphicsContextAndroidOpenGL::SetBlendFactors(BlendFactor source, BlendFactor dest) {
    _srcColorFactor = source;
    _dstColorFactor = dest;
    _srcAlphaFactor = source;
    _dstAlphaFactor = dest;
    gl::BlendFunc(factors[(int)source], factors[(int)dest]);
}

void GraphicsContextAndroidOpenGL::SetBlendFactors(
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

BlendFactor GraphicsContextAndroidOpenGL::GetSourceColorBlendFactor() const {
    return _srcColorFactor;
}

BlendFactor GraphicsContextAndroidOpenGL::GetDestColorBlendFactor() const {
    return _dstColorFactor;
}

BlendFactor GraphicsContextAndroidOpenGL::GetSourceAlphaBlendFactor() const {
    return _srcAlphaFactor;
}

BlendFactor GraphicsContextAndroidOpenGL::GetDestAlphaBlendFactor() const {
    return _dstAlphaFactor;
}

BlendOperation GraphicsContextAndroidOpenGL::GetBlendOperation() const {
    return _blendOperation;
}

void GraphicsContextAndroidOpenGL::SetBlendOperation(BlendOperation operation)
{
    _blendOperation = operation;
    gl::BlendEquation(ops[(int)operation]);
}

Color GraphicsContextAndroidOpenGL::GetBlendColor() const {
    return _blendColor;
}

void GraphicsContextAndroidOpenGL::SetBlendColor(Color color)
{
    _blendColor = color;
    gl::BlendColor(color.r, color.g, color.b, color.a);
}

CullMode GraphicsContextAndroidOpenGL::GetCullMode() const {
    return _cullMode;
}

void GraphicsContextAndroidOpenGL::SetCullMode(CullMode mode) {
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

int GraphicsContextAndroidOpenGL::GetSwapInterval() const {
    return _swapInterval;
}

void GraphicsContextAndroidOpenGL::SetSwapInterval(int interval) {
    _swapInterval = interval;
}

sptr<RenderTarget> GraphicsContextAndroidOpenGL::GetRenderTarget() const {
    return _targetSurface ? (sptr<RenderTarget>)_targetSurface : (sptr<RenderTarget>)_targetTexture;
}

void GraphicsContextAndroidOpenGL::SetRenderTarget(const sptr<RenderTarget>& target) {
    target->Bind(shared_from_this());
}

void GraphicsContextAndroidOpenGL::SetRenderTarget(const sptr<WindowSurface>& target)
{
    _targetTexture = nullptr;
    _targetSurface = std::dynamic_pointer_cast<WindowSurfaceAndroidOpenGL>(target);

    if (_targetSurface)
    {
        if(!eglMakeCurrent(display, _targetSurface->surface, _targetSurface->surface, context))
            throw std::runtime_error("failed to activate target surface");

        gl::BindFramebuffer(GL_FRAMEBUFFER, 0);

        auto rc = IntRect(0, 0, _targetSurface->window->size.x, _targetSurface->window->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        if(!eglMakeCurrent(display, dummySurface, dummySurface, context))
            throw std::runtime_error("failed to activate target surface");
        
        gl::BindFramebuffer(GL_FRAMEBUFFER, 0);

        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextAndroidOpenGL::SetRenderTarget(const sptr<RenderTexture>& target)
{
    _targetSurface = nullptr;
    _targetTexture = std::dynamic_pointer_cast<RenderTextureOpenGL>(target);

    if (_targetTexture)
    {
        if(!eglMakeCurrent(display, dummySurface, dummySurface, context))
            throw std::runtime_error("failed to activate target surface");

        gl::BindFramebuffer(GL_FRAMEBUFFER, _targetTexture->frameBuffer);
        
        auto rc = IntRect(0, 0, _targetTexture->size.x, _targetTexture->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        if(!eglMakeCurrent(display, dummySurface, dummySurface, context))
            throw std::runtime_error("failed to activate target surface");

        gl::BindFramebuffer(GL_FRAMEBUFFER, 0);

        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextAndroidOpenGL::RebindRenderTarget()
{
    if (_targetSurface)
    {
        if(!eglMakeCurrent(display, _targetSurface->surface, _targetSurface->surface, context))
            throw std::runtime_error("failed to activate target surface");

        gl::BindFramebuffer(GL_FRAMEBUFFER, 0);

        auto rc = IntRect(0, 0, _targetSurface->window->size.x, _targetSurface->window->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else if(_targetTexture)
    {
        if(!eglMakeCurrent(display, dummySurface, dummySurface, context))
            throw std::runtime_error("failed to activate target surface");

        gl::BindFramebuffer(GL_FRAMEBUFFER, _targetTexture->frameBuffer);
        
        auto rc = IntRect(0, 0, _targetTexture->size.x, _targetTexture->size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        if(!eglMakeCurrent(display, dummySurface, dummySurface, context))
            throw std::runtime_error("failed to activate target surface");
        
        gl::BindFramebuffer(GL_FRAMEBUFFER, 0);
        
        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextAndroidOpenGL::Clear(bool depth, bool color)
{
    if (_targetSurface || _targetTexture)
    {
        GLuint flags = 0;
        if (depth) flags |= GL_DEPTH_BUFFER_BIT;
        if (color) flags |= GL_COLOR_BUFFER_BIT;
        gl::Clear(flags);
    }
}

void GraphicsContextAndroidOpenGL::Flush()
{
    gl::Flush();
}

void GraphicsContextAndroidOpenGL::Flip()
{
    if (_targetSurface)
    {
        //gl::Flush();
        eglSwapBuffers(display, _targetSurface->surface);
    }
}

void GraphicsContextAndroidOpenGL::DrawArray(int start, int count, DrawMode mode) {
    auto drawMode = drawModes[(int)mode];
    gl::DrawArrays(drawMode, start, count);
}

void GraphicsContextAndroidOpenGL::DrawIndexed(int start, int count, DrawMode mode) {
    gl::DrawElements(drawModes[(int)mode], count, GL_UNSIGNED_INT, nullptr);
}

sptr<RenderTexture> GraphicsContextAndroidOpenGL::CreateRenderTexture(const Vec2& size) {
    return spnew<RenderTextureOpenGL>(size);
}

void GraphicsContextAndroidOpenGL::CreateWindowSurface(const sptr<Window>& window) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextAndroidOpenGL>(shared_from_this());
    auto win = std::dynamic_pointer_cast<WindowAndroid>(window);
    win->surface = spnew<WindowSurfaceAndroidOpenGL>(gfx, win);
}

sptr<Shader> GraphicsContextAndroidOpenGL::CreateShader(const std::string& source) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextAndroidOpenGL>(shared_from_this());
    return spnew<ShaderOpenGL>(source);
}

sptr<DrawBuffer> GraphicsContextAndroidOpenGL::CreateBuffer(uint32_t capacity, DrawBufferType type, bool dynamic, const mw::span<std::byte>& data) {
    return spnew<DrawBufferOpenGL>(capacity, type, dynamic, data);
}

sptr<Texture> GraphicsContextAndroidOpenGL::CreateTexture(const IVec2& size, PixelDataFormat format, bool dynamic, const mw::span<std::byte>& data) {
    return spnew<TextureOpenGL>(size, format, dynamic, data);
}

Mat4 GraphicsContextAndroidOpenGL::GetOrthoMatrix(float left, float right, float bottom, float top, float znear, float zfar) {
    return Mat4::OrthoOpenGL(left, right, bottom, top, znear, zfar);
}

Mat4 GraphicsContextAndroidOpenGL::GetPerspectiveMatrix(float fovY, float aspect, float znear, float zfar) {
    return Mat4::PerspectiveOpenGL(fovY, aspect, znear, zfar);
}

EGLConfig ChooseConfig(EGLDisplay display)
{
    EGLint numConfigs = 0;

    if(!eglGetConfigs(display, nullptr, 0, &numConfigs) || numConfigs == 0)
        throw std::runtime_error("no graphics configurations available for this device");

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

void GraphicsContextAndroidOpenGL::AllocateGraphicsResource()
{
    if (_alive)
        return;

    try
    {
        // create context
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if(display == EGL_NO_DISPLAY)
            throw std::runtime_error("failed to get default display");

        if(!eglInitialize(display, 0, 0))
            throw std::runtime_error("failed to initialize display");

        config = ChooseConfig(display);
        if(!config)
            throw std::runtime_error("supported display configuration not found");

        EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
        if(context == EGL_NO_CONTEXT)
            throw std::runtime_error("failed to create EGL context");

        EGLint dummySurfaceAttribs[] = {
         	EGL_WIDTH, 1,
         	EGL_HEIGHT, 1,
         	EGL_TEXTURE_TARGET, EGL_NO_TEXTURE,
         	EGL_TEXTURE_FORMAT, EGL_NO_TEXTURE,
         	EGL_NONE
        };

        dummySurface = eglCreatePbufferSurface(display, config, dummySurfaceAttribs);
        if(dummySurface == EGL_NO_SURFACE)
             throw std::runtime_error("failed to create dummy surface for context");

        if(!eglMakeCurrent(display, dummySurface, dummySurface, context))
            throw std::runtime_error("failed to activate graphics context");

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

void GraphicsContextAndroidOpenGL::ReleaseGraphicsResource()
{
    if(display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglTerminate(display);
    }

    if(context != EGL_NO_CONTEXT)
        eglDestroyContext(display, context);

    if(dummySurface != EGL_NO_SURFACE)
        eglDestroySurface(display, dummySurface);

    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    dummySurface = EGL_NO_SURFACE;
    _targetSurface = nullptr;
    _targetTexture = nullptr;
    _alive = false;
}

bool GraphicsContextAndroidOpenGL::IsGraphicsResourceAllocated() const {
    return _alive;
}

GraphicsDriverType GraphicsContextAndroidOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

}
