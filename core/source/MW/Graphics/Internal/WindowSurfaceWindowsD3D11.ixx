/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.WindowSurfaceWindowsD3D11;
import Microwave.Graphics.WindowSurface;
import Microwave.Graphics.Internal.GraphicsContextWindowsD3D11;
import Microwave.Math;
import Microwave.System.Pointers;
import Microwave.System.Internal.WindowWindows;
import <MW/System/Internal/PlatformHeaders.h>;

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

export namespace mw {
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
