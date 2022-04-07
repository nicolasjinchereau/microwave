/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/RenderTexture.h>
#include <MW/Graphics/GraphicsContext.h>
#include <MW/Math/Math.h>
#include <wrl/client.h>
#include <d3d11_1.h>

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace mw {
inline namespace gfx {

class RenderTextureWindowsD3D11 : public RenderTexture
{
public:
    IVec2 size;
    int depthBits = 0;
    RenderTextureFormat _format = RenderTextureFormat::RGBA32;
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<ID3D11DepthStencilView> depthStencilView;

    RenderTextureWindowsD3D11(const Vec2& size);

    virtual void Bind(const sptr<GraphicsContext>& graphics) override;
    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
    virtual IVec2 GetSize() override;
};

}
}
