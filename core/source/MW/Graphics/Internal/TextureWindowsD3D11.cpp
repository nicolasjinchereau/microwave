/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.TextureWindowsD3D11;
import Microwave.Graphics.Image;
import Microwave.Graphics.Internal.GraphicsContextWindowsD3D11;
import Microwave.System.Console;
import <algorithm>;
import <cassert>;
import <regex>;
import <stdexcept>;
import <unordered_map>;
import <MW/System/Internal/PlatformHeaders.h>;

namespace mw {
inline namespace gfx {

namespace {

std::unordered_map<WrapMode, D3D11_TEXTURE_ADDRESS_MODE> wrapModes = {
    { WrapMode::Clamp, D3D11_TEXTURE_ADDRESS_CLAMP },
    { WrapMode::Repeat, D3D11_TEXTURE_ADDRESS_WRAP }
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

std::unordered_map<FilterMode, D3D11_FILTER> filterModes = {
    { FilterMode::Point, D3D11_FILTER_MIN_MAG_MIP_POINT },
    { FilterMode::Bilinear, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT },
    { FilterMode::Trilinear, D3D11_FILTER_MIN_MAG_MIP_LINEAR }
};

}

TextureWindowsD3D11::TextureWindowsD3D11(
    const sptr<GraphicsContextWindowsD3D11>& graphics,
    const IVec2& size,
    PixelDataFormat format,
    bool dynamic,
    const std::span<std::byte>& data)
    : graphics(graphics),
    size(size),
    format(format),
    dynamic(dynamic)
{
    internalFormat = (format == PixelDataFormat::RGB24) ? PixelDataFormat::RGBA32 : format;

    int bytesPerPixelSrc = GetBytesPerPixel(format);
    int bytesPerPixelDst = GetBytesPerPixel(internalFormat);
    int srcByteCount = size.x * size.y * bytesPerPixelSrc;
    int dstByteCount = size.x * size.y * bytesPerPixelDst;
    this->data = std::make_unique<uint8_t[]>(dstByteCount);

    if (data.size() == srcByteCount)
    {
        Image::Blit(
            data.data(), size, IntRect(size), format,
            (std::byte*)this->data.get(), size, IVec2(0, 0), internalFormat);
    }
    else if (data.empty())
    {
        std::fill(this->data.get(), this->data.get() + srcByteCount, 0);
    }
    else
    {
        throw std::runtime_error("data size does not match buffer");
    }

    hasMipMaps = math::IsPowerOfTwo(size.x) && math::IsPowerOfTwo(size.y);

    AllocateGraphicsResource();
}

TextureWindowsD3D11::~TextureWindowsD3D11()
{
    ReleaseGraphicsResource();
}

uintptr_t TextureWindowsD3D11::GetHandle() {
    return (uintptr_t)texture.Get();
}

PixelDataFormat TextureWindowsD3D11::GetFormat() const {
    return format;
}

IVec2 TextureWindowsD3D11::GetSize() const {
    return size;
}

bool TextureWindowsD3D11::IsDynamic() const {
    return dynamic;
}

void TextureWindowsD3D11::SetPixels(const std::span<std::byte>& data)
{
    if (!dynamic)
        throw std::runtime_error("texture is not dynamic");

    auto bytesPerPixel = GetBytesPerPixel(format);

    if (data.size() != size.x * size.y * bytesPerPixel)
        throw std::runtime_error("incorrect data size");

    Image::Blit(
        data.data(), size, IntRect(size), format,
        (std::byte*)this->data.get(), size, IVec2(0, 0), internalFormat);

    D3D11_BOX destRegion;
    destRegion.left = 0;
    destRegion.right = size.x;
    destRegion.top = 0;
    destRegion.bottom = size.y;
    destRegion.front = 0;
    destRegion.back = 1;

    graphics->context->UpdateSubresource(
        texture.Get(), 0, &destRegion, this->data.get(), size.x * bytesPerPixel, 0);
}

void TextureWindowsD3D11::SetPixels(const std::span<std::byte>& data, const IntRect& rect)
{
    if (rect.w == 0 || rect.h == 0)
        return;

    if (!dynamic)
        throw std::runtime_error("texture is not dynamic");

    auto bytesPerPixelSrc = GetBytesPerPixel(format);
    auto bytesPerPixelDst = GetBytesPerPixel(internalFormat);

    if (data.size() != rect.w * rect.h * bytesPerPixelSrc)
        throw std::runtime_error("incorrect data size");

    Image::Blit(
        data.data(), rect.GetSize(), rect, format,
        (std::byte*)this->data.get(), size, rect.GetPos(), internalFormat,
        false);

    auto offset = (rect.y * size.x + rect.x) * bytesPerPixelDst;
    auto pSrc = this->data.get() + offset;

    D3D11_BOX destRegion;
    destRegion.left = rect.x;
    destRegion.right = rect.x + rect.w;
    destRegion.top = rect.y;
    destRegion.bottom = rect.y + rect.h;
    destRegion.front = 0;
    destRegion.back = 1;

    graphics->context->UpdateSubresource(
        texture.Get(), 0, &destRegion, pSrc, size.x * bytesPerPixelDst, 0);
}

void TextureWindowsD3D11::SetWrapMode(WrapMode wrapMode) {
    this->wrapMode = wrapMode;
    samplerStateDirty = true;
    UpdateSamplerState();
}

WrapMode TextureWindowsD3D11::GetWrapMode() const {
    return wrapMode;
}

void TextureWindowsD3D11::SetFilterMode(FilterMode filterMode) {
    this->filterMode = filterMode;
    samplerStateDirty = true;
    UpdateSamplerState();
}

FilterMode TextureWindowsD3D11::GetFilterMode() const {
    return filterMode;
}

void TextureWindowsD3D11::SetAnisoLevel(float level) {
    anisoLevel = std::min(maxAniso, level);
    samplerStateDirty = true;
    UpdateSamplerState();
}

float TextureWindowsD3D11::GetAnisoLevel() const {
    return anisoLevel;
}

void TextureWindowsD3D11::UpdateSamplerState()
{
    if (samplerStateDirty)
    {
        //anisoLevel > 0.0001f ? anisoLevel : 0
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

        auto res = graphics->device->CreateSamplerState(&samplerDesc, &samplerState);
        if (FAILED(res))
            throw std::runtime_error("could not create sampler state for texture");

        samplerStateDirty = false;
    }
}

void TextureWindowsD3D11::AllocateGraphicsResource()
{
    if (texture)
        return;

    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = size.x;
    textureDesc.Height = size.y;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = textureFormats[format];
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subResData;
    subResData.pSysMem = (void*)data.get();
    subResData.SysMemPitch = size.x * textureComponents[internalFormat];
    subResData.SysMemSlicePitch = size.x * size.y * textureComponents[internalFormat];

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResViewDesc;
    shaderResViewDesc.Format = textureDesc.Format;
    shaderResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResViewDesc.Texture2D.MipLevels = 1;

    HRESULT res = graphics->device->CreateTexture2D(&textureDesc, data ? &subResData : nullptr, &texture);
    if (FAILED(res))
        throw std::runtime_error("could not create texture");

    res = graphics->device->CreateShaderResourceView(texture.Get(), &shaderResViewDesc, &resourceView);
    if (FAILED(res)) {
        texture = nullptr;
        throw std::runtime_error("could not create resource view for texture");
    }

    samplerStateDirty = true;
    UpdateSamplerState();
}

void TextureWindowsD3D11::ReleaseGraphicsResource()
{
    if (!texture)
        return;

    samplerState = nullptr;
    resourceView = nullptr;
    texture = nullptr;
}

bool TextureWindowsD3D11::IsGraphicsResourceAllocated() const {
    return texture != nullptr;
}

GraphicsDriverType TextureWindowsD3D11::GetGraphicsDriverType() const {
    return GraphicsDriverType::Direct3D11;
}

}
}
