/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWContextD3D11;
import Microwave.Graphics.ShaderInfo;
import <unordered_map>;
import <cassert>;

namespace mw {
inline namespace gfx {

std::unordered_map<BlendFactor, D3D11_BLEND> blendFactors = {
    { BlendFactor::Zero, D3D11_BLEND_ZERO },
    { BlendFactor::One, D3D11_BLEND_ONE },
    { BlendFactor::SrcColor, D3D11_BLEND_SRC_COLOR },
    { BlendFactor::OneMinusSrcColor, D3D11_BLEND_INV_SRC_COLOR },
    { BlendFactor::DstColor, D3D11_BLEND_DEST_COLOR },
    { BlendFactor::OneMinusDstColor, D3D11_BLEND_INV_DEST_COLOR },
    { BlendFactor::SrcAlpha, D3D11_BLEND_SRC_ALPHA },
    { BlendFactor::OneMinusSrcAlpha, D3D11_BLEND_INV_SRC_ALPHA },
    { BlendFactor::DstAlpha, D3D11_BLEND_DEST_ALPHA },
    { BlendFactor::OneMinusDstAlpha, D3D11_BLEND_INV_DEST_ALPHA },
    { BlendFactor::SrcAlphaSaturate, D3D11_BLEND_SRC_ALPHA_SAT },
    { BlendFactor::ConstColor, D3D11_BLEND_BLEND_FACTOR },
    { BlendFactor::OneMinusConstColor, D3D11_BLEND_INV_BLEND_FACTOR },
    { BlendFactor::Src1Color, D3D11_BLEND_SRC1_COLOR },
    { BlendFactor::OneMinusSrc1Color, D3D11_BLEND_INV_SRC1_COLOR },
    { BlendFactor::Src1Alpha, D3D11_BLEND_SRC1_ALPHA },
    { BlendFactor::OneMinusSrc1Alpha, D3D11_BLEND_INV_SRC1_ALPHA }
};

std::unordered_map<BlendOperation, D3D11_BLEND_OP> blendOperations = {
    { BlendOperation::Add, D3D11_BLEND_OP_ADD },
    { BlendOperation::Subtract, D3D11_BLEND_OP_SUBTRACT },
    { BlendOperation::ReverseSubtract, D3D11_BLEND_OP_REV_SUBTRACT },
    { BlendOperation::Min, D3D11_BLEND_OP_MIN },
    { BlendOperation::Max, D3D11_BLEND_OP_MAX }
};

std::unordered_map<DrawMode, D3D11_PRIMITIVE_TOPOLOGY> primitiveTopologies = {
    { DrawMode::Points, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST },
    { DrawMode::Lines, D3D11_PRIMITIVE_TOPOLOGY_LINELIST },
    { DrawMode::LineStrip, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP },
    { DrawMode::Triangles, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST },
    { DrawMode::TriangleStrip, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP }
};

std::unordered_map<CullMode, D3D11_CULL_MODE> cullModes = {
    { CullMode::Off, D3D11_CULL_NONE },
    { CullMode::Back, D3D11_CULL_BACK },
    { CullMode::Front, D3D11_CULL_FRONT },
};

std::unordered_map<DepthTest, std::pair<bool, D3D11_COMPARISON_FUNC>> depthFuncs = {
    { DepthTest::Always, { false, D3D11_COMPARISON_ALWAYS } },
    { DepthTest::Never, { true, D3D11_COMPARISON_NEVER } },
    { DepthTest::Less, { true, D3D11_COMPARISON_LESS } },
    { DepthTest::Equal, { true, D3D11_COMPARISON_EQUAL } },
    { DepthTest::LessOrEqual, { true, D3D11_COMPARISON_LESS_EQUAL } },
    { DepthTest::Greater, { true, D3D11_COMPARISON_GREATER } },
    { DepthTest::NotEqual, { true, D3D11_COMPARISON_NOT_EQUAL } },
    { DepthTest::GreaterOrEqual, { true, D3D11_COMPARISON_GREATER_EQUAL } },
};

HWContextD3D11::HWContextD3D11()
{
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    //creationFlags &= ~D3D11_CREATE_DEVICE_SINGLETHREADED;

#ifndef NDEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
    };

    ComPtr<ID3D11Device> tmpDevice;
    ComPtr<ID3D11DeviceContext> tmpContext;

    HRESULT res;

    res = D3D11CreateDevice(nullptr,
        D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags,
        featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
        &tmpDevice, &mFeatureLevel, &tmpContext
    );
    if(FAILED(res)) throw std::runtime_error("failed to create d3d11 device");

    res = tmpDevice.As(&device);
    if(FAILED(res)) throw std::runtime_error("failed to get d3d11 device interface");

    res = tmpContext.As(&deviceContext);
    if(FAILED(res)) throw std::runtime_error("failed to get d3d11 device context interface");

    blendStateDirty = true;
    rasterizerStateDirty = true;
    depthStencilStateDirty = true;

    UpdateDeviceStates();
}

HWContextD3D11::~HWContextD3D11()
{
    blendState = nullptr;
    rasterizerState = nullptr;
    depthStencilState = nullptr;
    deviceContext = nullptr;
    device = nullptr;
}

void HWContextD3D11::SetActive(bool active)
{
}

Vec2 HWContextD3D11::GetDepthRangeNDC() const {
    return Vec2(0.0f, 1.0f);
}

void HWContextD3D11::SetViewport(const IntRect& rect)
{
    CD3D11_VIEWPORT viewport((float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h);
    deviceContext->RSSetViewports(1, &viewport);
}

void HWContextD3D11::SetRenderTarget(const sptr<HWRenderTarget>& target)
{
    if (auto surf = spcast<HWSurfaceD3D11>(target))
    {
        deviceContext->OMSetRenderTargets(
            1,
            surf->renderTargetView.GetAddressOf(),
            surf->depthStencilView.Get());
    }
    else if (auto tex = spcast<HWRenderTextureD3D11>(target))
    {
        deviceContext->OMSetRenderTargets(
            1,
            tex->renderTargetView.GetAddressOf(),
            tex->depthStencilView.Get());
    }
    else
    {
        deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    }
}

void HWContextD3D11::Flip(const sptr<HWRenderTarget>& target)
{
    if (auto surf = spcast<HWSurfaceD3D11>(target))
    {
        surf->swapChain->Present(swapInterval, 0);
        
        deviceContext->OMSetRenderTargets(
            1,
            surf->renderTargetView.GetAddressOf(),
            surf->depthStencilView.Get());
    }
}

void HWContextD3D11::SetSwapInterval(int interval) {
    swapInterval = interval;
}

void HWContextD3D11::Clear(const sptr<HWRenderTarget>& target, bool depth, bool color)
{
    if (auto surf = spcast<HWSurfaceD3D11>(target)) {
        if (color) deviceContext->ClearRenderTargetView(surf->renderTargetView.Get(), clearColor);
        if (depth) deviceContext->ClearDepthStencilView(surf->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
    else if (auto tex = spcast<HWRenderTextureD3D11>(target)) {
        if (color) deviceContext->ClearRenderTargetView(tex->renderTargetView.Get(), clearColor);
        if (depth) deviceContext->ClearDepthStencilView(tex->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
}

void HWContextD3D11::SetClearColor(const Color& color) {
    clearColor = color;
}

void HWContextD3D11::SetCullMode(CullMode mode) {
    cullMode = cullModes[mode];
    rasterizerStateDirty = true;
}

void HWContextD3D11::SetScissorTestEnabled(bool enabled) {
    scissorTestEnabled = enabled;
    rasterizerStateDirty = true;
}

void HWContextD3D11::SetScissorRect(const IntRect& rect)
{
    D3D11_RECT rc;
    rc.left = rect.x;
    rc.top = rect.x;
    rc.right = rect.x + rect.w;
    rc.bottom = rect.y + rect.h;
    deviceContext->RSSetScissorRects(1, &rc);
}

void HWContextD3D11::SetDepthTest(DepthTest test)
{
    auto& [enabled, func] = depthFuncs[test];
    depthTestEnabled = enabled;
    depthFunc = func;
    depthStencilStateDirty = true;
}

void HWContextD3D11::SetDepthWriteEnabled(bool enabled)
{
    depthWriteMask = enabled ?
        D3D11_DEPTH_WRITE_MASK_ALL :
        D3D11_DEPTH_WRITE_MASK_ZERO;

    depthStencilStateDirty = true;
}

void HWContextD3D11::SetBlendingEnabled(bool enabled)
{
    targetBlendDesc.BlendEnable = enabled;
    blendStateDirty = true;
}

void HWContextD3D11::SetBlendOperations(
    BlendOperation colorBlendOp, BlendOperation alphaBlendOp)
{
    targetBlendDesc.BlendOp = blendOperations[colorBlendOp];
    targetBlendDesc.BlendOpAlpha = blendOperations[alphaBlendOp];
    blendStateDirty = true;
}

void HWContextD3D11::SetBlendFactors(
    BlendFactor sourceColor, BlendFactor destColor,
    BlendFactor sourceAlpha, BlendFactor destAlpha)
{
    targetBlendDesc.SrcBlend = blendFactors[sourceColor];
    targetBlendDesc.DestBlend = blendFactors[destColor];
    targetBlendDesc.SrcBlendAlpha = blendFactors[sourceAlpha];
    targetBlendDesc.DestBlendAlpha = blendFactors[destAlpha];
    blendStateDirty = true;
}

void HWContextD3D11::SetColorMask(bool red, bool green, bool blue, bool alpha)
{
    UINT8 writeMask = 0;
    if(red)   writeMask |= D3D11_COLOR_WRITE_ENABLE_RED;
    if(green) writeMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
    if(blue)  writeMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
    if(alpha) writeMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
    targetBlendDesc.RenderTargetWriteMask = writeMask;
    blendStateDirty = true;
}

void HWContextD3D11::SetBlendColor(Color color)
{
    blendColor = color;
    assert(blendState.Get());
    deviceContext->OMSetBlendState(blendState.Get(), blendColor, 0xFFFFFFFF);
}

void HWContextD3D11::DrawArray(int start, int count, DrawMode mode)
{
    UpdateDeviceStates();
    deviceContext->IASetPrimitiveTopology(primitiveTopologies[mode]);
    deviceContext->Draw(count, start);
}

void HWContextD3D11::DrawIndexed(int start, int count, DrawMode mode)
{
    UpdateDeviceStates();
    deviceContext->IASetPrimitiveTopology(primitiveTopologies[mode]);
    deviceContext->DrawIndexed(count, start, 0);
}

void HWContextD3D11::Flush() {
    deviceContext->Flush();
}

Mat4 HWContextD3D11::GetOrthoMatrix(
    float left, float right,
    float bottom, float top,
    float znear, float zfar)
{
    return Mat4::OrthoD3D(left, right, bottom, top, znear, zfar);
}

Mat4 HWContextD3D11::GetPerspectiveMatrix(
    float fovY, float aspect,
    float znear, float zfar)
{
    return Mat4::PerspectiveD3D(fovY, aspect, znear, zfar);
}

ShaderLanguage HWContextD3D11::GetShaderLanguage() const {
    return ShaderLanguage::HLSL;
}

sptr<HWShader> HWContextD3D11::CreateShader(const sptr<ShaderInfo>& info) {
    return spnew<HWShaderD3D11>(SharedFrom(this), info);
}

sptr<HWRenderTexture> HWContextD3D11::CreateRenderTexture(const sptr<HWTexture>& tex) {
    return spnew<HWRenderTextureD3D11>(tex);
}

sptr<HWSurface> HWContextD3D11::CreateSurface(const sptr<Window>& window)
{
    auto win = spcast<WindowWindows>(window);
    assert(win);
    return spnew<HWSurfaceD3D11>(SharedFrom(this), win);
}

sptr<HWBuffer> HWContextD3D11::CreateBuffer(
    BufferType type, BufferUsage usage,
    BufferCPUAccess cpuAccess, size_t size)
{
    return spnew<HWBufferD3D11>(
        SharedFrom(this), type, usage, cpuAccess, size);
}

sptr<HWBuffer> HWContextD3D11::CreateBuffer(
    BufferType type, BufferUsage usage,
    BufferCPUAccess cpuAccess,
    const std::span<std::byte>& data)
{
    return spnew<HWBufferD3D11>(
        SharedFrom(this), type, usage, cpuAccess, data);
}

sptr<HWTexture> HWContextD3D11::CreateTexture(
    const IVec2& size, PixelDataFormat format, bool dynamic,
    const std::span<std::byte>& data)
{
    return spnew<HWTextureD3D11>(
        SharedFrom(this), size, format, dynamic, data);
}

sptr<HWTexture> HWContextD3D11::CreateTexture(
    const IVec2& size, PixelDataFormat format, bool dynamic,
    const sptr<HWBuffer>& buffer)
{
    return spnew<HWTextureD3D11>(
        SharedFrom(this), size, format, dynamic, buffer);
}

sptr<HWTexture> HWContextD3D11::GetDefaultTexture()
{
    if (!defaultTexture)
    {
        IVec2 texSize = { 1, 1 };
        auto texFormat = PixelDataFormat::RGBA32;
        std::array<std::byte, 4> texBuff {
            (std::byte)0x7F, (std::byte)0x7F, (std::byte)0x7F, (std::byte)0xFF };

        defaultTexture = spnew<HWTextureD3D11>(
            SharedFrom(this), texSize, texFormat, false, texBuff);
    }

    return defaultTexture;
}

void HWContextD3D11::UpdateDeviceStates()
{
    if (blendStateDirty)
    {
        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.AlphaToCoverageEnable = false;
        blendDesc.IndependentBlendEnable = false;
        blendDesc.RenderTarget[0] = targetBlendDesc;

        auto res = device->CreateBlendState(&blendDesc, &blendState);
        if(FAILED(res))
            throw std::runtime_error("failed to create d3d11 blend state");

        deviceContext->OMSetBlendState(blendState.Get(), blendColor, 0xFFFFFFFF);
        blendStateDirty = false;
    }

    if (rasterizerStateDirty)
    {
        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.AntialiasedLineEnable = false;
        rasterDesc.CullMode = cullMode;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.DepthClipEnable = true;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = false;
        rasterDesc.MultisampleEnable = false;
        rasterDesc.ScissorEnable = scissorTestEnabled;
        rasterDesc.SlopeScaledDepthBias = 0.0f;

        auto res = device->CreateRasterizerState(&rasterDesc, &rasterizerState);
        if(FAILED(res))
            throw std::runtime_error("failed to create d3d11 raster state");
        
        deviceContext->RSSetState(rasterizerState.Get());
        rasterizerStateDirty = false;
    }

    if (depthStencilStateDirty)
    {
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
        depthStencilDesc.DepthEnable = depthTestEnabled;
        depthStencilDesc.DepthWriteMask = depthWriteMask;
        depthStencilDesc.DepthFunc = depthFunc;
        depthStencilDesc.StencilEnable = true;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        auto res = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
        if(FAILED(res))
            throw std::runtime_error("failed to create d3d11 depth stencil state");

        deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);
        depthStencilStateDirty = false;
    }
}

} // gfx
} // mw
