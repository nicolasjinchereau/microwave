/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/GraphicsTypes.h>
#include <MW/Graphics/GraphicsResource.h>
#include <MW/Graphics/Color.h>
#include <MW/System/Pointers.h>
#include <MW/System/Window.h>
#include <MW/Math/Math.h>
#include <cstddef>
#include <cstdlib>
#include <span>
#include <vector>

namespace mw {

inline namespace system {
class App;
}

inline namespace gfx {

class RenderTarget;
class RenderTexture;
class WindowSurface;
class Shader;
class DrawBuffer;
class Texture;
enum class DrawBufferType : int;

class GraphicsContext : public GraphicsResource
{
    friend class mw::system::App;
public:
    static sptr<GraphicsContext> New(GraphicsDriverType type);
    virtual ~GraphicsContext(){}

    virtual sptr<Dispatcher> GetDispatcher() = 0;

    virtual Vec2 GetDepthRangeNDC() = 0;

    virtual void MakeCurrent() = 0;
    
    virtual IntRect GetViewport() const = 0;
    virtual void SetViewport(const IntRect& rect) = 0;

    virtual bool IsBlendingEnabled() const = 0;
    virtual void SetBlendingEnabled(bool enabled) = 0;

    virtual bool IsScissorTestEnabled() const = 0;
    virtual void SetScissorTestEnabled(bool enabled) = 0;

    virtual IntRect GetScissorRect() const = 0;
    virtual void SetScissorRect(const IntRect& rect) = 0;

    virtual Color GetClearColor() const = 0;
    virtual void SetClearColor(const Color& color) = 0;

    virtual void SetColorMask(bool red, bool green, bool blue, bool alpha) = 0;
    
    virtual void SetBlendFactors(BlendFactor source, BlendFactor dest) = 0;
    virtual void SetBlendFactors(
        BlendFactor sourceColor, BlendFactor destColor,
        BlendFactor sourceAlpha, BlendFactor destAlpha) = 0;

    virtual BlendFactor GetSourceColorBlendFactor() const = 0;
    virtual BlendFactor GetDestColorBlendFactor() const = 0;
    virtual BlendFactor GetSourceAlphaBlendFactor() const = 0;
    virtual BlendFactor GetDestAlphaBlendFactor() const = 0;
    
    virtual void SetBlendOperations(BlendOperation blendOp) = 0;
    virtual void SetBlendOperations(BlendOperation colorBlendOp, BlendOperation alphaBlendOp) = 0;

    virtual BlendOperation GetColorBlendOperation() const = 0;
    virtual BlendOperation GetAlphaBlendOperation() const = 0;

    virtual Color GetBlendColor() const = 0;
    virtual void SetBlendColor(Color color) = 0;

    virtual CullMode GetCullMode() const = 0;
    virtual void SetCullMode(CullMode mode) = 0;

    virtual DepthTest GetDepthTest() const = 0;
    virtual void SetDepthTest(DepthTest test) = 0;

    virtual bool IsDepthWriteEnabled() const = 0;
    virtual void SetDepthWriteEnabled(bool enabled) = 0;

    virtual int GetSwapInterval() const = 0;
    virtual void SetSwapInterval(int interval) = 0;

    virtual sptr<RenderTarget> GetRenderTarget() const = 0;
    virtual void SetRenderTarget(const sptr<RenderTarget>& target) = 0;
    virtual void SetRenderTarget(const sptr<WindowSurface>& target) = 0;
    virtual void SetRenderTarget(const sptr<RenderTexture>& target) = 0;
    virtual void RebindRenderTarget() = 0;

    virtual void Clear(bool depth, bool color) = 0;
    virtual void Flush() = 0;
    virtual void Flip() = 0;

    virtual void DrawArray(int start, int count, DrawMode mode) = 0;
    virtual void DrawIndexed(int start, int count, DrawMode mode) = 0;

    virtual sptr<RenderTexture> CreateRenderTexture(const Vec2& size) = 0;
    virtual void CreateWindowSurface(const sptr<Window>& window) = 0;
    virtual sptr<Shader> CreateShader(const std::string& source) = 0;

    // empty span ok for `data` for dynamic buffers
    virtual sptr<DrawBuffer> CreateBuffer(uint32_t capacity, DrawBufferType type, bool dynamic, const std::span<std::byte>& data) = 0;

    virtual sptr<Texture> CreateTexture(const IVec2& size, PixelDataFormat format, bool dynamic, const std::span<std::byte>& data) = 0;

    virtual Mat4 GetOrthoMatrix(float left, float right, float bottom, float top, float zNear, float zFar) = 0;
    virtual Mat4 GetPerspectiveMatrix(float fovY, float aspect, float zNear, float zFar) = 0;
};

}
}
