/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWRenderTextureD3D11;
import Microwave.Graphics.Internal.HWRenderTexture;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Math;
import Microwave.System.Pointers;
import <MW/System/Internal/PlatformHeaders.h>;

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

export namespace mw {
inline namespace gfx {

class HWContext;
class HWTextureD3D11;

class HWRenderTextureD3D11 : public HWRenderTexture
{
public:
    IVec2 size;
    int depthBits = 24;
    RenderTextureFormat format = RenderTextureFormat::RGBA32;
    sptr<HWTextureD3D11> tex;
    ComPtr<ID3D11Texture2D> depthTexture;
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<ID3D11DepthStencilView> depthStencilView;
    //ComPtr<ID3D11Texture2D> stagingTexture;

    HWRenderTextureD3D11(const sptr<HWTexture>& tex);
    ~HWRenderTextureD3D11();

    virtual IVec2 GetSize() override;
    virtual sptr<HWTexture> GetTexture() override;
};

} // gfx
} // mw
