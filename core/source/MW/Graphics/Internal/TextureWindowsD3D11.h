/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Texture.h>
#include <string>
#include <glew.h>
#include <cstdint>
#include <wrl/client.h>
#include <d3d11_1.h>

namespace mw {
inline namespace gfx {

class GraphicsContextWindowsD3D11;

class TextureWindowsD3D11 : public Texture
{
public:
    sptr<GraphicsContextWindowsD3D11> graphics;
    std::unique_ptr<uint8_t[]> data;
    IVec2 size = IVec2::Zero();
    PixelDataFormat format = PixelDataFormat::RGBA32;
    PixelDataFormat internalFormat = PixelDataFormat::RGBA32;
    WrapMode wrapMode = WrapMode::Clamp;
    FilterMode filterMode = FilterMode::Bilinear;
    float maxAniso = 0;
    float anisoLevel = 0;
    bool hasMipMaps = true;
    bool samplerStateDirty = true;
    bool dynamic = false;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

    void UpdateSamplerState();

    TextureWindowsD3D11(
        const sptr<GraphicsContextWindowsD3D11>& graphics,
        const IVec2& size,
        PixelDataFormat format,
        bool dynamic,
        const std::span<std::byte>& data);
    
    ~TextureWindowsD3D11();

    virtual uintptr_t GetHandle() override;
    virtual PixelDataFormat GetFormat() const override;
    virtual IVec2 GetSize() const override;

    virtual bool IsDynamic() const override;

    virtual void SetPixels(const std::span<std::byte>& data) override;
    virtual void SetPixels(const std::span<std::byte>& data, const IntRect& rect) override;

    virtual void SetWrapMode(WrapMode wrapMode) override;
    virtual WrapMode GetWrapMode() const override;

    virtual void SetFilterMode(FilterMode filterMode) override;
    virtual FilterMode GetFilterMode() const override;

    virtual void SetAnisoLevel(float level) override;
    virtual float GetAnisoLevel() const override;

    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
};

}
}
