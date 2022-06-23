/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWContext;
import Microwave.Graphics.Color;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.ShaderInfo;
import Microwave.Graphics.Internal.HWBuffer;
import Microwave.Graphics.Internal.HWRenderTarget;
import Microwave.Graphics.Internal.HWRenderTexture;
import Microwave.Graphics.Internal.HWShader;
import Microwave.Graphics.Internal.HWSurface;
import Microwave.Graphics.Internal.HWTexture;
import Microwave.Math;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.System.Window;

export namespace mw {
inline namespace gfx {

class HWContext : public Object
{
public:
    HWContext() = default;
    virtual ~HWContext() = default;

    virtual void SetActive(bool active) = 0;

    virtual Vec2 GetDepthRangeNDC() const = 0;
    virtual void SetClearColor(const Color& color) = 0;
    virtual void SetCullMode(CullMode mode) = 0;
    virtual void SetScissorTestEnabled(bool enabled) = 0;
    virtual void SetScissorRect(const IntRect& rect) = 0;
    virtual void SetDepthTest(DepthTest test) = 0;
    virtual void SetDepthWriteEnabled(bool enabled) = 0;
    virtual void SetBlendingEnabled(bool enabled) = 0;
    virtual void SetBlendOperations(
        BlendOperation colorBlendOp, BlendOperation alphaBlendOp) = 0;
    virtual void SetBlendFactors(
        BlendFactor sourceColor, BlendFactor destColor,
        BlendFactor sourceAlpha, BlendFactor destAlpha) = 0;
    virtual void SetColorMask(bool red, bool green, bool blue, bool alpha) = 0;
    virtual void SetBlendColor(Color color) = 0;
    virtual void SetViewport(const IntRect& rect) = 0;
    virtual void SetRenderTarget(const gptr<HWRenderTarget>& target) = 0;
    virtual void SetSwapInterval(int interval) = 0;
    virtual void Clear(const gptr<HWRenderTarget>& target, bool depth, bool color) = 0;
    virtual void DrawArray(int start, int count, DrawMode mode) = 0;
    virtual void DrawIndexed(int start, int count, DrawMode mode) = 0;
    virtual void Flush() = 0;
    virtual void Flip(const gptr<HWRenderTarget>& target) = 0;
    virtual Mat4 GetOrthoMatrix(float left, float right, float bottom, float top, float znear, float zfar) = 0;
    virtual Mat4 GetPerspectiveMatrix(float fovY, float aspect, float znear, float zfar) = 0;

    virtual ShaderLanguage GetShaderLanguage() const = 0;

    virtual gptr<HWShader> CreateShader(const gptr<ShaderInfo>& info) = 0;
    virtual gptr<HWRenderTexture> CreateRenderTexture(const gptr<HWTexture>& tex) = 0;
    virtual gptr<HWBuffer> CreateBuffer(BufferType type, BufferUsage usage, BufferCPUAccess cpuAccess, size_t size) = 0;
    virtual gptr<HWBuffer> CreateBuffer(BufferType type, BufferUsage usage, BufferCPUAccess cpuAccess, const std::span<std::byte>& data) = 0;
    virtual gptr<HWSurface> CreateSurface(const gptr<Window>& window) = 0;
    virtual gptr<HWTexture> CreateTexture(const IVec2& size, PixelDataFormat format, bool dynamic, const std::span<std::byte>& data) = 0;
    virtual gptr<HWTexture> CreateTexture(const IVec2& size, PixelDataFormat format, bool dynamic, const gptr<HWBuffer>& buffer) = 0;
    virtual gptr<HWTexture> GetDefaultTexture() = 0;
};

} // gfx
} // mw
