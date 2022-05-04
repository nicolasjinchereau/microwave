/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWSurfaceD3D11;
import Microwave.Graphics.Internal.HWSurface;
import Microwave.Math;
import Microwave.System.Pointers;
import <MW/System/Internal/PlatformHeaders.h>;

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

export namespace mw {

inline namespace system {
class WindowWindows;
} // system

inline namespace gfx {

class HWContextD3D11;

class HWSurfaceD3D11 : public HWSurface
{
    void Create();
    void Destroy();
public:
    sptr<HWContextD3D11> context;
    sptr<WindowWindows> window;

    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<ID3D11DepthStencilView> depthStencilView;
    ComPtr<IDXGISwapChain1> swapChain;
    
    HWSurfaceD3D11(
        const sptr<HWContextD3D11>& context,
        const sptr<WindowWindows>& window);

    ~HWSurfaceD3D11();

    virtual void UpdateSize() override;
    virtual IVec2 GetSize() const override;

    DXGI_SWAP_CHAIN_DESC1 CreateSwapChainDesc();
    CD3D11_TEXTURE2D_DESC CreateDepthTextureDesc();
};

} // gfx
} // mw
