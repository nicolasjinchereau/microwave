/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWTextureD3D11;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.Internal.HWBuffer;
import Microwave.Graphics.Internal.HWTexture;
import Microwave.Math;
import Microwave.System.Pointers;
import <cstddef>;
import <cstdint>;
import <span>;
import <string>;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {
inline namespace gfx {

class HWContextD3D11;

class HWTextureD3D11 : public HWTexture
{
public:
    gptr<HWContextD3D11> context;
    IVec2 size = IVec2::Zero();
    PixelDataFormat format = PixelDataFormat::RGBA32;
    PixelDataFormat internalFormat = PixelDataFormat::RGBA32;
    float maxAniso = 1;
    bool dynamic = {};

    TextureWrapMode wrapMode = TextureWrapMode::Clamp;
    TextureFilterMode filterMode = TextureFilterMode::Bilinear;
    float anisoLevel = 0;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

    HWTextureD3D11(
        const gptr<HWContextD3D11>& context,
        const IVec2& size,
        PixelDataFormat format,
        bool dynamic,
        const std::span<std::byte>& data);
    
    HWTextureD3D11(
        const gptr<HWContextD3D11>& context,
        const IVec2& size,
        PixelDataFormat format,
        bool dynamic,
        const gptr<HWBuffer>& buffer);

    virtual ~HWTextureD3D11();

    virtual void SetPixels(const std::span<std::byte>& data, const IntRect& rect) override;
    virtual void SetWrapMode(TextureWrapMode wrapMode) override;
    virtual void SetFilterMode(TextureFilterMode filterMode) override;
    virtual void SetAnisoLevel(float aniso) override;

    void CreateTexture(const std::span<std::byte>& data);
    void UpdateSamplerState();
};

}
}
