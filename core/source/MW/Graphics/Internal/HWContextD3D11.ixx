/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWContextD3D11;
import Microwave.Graphics.Internal.HWBufferD3D11;
import Microwave.Graphics.Internal.HWContext;
import Microwave.Graphics.Internal.HWShaderD3D11;
import Microwave.Graphics.Internal.HWTextureD3D11;
import Microwave.Graphics.Internal.HWRenderTextureD3D11;
import Microwave.Graphics.Internal.HWSurfaceD3D11;
import Microwave.Graphics.RenderTarget;
import Microwave.Graphics.GraphicsTypes;
import Microwave.System.Pointers;
import Microwave.System.Internal.WindowWindows;
import <MW/System/Internal/PlatformHeaders.h>;

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

export namespace mw {
inline namespace gfx {

class HWContextD3D11 : public HWContext
{
public:
    D3D_FEATURE_LEVEL mFeatureLevel = D3D_FEATURE_LEVEL_11_0;
    ComPtr<ID3D11Device1> device;
    ComPtr<ID3D11DeviceContext1> deviceContext;
    
    // blend
    ComPtr<ID3D11BlendState> blendState;
    D3D11_RENDER_TARGET_BLEND_DESC targetBlendDesc {
        .BlendEnable = false,
        .SrcBlend = D3D11_BLEND_ONE,
        .DestBlend = D3D11_BLEND_ZERO,
        .BlendOp = D3D11_BLEND_OP_ADD,
        .SrcBlendAlpha = D3D11_BLEND_ONE,
        .DestBlendAlpha = D3D11_BLEND_ZERO,
        .BlendOpAlpha = D3D11_BLEND_OP_ADD,
        .RenderTargetWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO
    };
    Color blendColor = Color::White();
    bool blendStateDirty = true;

    // rasterizer
    ComPtr<ID3D11RasterizerState> rasterizerState;
    D3D11_CULL_MODE cullMode = D3D11_CULL_BACK;
    bool scissorTestEnabled = false;
    bool rasterizerStateDirty = true;

    // depth stencil
    ComPtr<ID3D11DepthStencilState> depthStencilState;
    bool depthTestEnabled = false;
    D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS_EQUAL;
    bool depthStencilStateDirty = true;

    // other
    int swapInterval = 0;
    Color clearColor;
    sptr<HWTexture> defaultTexture;

    HWContextD3D11();
    virtual ~HWContextD3D11();

    virtual void SetActive(bool active) override;

    virtual Vec2 GetDepthRangeNDC() const override;
    virtual void SetViewport(const IntRect& rect) override;
    virtual void SetRenderTarget(const sptr<HWRenderTarget>& target) override;
    virtual void Flip(const sptr<HWRenderTarget>& target) override;
    virtual void SetSwapInterval(int interval) override;
    virtual void Clear(const sptr<HWRenderTarget>& target, bool depth, bool color) override;
    virtual void SetClearColor(const Color& color) override;
    virtual void SetCullMode(CullMode mode) override;
    virtual void SetScissorTestEnabled(bool enabled) override;
    virtual void SetScissorRect(const IntRect& rect) override;
    virtual void SetDepthTest(DepthTest test) override;
    virtual void SetDepthWriteEnabled(bool enabled) override;
    virtual void SetBlendingEnabled(bool enabled) override;

    virtual void SetBlendOperations(
        BlendOperation colorBlendOp, BlendOperation alphaBlendOp) override;

    virtual void SetBlendFactors(
        BlendFactor sourceColor, BlendFactor destColor,
        BlendFactor sourceAlpha, BlendFactor destAlpha) override;

    virtual void SetColorMask(bool red, bool green, bool blue, bool alpha) override;
    virtual void SetBlendColor(Color color) override;
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

    virtual sptr<HWShader> CreateShader(
        const sptr<ShaderInfo>& info) override;

    virtual sptr<HWRenderTexture> CreateRenderTexture(
        const sptr<HWTexture>& tex) override;

    virtual sptr<HWBuffer> CreateBuffer(
        BufferType type, BufferUsage usage,
        BufferCPUAccess cpuAccess, size_t size) override;

    virtual sptr<HWBuffer> CreateBuffer(
        BufferType type, BufferUsage usage,
        BufferCPUAccess cpuAccess,
        const std::span<std::byte>& data) override;

    virtual sptr<HWSurface> CreateSurface(
        const sptr<Window>& window) override;

    virtual sptr<HWTexture> CreateTexture(
        const IVec2& size, PixelDataFormat format, bool dynamic,
        const std::span<std::byte>& data) override;

    virtual sptr<HWTexture> CreateTexture(
        const IVec2& size, PixelDataFormat format, bool dynamic,
        const sptr<HWBuffer>& buffer) override;

    virtual sptr<HWTexture> GetDefaultTexture() override;

    void UpdateDeviceStates();
};

} // gfx
} // mw
