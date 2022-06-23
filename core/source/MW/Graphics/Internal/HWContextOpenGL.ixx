/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWContextOpenGL;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.Internal.HWBufferOpenGL;
import Microwave.Graphics.Internal.HWContext;
import Microwave.Graphics.Internal.HWDriverContext;
import Microwave.Graphics.Internal.HWRenderTarget;
import Microwave.Graphics.Internal.HWShader;
import Microwave.Graphics.Internal.HWSurface;
import Microwave.Graphics.Internal.HWTextureOpenGL;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.System.Pointers;
import Microwave.System.Internal.WindowWindows;
import <MW/System/Internal/PlatformHeaders.h>;
import <unordered_map>;

export namespace mw {
inline namespace gfx {

class HWContextOpenGL : public HWContext
{
public:
    gptr<HWDriverContext> driverContext;
    bool depthWriteEnabled = false;
    gptr<HWTexture> defaultTexture;

    HWContextOpenGL();
    virtual ~HWContextOpenGL(){}

    virtual void SetActive(bool active) override;

    virtual Vec2 GetDepthRangeNDC() const override;
    virtual void SetRenderTarget(const gptr<HWRenderTarget>& target) override;
    virtual void Flip(const gptr<HWRenderTarget>& target) override;
    virtual void SetSwapInterval(int interval) override;
    virtual void SetCullMode(CullMode mode) override;
    virtual void SetScissorTestEnabled(bool enabled) override;
    virtual void SetScissorRect(const IntRect& rect) override;
    virtual void SetDepthTest(DepthTest test) override;
    virtual void SetDepthWriteEnabled(bool enabled) override;
    virtual void SetBlendingEnabled(bool enabled) override;
    virtual void SetClearColor(const Color& color) override;

    virtual void SetBlendOperations(
        BlendOperation colorBlendOp, BlendOperation alphaBlendOp) override;

    virtual void SetBlendFactors(
        BlendFactor sourceColor, BlendFactor destColor,
        BlendFactor sourceAlpha, BlendFactor destAlpha) override;

    virtual void SetColorMask(
        bool red,
        bool green,
        bool blue,
        bool alpha) override;

    virtual void SetBlendColor(Color color) override;
    virtual void SetViewport(const IntRect& rect) override;
    virtual void Clear(const gptr<HWRenderTarget>& target, bool depth, bool color) override;
    virtual void DrawArray(int start, int count, DrawMode mode) override;
    virtual void DrawIndexed(int start, int count, DrawMode mode) override;
    virtual void Flush() override;

    virtual Mat4 GetOrthoMatrix(
        float left, float right,
        float bottom, float top,
        float znear, float zfar) override;

    virtual Mat4 GetPerspectiveMatrix(
        float fovY, float aspect,
        float znear, float zfar) override;

    virtual ShaderLanguage GetShaderLanguage() const override;

    virtual gptr<HWShader> CreateShader(
        const gptr<ShaderInfo>& info) override;
    
    virtual gptr<HWRenderTexture> CreateRenderTexture(
        const gptr<HWTexture>& tex) override;

    virtual gptr<HWBuffer> CreateBuffer(
        BufferType type, BufferUsage usage,
        BufferCPUAccess cpuAccess, size_t size) override;

    virtual gptr<HWBuffer> CreateBuffer(
        BufferType type, BufferUsage usage,
        BufferCPUAccess cpuAccess,
        const std::span<std::byte>& data) override;

    virtual gptr<HWSurface> CreateSurface(
        const gptr<Window>& window) override;

    virtual gptr<HWTexture> CreateTexture(
        const IVec2& size, PixelDataFormat format, bool dynamic,
        const std::span<std::byte>& data) override;

    virtual gptr<HWTexture> CreateTexture(
        const IVec2& size, PixelDataFormat format, bool dynamic,
        const gptr<HWBuffer>& buffer) override;

    virtual gptr<HWTexture> GetDefaultTexture() override;
};

} // gfx
} // mw
