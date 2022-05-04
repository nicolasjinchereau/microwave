/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWTextureD3D11;
import Microwave.Graphics.Image;
import Microwave.Graphics.Internal.HWContextD3D11;
import Microwave.Graphics.Internal.HWBufferD3D11;
import Microwave.Math;
import Microwave.System.Console;
import <algorithm>;
import <cassert>;
import <regex>;
import <stdexcept>;
import <unordered_map>;
import <MW/System/Internal/PlatformHeaders.h>;

namespace mw {
inline namespace gfx {

inline namespace d3d11 {

std::unordered_map<TextureWrapMode, D3D11_TEXTURE_ADDRESS_MODE> wrapModes = {
    { TextureWrapMode::Clamp, D3D11_TEXTURE_ADDRESS_CLAMP },
    { TextureWrapMode::Repeat, D3D11_TEXTURE_ADDRESS_WRAP }
};

std::unordered_map<PixelDataFormat, DXGI_FORMAT> textureFormats = {
    { PixelDataFormat::Alpha8, DXGI_FORMAT_A8_UNORM },
    { PixelDataFormat::RGB24, DXGI_FORMAT_R8G8B8A8_UNORM },
    { PixelDataFormat::RGBA32, DXGI_FORMAT_R8G8B8A8_UNORM },
    { PixelDataFormat::RGBAFloat, DXGI_FORMAT_R32G32B32A32_FLOAT }
};

std::unordered_map<PixelDataFormat, int> textureComponents = {
    { PixelDataFormat::Alpha8, 1 },
    { PixelDataFormat::RGB24, 4 },
    { PixelDataFormat::RGBA32, 4 },
    { PixelDataFormat::RGBAFloat, 4 }
};

std::unordered_map<TextureFilterMode, D3D11_FILTER> filterModes = {
    { TextureFilterMode::Point, D3D11_FILTER_MIN_MAG_MIP_POINT },
    { TextureFilterMode::Bilinear, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT },
    { TextureFilterMode::Trilinear, D3D11_FILTER_MIN_MAG_MIP_LINEAR }
};

} // d3d11

HWTextureD3D11::HWTextureD3D11(
    const sptr<HWContextD3D11>& context,
    const IVec2& size,
    PixelDataFormat format,
    bool dynamic,
    const std::span<std::byte>& data)
    : context(context)
    , size(size)
    , format(format)
    , dynamic(dynamic)
{
    CreateTexture(data);
}

HWTextureD3D11::HWTextureD3D11(
    const sptr<HWContextD3D11>& context,
    const IVec2& size,
    PixelDataFormat format,
    bool dynamic,
    const sptr<HWBuffer>& buffer)
    : context(context)
    , size(size)
    , format(format)
    , dynamic(dynamic)
{
    auto buff = spcast<HWBufferD3D11>(buffer);
    assert(buff);

    auto data = buff->Map(BufferMapAccess::WriteNoSync);
    CreateTexture(data);
    buffer->Unmap();
}

HWTextureD3D11::~HWTextureD3D11()
{
    samplerState = nullptr;
    resourceView = nullptr;
    texture = nullptr;
}

void HWTextureD3D11::CreateTexture(const std::span<std::byte>& data)
{
    internalFormat = (format == PixelDataFormat::RGB24) ? PixelDataFormat::RGBA32 : format;

    std::span<std::byte> buffer = data;
    std::vector<std::byte> temp;

    if (!data.empty())
    {
        int bytesPerPixelSrc = GetBytesPerPixel(format);
        int srcByteCount = size.x * size.y * bytesPerPixelSrc;

        if (data.size() != srcByteCount)
            throw std::runtime_error("data size does not match buffer");

        if (internalFormat != format)
        {
            int bytesPerPixelDst = GetBytesPerPixel(internalFormat);
            int dstByteCount = size.x * size.y * bytesPerPixelDst;

            temp.resize(dstByteCount);

            Image::Blit(
                data.data(), size, IntRect(size), format,
                temp.data(), size, IVec2(0, 0), internalFormat);

            buffer = temp;
        }
    }

    bool generateMipmaps = false;

    if (!buffer.empty() &&
        IsPowerOfTwo(size.x) &&
        IsPowerOfTwo(size.y))
    {
        UINT fmtSupport = 0;

        auto result = context->device->CheckFormatSupport(
            DXGI_FORMAT_R8G8B8A8_UNORM, &fmtSupport);

        if(SUCCEEDED(result) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
        {
            generateMipmaps = true;
        }
    }

    UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;
    UINT miscFlags = 0;
    UINT mipLevels = 1;
    UINT resourceViewMipLevels = 1;

    if (generateMipmaps) {
        bindFlags |= D3D11_BIND_RENDER_TARGET;
        miscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        mipLevels = 0; // generate all levels
        resourceViewMipLevels = -1; // generate all levels
    }

    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = size.x;
    textureDesc.Height = size.y;
    textureDesc.MipLevels = mipLevels;
    textureDesc.ArraySize = 1;
    textureDesc.Format = textureFormats[format];
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = bindFlags;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = miscFlags;

    D3D11_SUBRESOURCE_DATA subResData;
    subResData.pSysMem = buffer.data();
    subResData.SysMemPitch = size.x * textureComponents[internalFormat];
    subResData.SysMemSlicePitch = size.x * size.y * textureComponents[internalFormat];

    D3D11_SUBRESOURCE_DATA* pSubResData = nullptr;

    if (!buffer.empty() && !generateMipmaps) {
        pSubResData = &subResData;
    }

    HRESULT res = context->device->CreateTexture2D(
        &textureDesc, pSubResData, &texture);

    if (FAILED(res))
        throw std::runtime_error("texture creation failed");

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResViewDesc;
    shaderResViewDesc.Format = textureDesc.Format;
    shaderResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResViewDesc.Texture2D.MipLevels = resourceViewMipLevels;

    res = context->device->CreateShaderResourceView(
        texture.Get(), &shaderResViewDesc, &resourceView);

    if (FAILED(res))
        throw std::runtime_error("texture creation failed");

    if(generateMipmaps)
    {
        context->deviceContext->UpdateSubresource(
            texture.Get(),
            0,
            nullptr,
            buffer.data(),
            subResData.SysMemPitch,
            subResData.SysMemSlicePitch);

        context->deviceContext->GenerateMips(resourceView.Get());
    }

    UpdateSamplerState();
}

void HWTextureD3D11::SetPixels(const std::span<std::byte>& data, const IntRect& rect)
{
    if (rect.w == 0 || rect.h == 0)
        return;

    if (!dynamic)
        throw std::runtime_error("texture is not dynamic");

    auto bytesPerPixelSrc = GetBytesPerPixel(format);
    auto bytesPerPixelDst = GetBytesPerPixel(internalFormat);

    if (data.size() != rect.w * rect.h * bytesPerPixelSrc)
        throw std::runtime_error("incorrect data size");

    std::span<std::byte> buffer = data;
    std::vector<std::byte> temp;

    if (internalFormat != format)
    {
        int bytesPerPixelDst = GetBytesPerPixel(internalFormat);
        int dstByteCount = size.x * size.y * bytesPerPixelDst;

        temp.resize(dstByteCount);
        
        Image::Blit(
            data.data(), rect.GetSize(), IntRect(rect.GetSize()), format,
            temp.data(), size, rect.GetPos(), internalFormat);

        buffer = temp;
    }

    D3D11_BOX destRegion;
    destRegion.left = rect.x;
    destRegion.right = rect.x + rect.w;
    destRegion.top = rect.y;
    destRegion.bottom = rect.y + rect.h;
    destRegion.front = 0;
    destRegion.back = 1;

    context->deviceContext->UpdateSubresource(
        texture.Get(), 0, &destRegion, buffer.data(), rect.w * bytesPerPixelDst, 0);

    if (IsPowerOfTwo(size.x) && IsPowerOfTwo(size.y))
        context->deviceContext->GenerateMips(resourceView.Get());
}

void HWTextureD3D11::SetWrapMode(TextureWrapMode wrapMode) {
    this->wrapMode = wrapMode;
    UpdateSamplerState();
}

void HWTextureD3D11::SetFilterMode(TextureFilterMode filterMode) {
    this->filterMode = filterMode;
    UpdateSamplerState();
}

void HWTextureD3D11::SetAnisoLevel(float aniso) {
    auto anisoLevel = aniso;
    UpdateSamplerState();
}

void HWTextureD3D11::UpdateSamplerState()
{
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = filterModes[filterMode];
    samplerDesc.AddressU = wrapModes[wrapMode];
    samplerDesc.AddressV = wrapModes[wrapMode];
    samplerDesc.AddressW = wrapModes[wrapMode];
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    auto res = context->device->CreateSamplerState(&samplerDesc, &samplerState);
    if (FAILED(res))
        throw std::runtime_error("could not create sampler state for texture");
}

} // gfx
} // mw
