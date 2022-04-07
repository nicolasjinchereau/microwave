/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/WindowSurface.h>
#include <MW/Graphics/Internal/GraphicsContextWindowsD3D11.h>
#include <MW/System/Internal/WindowWindows.h>
#include <MW/Math/Math.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <wrl/client.h>

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace mw {
inline namespace gfx {

class WindowSurfaceWindowsD3D11 : public WindowSurface
{
public:
    sptr<GraphicsContextWindowsD3D11> graphics;
    sptr<WindowWindows> window;

    ComPtr<IDXGISwapChain1> swapChain;
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<ID3D11DepthStencilView> depthStencilView;
    
    WindowSurfaceWindowsD3D11(
        const sptr<GraphicsContextWindowsD3D11>& graphics,
        const sptr<WindowWindows>& window);

    ~WindowSurfaceWindowsD3D11();

    virtual void Bind(const sptr<GraphicsContext>& graphics) override;
    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
    virtual void UpdateSize() override;
    virtual IVec2 GetSize() override;

    DXGI_SWAP_CHAIN_DESC1 CreateSwapChainDesc();
    CD3D11_TEXTURE2D_DESC CreateDepthTextureDesc();
};

}
}
