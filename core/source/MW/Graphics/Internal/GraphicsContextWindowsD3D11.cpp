/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <Windows.h>
#include <glew.h>
#include <gl/GL.h>
#include <MW/Graphics/GraphicsTypes.h>
#include <MW/Graphics/Internal/GraphicsContextWindowsD3D11.h>
#include <MW/Graphics/Internal/WindowSurfaceWindowsD3D11.h>
#include <MW/Graphics/Internal/RenderTextureWindowsD3D11.h>
#include <MW/Graphics/Internal/ShaderWindowsD3D11.h>
#include <MW/Graphics/Internal/DrawBufferWindowsD3D11.h>
#include <MW/Graphics/Internal/TextureWindowsD3D11.h>
#include <MW/System/Internal/WindowWindows.h>
#include <stdexcept>

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

GraphicsContextWindowsD3D11::GraphicsContextWindowsD3D11()
{
    _dispatcher = Dispatcher::GetCurrent();
    AllocateGraphicsResource();
}

GraphicsContextWindowsD3D11::~GraphicsContextWindowsD3D11()
{
    ReleaseGraphicsResource();
}

sptr<Dispatcher> GraphicsContextWindowsD3D11::GetDispatcher() {
    return _dispatcher;
}

Vec2 GraphicsContextWindowsD3D11::GetDepthRangeNDC() {
    return Vec2(0.0f, 1.0f);
}

void GraphicsContextWindowsD3D11::MakeCurrent() {
    
}

IntRect GraphicsContextWindowsD3D11::GetViewport() const {
    return _viewportRect;
}

