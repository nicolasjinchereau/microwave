/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Pointers.h>
#include <MW/System/Dispatcher.h>
#include <MW/Graphics/GraphicsContext.h>
#include <MW/Graphics/GraphicsTypes.h>
#include <MW/Graphics/Graphics.h>
#include <MW/Graphics/Internal/OpenGLAPI.h>

namespace mw::gfx {

class WindowSurfaceAndroidOpenGL;
class RenderTextureOpenGL;
class RenderTarget;

class GraphicsContextAndroidOpenGL : public GraphicsContext
{
public:
    sptr<Dispatcher> _dispatcher;
    sptr<WindowSurfaceAndroidOpenGL> _targetSurface;
    sptr<RenderTextureOpenGL> _targetTexture;
    IntRect _viewportRect = IntRect();
    IntRect _scissorRect = IntRect();
    Color _clearColor = Color::Clear();
    bool _depthEnabled = true;
    bool _scissorTestEnabled = false;
    bool _blendingEnabled = false;
    BlendFactor _srcColorFactor = BlendFactor::One;
    BlendFactor _dstColorFactor = BlendFactor::Zero;
    BlendFactor _srcAlphaFactor = BlendFactor::One;
    BlendFactor _dstAlphaFactor = BlendFactor::Zero;
    BlendOperation _blendOperation = BlendOperation::Add;
    Color _blendColor = Color::Clear();
    CullMode _cullMode = CullMode::Back;
    bool _colorMask[4] = { true, true, true, true };
    int _swapInterval = 0;
    
    bool _alive = false;

    EGLConfig config = nullptr;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLContext context = EGL_NO_CONTEXT;
    EGLSurface dummySurface = EGL_NO_SURFACE;

    GraphicsContextAndroidOpenGL();
    ~GraphicsContextAndroidOpenGL();

    virtual sptr<Dispatcher> GetDispatcher() override;

    virtual Vec2 GetDepthRangeNDC() override;

    virtual void MakeCurrent() override;
    
    virtual IntRect GetViewport() const override;
    virtual void SetViewport(const IntRect& rect) override;

    virtual bool IsDepthBufferEnabled() const override;
    virtual void SetDepthBufferEnabled(bool enabled) override;

    virtual bool IsBlendingEnabled() const override;
    virtual void SetBlendingEnabled(bool enabled) override;

    virtual bool IsScissorTestEnabled() const override;
    virtual void SetScissorTestEnabled(bool enabled) override;

    virtual IntRect GetScissorRect() const override;
    virtual void SetScissorRect(const IntRect& rect) override;

    virtual Color GetClearColor() const override;
    virtual void SetClearColor(const Color& color) override;

    virtual void SetColorMask(bool red, bool green, bool blue, bool alpha) override;

    virtual void SetBlendFactors(BlendFactor source, BlendFactor dest) override;
    virtual void SetBlendFactors(
        BlendFactor sourceColor, BlendFactor destColor,
        BlendFactor sourceAlpha, BlendFactor destAlpha) override;

    virtual BlendFactor GetSourceColorBlendFactor() const override;
    virtual BlendFactor GetDestColorBlendFactor() const override;
    virtual BlendFactor GetSourceAlphaBlendFactor() const override;
    virtual BlendFactor GetDestAlphaBlendFactor() const override;

    virtual BlendOperation GetBlendOperation() const override;
    virtual void SetBlendOperation(BlendOperation operation) override;

    virtual Color GetBlendColor() const override;
    virtual void SetBlendColor(Color color) override;

    virtual CullMode GetCullMode() const override;
    virtual void SetCullMode(CullMode mode) override;

    virtual int GetSwapInterval() const override;
    virtual void SetSwapInterval(int interval) override;

    virtual sptr<RenderTarget> GetRenderTarget() const override;
    virtual void SetRenderTarget(const sptr<RenderTarget>& target) override;
    virtual void SetRenderTarget(const sptr<WindowSurface>& target) override;
    virtual void SetRenderTarget(const sptr<RenderTexture>& target) override;
    virtual void RebindRenderTarget() override;

    virtual void Clear(bool depth, bool color) override;
    virtual void Flush() override;
    virtual void Flip() override;

    virtual void DrawArray(int start, int count, DrawMode mode) override;
    virtual void DrawIndexed(int start, int count, DrawMode mode) override;

    virtual void CreateWindowSurface(const sptr<Window>& window) override;
    virtual sptr<Shader> CreateShader(const std::string& source) override;
    virtual sptr<DrawBuffer> CreateBuffer(uint32_t capacity, DrawBufferType type, bool dynamic, const mw::span<std::byte>& data) override;
    virtual sptr<Texture> CreateTexture(const IVec2& size, PixelDataFormat format, bool dynamic, const mw::span<std::byte>& data) override;
    
    virtual Mat4 GetOrthoMatrix(float left, float right, float bottom, float top, float znear, float zfar) override;
    virtual Mat4 GetPerspectiveMatrix(float fovY, float aspect, float znear, float zfFar) override;

    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
};

}
