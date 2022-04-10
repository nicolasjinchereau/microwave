/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.WindowSurfaceWindowsD3D11;
import Microwave.Graphics.Internal.GraphicsContextWindowsD3D11;
import <stdexcept>;
import <MW/System/Internal/PlatformHeaders.h>;

namespace mw {
inline namespace gfx {

WindowSurfaceWindowsD3D11::WindowSurfaceWindowsD3D11(
    const sptr<GraphicsContextWindowsD3D11>& graphics,
    const sptr<WindowWindows>& window)
    : graphics(graphics), window(window)
{
    AllocateGraphicsResource();
}

WindowSurfaceWindowsD3D11::~WindowSurfaceWindowsD3D11()
{
    ReleaseGraphicsResource();
}

DXGI_SWAP_CHAIN_DESC1 WindowSurfaceWindowsD3D11::CreateSwapChainDesc()
{
    auto winSize = window->GetSize();

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
    swapChainDesc.Width = (UINT)math::RoundToInt(winSize.x);
    swapChainDesc.Height = (UINT)math::RoundToInt(winSize.y);
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1; // 2
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;// DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = 0;
    return swapChainDesc;
}

CD3D11_TEXTURE2D_DESC WindowSurfaceWindowsD3D11::CreateDepthTextureDesc()
{
    CD3D11_TEXTURE2D_DESC desc(
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        window->size.x,
        window->size.y,
        1,
        1,
        D3D11_BIND_DEPTH_STENCIL);

    return desc;
}

void WindowSurfaceWindowsD3D11::Bind(const sptr<GraphicsContext>& graphics) {
    graphics->SetRenderTarget(This<RenderTarget>());
}

void WindowSurfaceWindowsD3D11::AllocateGraphicsResource()
{
// create swap chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = CreateSwapChainDesc();
    ComPtr<IDXGIDevice1> dxgiDevice;
    ComPtr<IDXGIAdapter> dxgiAdapter;
    ComPtr<IDXGIFactory2> dxgiFactory;
    graphics->device.As(&dxgiDevice);
    dxgiDevice->GetAdapter(&dxgiAdapter);
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);
    
    HRESULT res;

    res = dxgiFactory->CreateSwapChainForHwnd(
        graphics->device.Get(), window->hWnd,
        &swapChainDesc, nullptr, nullptr,
        &swapChain);
    
    if(FAILED(res))
        throw std::runtime_error("failed to create d3d11 swap chain");

    dxgiDevice->SetMaximumFrameLatency(1);
    
// create render target view
    ComPtr<ID3D11Texture2D> backBufferTex;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTex);
    res = graphics->device->CreateRenderTargetView(backBufferTex.Get(), nullptr, &renderTargetView);

    if(FAILED(res))
        throw std::runtime_error("failed to create d3d11 render target view");

// create depth stencil view
    CD3D11_TEXTURE2D_DESC depthTextureDesc = CreateDepthTextureDesc();
    ComPtr<ID3D11Texture2D> depthStencilTex;
    res = graphics->device->CreateTexture2D(&depthTextureDesc, nullptr, &depthStencilTex);

    if(FAILED(res))
        throw std::runtime_error("failed to create texture for d3d11 depth stencil");

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    res = graphics->device->CreateDepthStencilView(depthStencilTex.Get(), &depthStencilViewDesc, &depthStencilView);

    if(FAILED(res))
        throw std::runtime_error("failed to create d3d11 depth stencil view");
}

void WindowSurfaceWindowsD3D11::ReleaseGraphicsResource() {
    swapChain = nullptr;
    renderTargetView = nullptr;
    depthStencilView = nullptr;
}

bool WindowSurfaceWindowsD3D11::IsGraphicsResourceAllocated() const {
    return false;
}

GraphicsDriverType WindowSurfaceWindowsD3D11::GetGraphicsDriverType() const {
    return GraphicsDriverType::Direct3D11;
}

void WindowSurfaceWindowsD3D11::UpdateSize()
{
    ReleaseGraphicsResource();
    AllocateGraphicsResource();
}

IVec2 WindowSurfaceWindowsD3D11::GetSize() {
    return window->GetSize();
}

}
}