void GraphicsContextWindowsD3D11::SetViewport(const IntRect& rect)
{
    _viewportRect = rect;
    CD3D11_VIEWPORT viewport((float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h);
    context->RSSetViewports(1, &viewport);
}

bool GraphicsContextWindowsD3D11::IsBlendingEnabled() const {
    return _blendingEnabled;
}

void GraphicsContextWindowsD3D11::SetBlendingEnabled(bool enabled) {
    if (_blendingEnabled != enabled) {
        _blendingEnabled = enabled;
        blendStateDirty = true;
    }
}

bool GraphicsContextWindowsD3D11::IsScissorTestEnabled() const {
    return _scissorTestEnabled;
}

void GraphicsContextWindowsD3D11::SetScissorTestEnabled(bool enabled) {
    if (_scissorTestEnabled != enabled) {
        _scissorTestEnabled = enabled;
        rasterizerStateDirty = true;
    }
}

IntRect GraphicsContextWindowsD3D11::GetScissorRect() const {
    return _scissorRect;
}

void GraphicsContextWindowsD3D11::SetScissorRect(const IntRect& rect)
{
    _scissorRect = rect;
    
    D3D11_RECT rc;
    rc.left = rect.x;
    rc.top = rect.x;
    rc.right = rect.x + rect.w;
    rc.bottom = rect.y + rect.h;

    context->RSSetScissorRects(1, &rc);
}

Color GraphicsContextWindowsD3D11::GetClearColor() const {
    return _clearColor;
}

void GraphicsContextWindowsD3D11::SetClearColor(const Color& color) {
    _clearColor = color;
}

void GraphicsContextWindowsD3D11::SetColorMask(bool red, bool green, bool blue, bool alpha)
{
    if (_colorMask[0] != red ||
        _colorMask[1] != green ||
        _colorMask[2] != blue ||
        _colorMask[3] != alpha)
    {
        _colorMask[0] = red;
        _colorMask[1] = green;
        _colorMask[2] = blue;
        _colorMask[3] = alpha;
        blendStateDirty = true;
    }
}

void GraphicsContextWindowsD3D11::SetBlendFactors(BlendFactor source, BlendFactor dest)
{
    if (_srcColorFactor != source ||
        _dstColorFactor != dest ||
        _srcAlphaFactor != source ||
        _dstAlphaFactor != dest)
    {
        _srcColorFactor = source;
        _dstColorFactor = dest;
        _srcAlphaFactor = source;
        _dstAlphaFactor = dest;
        blendStateDirty = true;
    }
}

void GraphicsContextWindowsD3D11::SetBlendFactors(
    BlendFactor sourceColor, BlendFactor destColor,
    BlendFactor sourceAlpha, BlendFactor destAlpha)
{
    if (_srcColorFactor != sourceColor ||
        _dstColorFactor != destColor ||
        _srcAlphaFactor != sourceAlpha ||
        _dstAlphaFactor != destAlpha)
    {
        _srcColorFactor = sourceColor;
        _dstColorFactor = destColor;
        _srcAlphaFactor = sourceAlpha;
        _dstAlphaFactor = destAlpha;
        blendStateDirty = true;
    }
}

BlendFactor GraphicsContextWindowsD3D11::GetSourceColorBlendFactor() const {
    return _srcColorFactor;
}

BlendFactor GraphicsContextWindowsD3D11::GetDestColorBlendFactor() const {
    return _dstColorFactor;
}

BlendFactor GraphicsContextWindowsD3D11::GetSourceAlphaBlendFactor() const {
    return _srcAlphaFactor;
}

BlendFactor GraphicsContextWindowsD3D11::GetDestAlphaBlendFactor() const {
    return _dstAlphaFactor;
}

void GraphicsContextWindowsD3D11::SetBlendOperations(BlendOperation blendOp)
{
    if (_colorBlendOperation != blendOp || _alphaBlendOperation != blendOp)
    {
        _colorBlendOperation = blendOp;
        _alphaBlendOperation = blendOp;
        blendStateDirty = true;
    }
}

void GraphicsContextWindowsD3D11::SetBlendOperations(
    BlendOperation colorBlendOp, BlendOperation alphaBlendOp)
{
    if (_colorBlendOperation != colorBlendOp || _alphaBlendOperation != alphaBlendOp)
    {
        _colorBlendOperation = colorBlendOp;
        _alphaBlendOperation = alphaBlendOp;
        blendStateDirty = true;
    }
}

BlendOperation GraphicsContextWindowsD3D11::GetColorBlendOperation() const {
    return _colorBlendOperation;
}

BlendOperation GraphicsContextWindowsD3D11::GetAlphaBlendOperation() const {
    return _alphaBlendOperation;
}

Color GraphicsContextWindowsD3D11::GetBlendColor() const {
    return _blendColor;
}

void GraphicsContextWindowsD3D11::SetBlendColor(Color color)
{
    if (_blendColor != color) {
        _blendColor = color;
        blendStateDirty = true; // TODO: make separate flag for this since blend-color is a separate arg to OMSetBlendState
    }
}

CullMode GraphicsContextWindowsD3D11::GetCullMode() const {
    return _cullMode;
}

void GraphicsContextWindowsD3D11::SetCullMode(CullMode mode)
{
    if (_cullMode != mode) {
        _cullMode = mode;
        rasterizerStateDirty = true;
    }
}

DepthTest GraphicsContextWindowsD3D11::GetDepthTest() const {
    return _depthTest;
}

void GraphicsContextWindowsD3D11::SetDepthTest(DepthTest test) {
    _depthTest = test;
    depthStencilStateDirty = true;
}

bool GraphicsContextWindowsD3D11::IsDepthWriteEnabled() const {
    return _depthWriteEnabled;
}

void GraphicsContextWindowsD3D11::SetDepthWriteEnabled(bool enabled) {
    _depthWriteEnabled = enabled;
    depthStencilStateDirty = true;
}

int GraphicsContextWindowsD3D11::GetSwapInterval() const {
    return _swapInterval;
}

void GraphicsContextWindowsD3D11::SetSwapInterval(int interval) {
    _swapInterval = interval;
}

sptr<RenderTarget> GraphicsContextWindowsD3D11::GetRenderTarget() const {
    return _targetSurface ? (sptr<RenderTarget>)_targetSurface : (sptr<RenderTarget>)_targetTexture;
}

void GraphicsContextWindowsD3D11::SetRenderTarget(const sptr<RenderTarget>& target) {
    target->Bind(This<GraphicsContext>());
}

void GraphicsContextWindowsD3D11::SetRenderTarget(const sptr<WindowSurface>& target)
{
    _targetTexture = nullptr;
    _targetSurface = std::dynamic_pointer_cast<WindowSurfaceWindowsD3D11>(target);

    if (_targetSurface)
    {
        context->OMSetRenderTargets(1, _targetSurface->renderTargetView.GetAddressOf(), _targetSurface->depthStencilView.Get());
        auto size = _targetSurface->window->size;
        auto rc = IntRect(0, 0, size.x, size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        context->OMSetRenderTargets(0, nullptr, nullptr);
        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextWindowsD3D11::SetRenderTarget(const sptr<RenderTexture>& target)
{
    _targetSurface = nullptr;
    _targetTexture = std::dynamic_pointer_cast<RenderTextureWindowsD3D11>(target);
    
    if (_targetTexture)
    {
        context->OMSetRenderTargets(1, _targetTexture->renderTargetView.GetAddressOf(), _targetTexture->depthStencilView.Get());
        auto size = _targetTexture->size;
        auto rc = IntRect(0, 0, size.x, size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        context->OMSetRenderTargets(0, nullptr, nullptr);
        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextWindowsD3D11::RebindRenderTarget()
{
    if (_targetSurface)
    {
        context->OMSetRenderTargets(1, _targetSurface->renderTargetView.GetAddressOf(), _targetSurface->depthStencilView.Get());
        auto size = _targetSurface->window->size;
        auto rc = IntRect(0, 0, size.x, size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else if (_targetTexture)
    {
        context->OMSetRenderTargets(1, _targetTexture->renderTargetView.GetAddressOf(), _targetTexture->depthStencilView.Get());
        auto size = _targetTexture->size;
        auto rc = IntRect(0, 0, size.x, size.y);
        SetViewport(rc);
        SetScissorRect(rc);
    }
    else
    {
        context->OMSetRenderTargets(0, nullptr, nullptr);
        auto rc = IntRect();
        SetViewport(rc);
        SetScissorRect(rc);
    }
}

void GraphicsContextWindowsD3D11::Clear(bool depth, bool color)
{
    if (_targetSurface) {
        if (color) context->ClearRenderTargetView(_targetSurface->renderTargetView.Get(), _clearColor);
        if (depth) context->ClearDepthStencilView(_targetSurface->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
    else if (_targetTexture) {
        if(color) context->ClearRenderTargetView(_targetTexture->renderTargetView.Get(), _clearColor);
        if(depth) context->ClearDepthStencilView(_targetTexture->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
}

void GraphicsContextWindowsD3D11::Flush() {
    context->Flush();
}

void GraphicsContextWindowsD3D11::Flip()
{
    if (_targetSurface) {
        _targetSurface->swapChain->Present(_swapInterval, 0);
        context->DiscardView(_targetSurface->renderTargetView.Get());
        context->DiscardView(_targetSurface->depthStencilView.Get());
    }
}

void GraphicsContextWindowsD3D11::DrawArray(int start, int count, DrawMode mode)
{
    UpdatePipeline();
    context->IASetPrimitiveTopology(primitiveTopologies[mode]);
    context->Draw(count, start);
}

void GraphicsContextWindowsD3D11::DrawIndexed(int start, int count, DrawMode mode)
{
    UpdatePipeline();
    context->IASetPrimitiveTopology(primitiveTopologies[mode]);
    context->DrawIndexed(count, start, 0);
}

sptr<RenderTexture> GraphicsContextWindowsD3D11::CreateRenderTexture(const Vec2& size) {
    return spnew<RenderTextureWindowsD3D11>(size);
}

void GraphicsContextWindowsD3D11::CreateWindowSurface(const sptr<Window>& window) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextWindowsD3D11>(This());
    auto win = std::dynamic_pointer_cast<WindowWindows>(window);
    win->surface = spnew<WindowSurfaceWindowsD3D11>(gfx, win);
}

sptr<Shader> GraphicsContextWindowsD3D11::CreateShader(const std::string& source) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextWindowsD3D11>(This());
    return spnew<ShaderWindowsD3D11>(gfx, source);
}

sptr<DrawBuffer> GraphicsContextWindowsD3D11::CreateBuffer(uint32_t capacity, DrawBufferType type, bool dynamic, const std::span<std::byte>& data) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextWindowsD3D11>(This());
    return spnew<DrawBufferWindowsD3D11>(gfx, capacity, type, dynamic, data);
}

sptr<Texture> GraphicsContextWindowsD3D11::CreateTexture(const IVec2& size, PixelDataFormat format, bool dynamic, const std::span<std::byte>& data) {
    auto gfx = std::dynamic_pointer_cast<GraphicsContextWindowsD3D11>(This());
    return spnew<TextureWindowsD3D11>(gfx, size, format, dynamic, data);
}

Mat4 GraphicsContextWindowsD3D11::GetOrthoMatrix(float left, float right, float bottom, float top, float znear, float zfar) {
    return Mat4::OrthoD3D(left, right, bottom, top, znear, zfar);
}

Mat4 GraphicsContextWindowsD3D11::GetPerspectiveMatrix(float fovY, float aspect, float znear, float zfar) {
    return Mat4::PerspectiveD3D(fovY, aspect, znear, zfar);
}

void GraphicsContextWindowsD3D11::UpdateBlendState()
{
    if (blendStateDirty)
    {
        UINT8 writeMask = 0;
        if(_colorMask[0]) writeMask |= D3D11_COLOR_WRITE_ENABLE_RED;
        if(_colorMask[1]) writeMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
        if(_colorMask[2]) writeMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
        if(_colorMask[3]) writeMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
        
        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;
        blendDesc.RenderTarget[0].BlendEnable = _blendingEnabled;
        blendDesc.RenderTarget[0].SrcBlend = blendFactors[_srcColorFactor];
        blendDesc.RenderTarget[0].DestBlend = blendFactors[_dstColorFactor];
        blendDesc.RenderTarget[0].BlendOp = blendOperations[_colorBlendOperation];
        blendDesc.RenderTarget[0].SrcBlendAlpha = blendFactors[_srcAlphaFactor];
        blendDesc.RenderTarget[0].DestBlendAlpha = blendFactors[_dstAlphaFactor];
        blendDesc.RenderTarget[0].BlendOpAlpha = blendOperations[_alphaBlendOperation];
        blendDesc.RenderTarget[0].RenderTargetWriteMask = writeMask;

        auto res = device->CreateBlendState(&blendDesc, &blendState);
        if(FAILED(res))
            throw std::runtime_error("failed to create d3d11 blend state");

        context->OMSetBlendState(blendState.Get(), _blendColor, 0xFFFFFFFF);
        blendStateDirty = false;
    }
}

void GraphicsContextWindowsD3D11::UpdateRasterizerState()
{
    if (rasterizerStateDirty)
    {
        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.AntialiasedLineEnable = false;
        rasterDesc.CullMode = cullModes[_cullMode];
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.0f;
        rasterDesc.DepthClipEnable = true;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = false;
        rasterDesc.MultisampleEnable = false;
        rasterDesc.ScissorEnable = _scissorTestEnabled;
        rasterDesc.SlopeScaledDepthBias = 0.0f;

        auto res = device->CreateRasterizerState(&rasterDesc, &rasterizerState);
        if(FAILED(res))
            throw std::runtime_error("failed to create d3d11 raster state");

        context->RSSetState(rasterizerState.Get());
        rasterizerStateDirty = false;
    }
}

void GraphicsContextWindowsD3D11::UpdateDepthStencilState()
{
    if (depthStencilStateDirty)
    {
        bool depthEnabled;
        D3D11_COMPARISON_FUNC depthFunc;

        switch (_depthTest)
        {
        case DepthTest::Always:
            depthEnabled = false;
            depthFunc = D3D11_COMPARISON_ALWAYS;
            break;

        case DepthTest::Never:
            depthEnabled = true;
            depthFunc = D3D11_COMPARISON_NEVER;
            break;

        case DepthTest::Less:
            depthEnabled = true;
            depthFunc = D3D11_COMPARISON_LESS;
            break;

        case DepthTest::Equal:
            depthEnabled = true;
            depthFunc = D3D11_COMPARISON_EQUAL;
            break;

        case DepthTest::LessOrEqual:
            depthEnabled = true;
            depthFunc = D3D11_COMPARISON_LESS_EQUAL;
            break;

        case DepthTest::Greater:
            depthEnabled = true;
            depthFunc = D3D11_COMPARISON_GREATER;
            break;

        case DepthTest::NotEqual:
            depthEnabled = true;
            depthFunc = D3D11_COMPARISON_NOT_EQUAL;
            break;

        case DepthTest::GreaterOrEqual:
            depthEnabled = true;
            depthFunc = D3D11_COMPARISON_GREATER_EQUAL;
            break;
        }

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
        depthStencilDesc.DepthEnable = depthEnabled;
        depthStencilDesc.DepthWriteMask = _depthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
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

        context->OMSetDepthStencilState(depthStencilState.Get(), 1);
        depthStencilStateDirty = false;
    }
}

void GraphicsContextWindowsD3D11::UpdatePipeline() {
    UpdateDepthStencilState();
    UpdateRasterizerState();
    UpdateBlendState();
}

void GraphicsContextWindowsD3D11::AllocateGraphicsResource()
{
    if (_alive)
        return;

    try
    {
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

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

        res = tmpContext.As(&context);
        if(FAILED(res)) throw std::runtime_error("failed to get d3d11 context interface");
        
        blendStateDirty = true;
        rasterizerStateDirty = true;
        depthStencilStateDirty = true;
        
        UpdatePipeline();
        
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

void GraphicsContextWindowsD3D11::ReleaseGraphicsResource()
{
    _alive = false;

    _targetSurface = nullptr;
    _targetTexture = nullptr;
    blendState = nullptr;
    rasterizerState = nullptr;
    depthStencilState = nullptr;
    context = nullptr;
    device = nullptr;

    blendStateDirty = true;
    rasterizerStateDirty = true;
    depthStencilStateDirty = true;
}

bool GraphicsContextWindowsD3D11::IsGraphicsResourceAllocated() const {
    return _alive;
}

GraphicsDriverType GraphicsContextWindowsD3D11::GetGraphicsDriverType() const {
    return GraphicsDriverType::Direct3D11;
}

}
}
