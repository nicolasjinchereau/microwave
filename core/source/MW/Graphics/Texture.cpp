/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Texture;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.Image;
import Microwave.System.Console;
import Microwave.System.ThreadPool;
import <algorithm>;
import <cassert>;
import <MW/System/Internal/PlatformHeaders.h>;

namespace mw {
inline namespace gfx {

Texture::Texture(
    const path& filePath,
    ImageFileFormat fileFormat,
    bool dynamic,
    bool deferLoading)
    : filePath(filePath)
    , fileFormat(fileFormat)
    , dynamic(dynamic)
{
    ImageInfo info = Image::GetInfo(filePath, fileFormat);
    format = info.format;
    size = info.size;

    if (!deferLoading) {
        LoadFile();
    }
}

Texture::Texture(
    const std::span<std::byte>& data,
    PixelDataFormat format,
    const IVec2& size,
    bool dynamic)
    : format(format)
    , size(size)
    , dynamic(dynamic)
{
    auto graphics = GraphicsContext::GetCurrent();
    if (!graphics)
        throw std::runtime_error("no active graphics context");

    if (data.size() != size.x * size.y * GetBytesPerPixel(format))
        throw std::runtime_error("incorrect 'size' or 'format' for 'data'");

    tex = graphics->context->CreateTexture(size, format, dynamic, data);
    tex->SetWrapMode(wrapMode);
    tex->SetFilterMode(filterMode);
    tex->SetAnisoLevel(anisoLevel);

    loadState = LoadState::Loaded;
}

sptr<HWTexture> Texture::GetHWTexture()
{
    auto ret = tex;

    if(!ret)
    {
        auto graphics = GraphicsContext::GetCurrent();
        if (graphics)
            ret = graphics->context->GetDefaultTexture();
    }

    return ret;
}

PixelDataFormat Texture::GetFormat() const {
    return format;
}

IVec2 Texture::GetSize() const {
    return size;
}

bool Texture::IsDynamic() const {
    return dynamic;
}

void Texture::SetPixels(const std::span<std::byte>& data) {
    SetPixels(data, { 0, 0, size.x, size.y });
}

void Texture::SetPixels(const std::span<std::byte>& data, const IntRect& rect)
{
    if (!dynamic)
        throw std::runtime_error("texture is not dynamic");

    auto bytesPerPixel = GetBytesPerPixel(format);
    if (data.size() != rect.w * rect.h * bytesPerPixel)
        throw std::runtime_error("incorrect rect size for data");

    assert(tex);
    tex->SetPixels(data, rect);
}

void Texture::SetWrapMode(TextureWrapMode mode)
{
    if (wrapMode != mode) {
        wrapMode = mode;
        if(tex) tex->SetWrapMode(mode);
    }
}

TextureWrapMode Texture::GetWrapMode() {
    return wrapMode;
}

void Texture::SetFilterMode(TextureFilterMode mode)
{
    if (filterMode != mode) {
        filterMode = mode;
        if(tex) tex->SetFilterMode(mode);
    }
}

TextureFilterMode Texture::GetFilterMode() const {
    return filterMode;
}

void Texture::SetAnisoLevel(float aniso)
{
    if (anisoLevel != aniso) {
        anisoLevel = aniso;
        if(tex) tex->SetAnisoLevel(aniso);
    }
}

float Texture::GetAnisoLevel() const {
    return anisoLevel;
}

LoadState Texture::GetLoadState() const {
    return loadState;
}

void Texture::LoadFile()
{
    if(loadState != LoadState::Unloaded || filePath.empty())
        return;

    auto graphics = GraphicsContext::GetCurrent();
    if (!graphics)
        throw std::runtime_error("no active graphics context");

    auto img = spnew<Image>(filePath, fileFormat);

    tex = graphics->context->CreateTexture(size, format, dynamic, img->GetData());
    tex->SetWrapMode(wrapMode);
    tex->SetFilterMode(filterMode);
    tex->SetAnisoLevel(anisoLevel);

    loadState = LoadState::Loaded;
}

Task<void> Texture::LoadFileAsync()
{
    if(loadState != LoadState::Unloaded || filePath.empty())
        co_return;

    sptr<Texture> pin = SharedFrom(this);

    try
    {
        loadState = LoadState::Loading;

        auto graphics = GraphicsContext::GetCurrent();
        if (!graphics)
            throw std::runtime_error("no active graphics context");

        auto totalBytes = size.x * size.y * GetBytesPerPixel(format);

        sptr<HWBuffer> buffer = graphics->context->CreateBuffer(
            BufferType::PixelUnpack,
            BufferUsage::Static,
            BufferCPUAccess::WriteOnly,
            totalBytes);

        std::span<std::byte> mapping = buffer->Map(BufferMapAccess::WriteNoSync);

        assert(!filePath.empty());

        co_await ThreadPool::InvokeAsync(
            [=](){
                auto img = spnew<Image>(filePath, fileFormat);
                std::span<std::byte> data = img->GetData();
                std::copy(data.begin(), data.end(), mapping.begin());
            });

        buffer->Unmap();

        if (loadState == LoadState::Loading)
        {
            tex = graphics->context->CreateTexture(size, format, dynamic, buffer);
            tex->SetWrapMode(wrapMode);
            tex->SetFilterMode(filterMode);
            tex->SetAnisoLevel(anisoLevel);
            loadState = LoadState::Loaded;
        }
    }
    catch(const std::exception& ex)
    {
        Console::WriteLine(ex.what());

        if (loadState == LoadState::Loading)
        {
            loadState = LoadState::Unloaded;
        }
    }
}

void Texture::UnloadFile()
{
    if(filePath.empty())
        return;

    tex = nullptr;
    loadState = LoadState::Unloaded;
}

} // gfx
} // mw
