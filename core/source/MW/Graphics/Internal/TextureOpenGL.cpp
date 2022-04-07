/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/TextureOpenGL.h>
#include <MW/System/Console.h>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <regex>
#include <stdexcept>

namespace mw {
inline namespace gfx {

std::unordered_map<WrapMode, GLint> wrapModes = {
    { WrapMode::Clamp, gl::CLAMP_TO_EDGE },
    { WrapMode::Repeat, gl::REPEAT }
};

std::unordered_map<PixelDataFormat, GLint> textureFormats = {
    { PixelDataFormat::Alpha8, gl::ALPHA },
    { PixelDataFormat::RGB24, gl::RGB },
    { PixelDataFormat::RGBA32, gl::RGBA },
    { PixelDataFormat::RGBAFloat, gl::RGBA }
};

std::unordered_map<PixelDataFormat, GLenum> componentTypes = {
    { PixelDataFormat::Alpha8, gl::UNSIGNED_BYTE },
    { PixelDataFormat::RGB24, gl::UNSIGNED_BYTE },
    { PixelDataFormat::RGBA32, gl::UNSIGNED_BYTE },
    { PixelDataFormat::RGBAFloat, gl::FLOAT }
};

std::unordered_map<PixelDataFormat, int> textureComponents = {
    { PixelDataFormat::Alpha8, 1 },
    { PixelDataFormat::RGB24, 3 },
    { PixelDataFormat::RGBA32, 4 },
    { PixelDataFormat::RGBAFloat, 4 }
};

std::unordered_map<FilterMode, int> minFilters = {
    { FilterMode::Point, gl::NEAREST_MIPMAP_NEAREST },
    { FilterMode::Bilinear, gl::LINEAR_MIPMAP_NEAREST },
    { FilterMode::Trilinear, gl::LINEAR_MIPMAP_LINEAR }
};

std::unordered_map<FilterMode, int> magFilters = {
    { FilterMode::Point, gl::NEAREST },
    { FilterMode::Bilinear, gl::LINEAR },
    { FilterMode::Trilinear, gl::LINEAR }
};

void CopyPixels(const uint8_t* input, uint8_t* output, IVec2 size, int bytesPerPixel, bool flipVertically)
{
    if (flipVertically)
    {
        const uint8_t* srcRow = input;
        const uint8_t* srcRowsEnd = input + size.x * size.y * bytesPerPixel;
        const uint32_t rowPitch = size.x * bytesPerPixel;
        uint8_t* dstRow = output + size.x * (size.y - 1) * bytesPerPixel;

        for (; srcRow != srcRowsEnd; srcRow += rowPitch, dstRow -= rowPitch)
        {
            const uint8_t* pSrc = srcRow;
            const uint8_t* pSrcEnd = srcRow + rowPitch;
            uint8_t* pDst = dstRow;

            for (; pSrc < pSrcEnd; ++pSrc, ++pDst)
                *pDst = *pSrc;
        }
    }
    else
    {
        memcpy(output, input, size.x * size.y * bytesPerPixel);
    }
}

void BlitInto(const uint8_t* src, const IVec2& srcSize, uint8_t* dest, const IVec2& destSize, const IVec2& pos, int bytesPerPixel, bool flipVertically)
{
    const uint8_t* srcRow = src;
    const uint8_t* srcRowsEnd = src + srcSize.x * srcSize.y * bytesPerPixel;
    int srcStride = srcSize.x * bytesPerPixel;

    uint8_t* dstRow;
    int dstStride;

    if (flipVertically) {
        dstRow = dest + ((destSize.y - 1 - pos.y) * destSize.x + pos.x) * bytesPerPixel;
        dstStride = -destSize.x * bytesPerPixel;
    }
    else {
        dstRow = dest + (pos.y * destSize.x + pos.x) * bytesPerPixel;
        dstStride = destSize.x * bytesPerPixel;
    }

    for (; srcRow != srcRowsEnd;
        srcRow += srcStride, dstRow += dstStride)
    {
        const uint8_t* pSrc = srcRow;
        const uint8_t* pSrcEnd = srcRow + srcStride;
        uint8_t* pDst = dstRow;

        for (; pSrc < pSrcEnd; ++pSrc, ++pDst)
            *pDst = *pSrc;
    }
}

TextureOpenGL::TextureOpenGL(
    const IVec2& size,
    PixelDataFormat format,
    bool dynamic,
    const std::span<std::byte>& data)
    : size(size), format(format), dynamic(dynamic)
{
    int bytesPerPixel = GetBytesPerPixel(format);
    int byteCount = size.x * size.y * bytesPerPixel;
    this->data = std::make_unique<uint8_t[]>(byteCount);

    if (data.size() == byteCount)
        CopyPixels((uint8_t*)data.data(), this->data.get(), size, bytesPerPixel, false);
    else if (data.empty())
        std::fill(this->data.get(), this->data.get() + byteCount, 0);
    else
        throw std::runtime_error("data size does not match buffer");

    isPowerOfTwo = math::IsPowerOfTwo(size.x) && math::IsPowerOfTwo(size.y);

    AllocateGraphicsResource();
}

TextureOpenGL::~TextureOpenGL()
{
    ReleaseGraphicsResource();
}

uintptr_t TextureOpenGL::GetHandle() {
    return (uintptr_t)textureID;
}

PixelDataFormat TextureOpenGL::GetFormat() const {
    return format;
}

IVec2 TextureOpenGL::GetSize() const {
    return size;
}

bool TextureOpenGL::IsDynamic() const {
    return dynamic;
}

void TextureOpenGL::SetPixels(const std::span<std::byte>& data)
{
    if (!dynamic)
        throw std::runtime_error("texture is not dynamic");

    auto bytesPerPixel = GetBytesPerPixel(format);

    if (data.size() != size.x * size.y * bytesPerPixel)
        throw std::runtime_error("incorrect data size");

    CopyPixels((uint8_t*)data.data(), this->data.get(), size, bytesPerPixel, false);

    gl::BindTexture(gl::TEXTURE_2D, textureID);

    auto fmt = textureFormats[format];
    gl::TexImage2D(gl::TEXTURE_2D, 0, fmt, size.x, size.y, 0, fmt, componentTypes[format], this->data.get());
    gl::GenerateMipmap(gl::TEXTURE_2D);

    gl::BindTexture(gl::TEXTURE_2D, 0);
}

void TextureOpenGL::SetPixels(const std::span<std::byte>& data, const IntRect& rect)
{
    if (!dynamic)
        throw std::runtime_error("texture is not dynamic");

    auto bytesPerPixel = GetBytesPerPixel(format);

    if (data.size() != rect.w * rect.h * bytesPerPixel)
        throw std::runtime_error("incorrect data size");

    BlitInto((uint8_t*)data.data(), IVec2(rect.w, rect.h), this->data.get(), this->size, IVec2(rect.x, rect.y), bytesPerPixel, false);

    gl::BindTexture(gl::TEXTURE_2D, textureID);

#if PLATFORM_IOS || PLATFORM_ANDROID
    auto fmt = textureFormats[format];
    auto type = componentTypes[format];
    auto stride = size.x * bytesPerPixel;

    auto pPixel = this->data.get() + (rect.y * size.x + rect.x) * bytesPerPixel;

    for (int y = 0; y < rect.h; ++y)
    {
        gl::TexSubImage2D(GL_TEXTURE_2D, 0, rect.x, rect.y + y, rect.w, 1, fmt, type, pPixel);
        pPixel += stride;
    }
#else
    glPixelStorei(gl::UNPACK_SKIP_ROWS, rect.y);
    glPixelStorei(gl::UNPACK_SKIP_PIXELS, rect.x);
    glPixelStorei(gl::UNPACK_ROW_LENGTH, size.x);

    auto fmt = textureFormats[format];
    auto type = componentTypes[format];
    glTexSubImage2D(gl::TEXTURE_2D, 0, rect.x, rect.y, rect.w, rect.h, fmt, type, this->data.get());

    glPixelStorei(gl::UNPACK_SKIP_ROWS, 0);
    glPixelStorei(gl::UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(gl::UNPACK_ROW_LENGTH, 0);
#endif

    gl::GenerateMipmap(gl::TEXTURE_2D);

    gl::BindTexture(gl::TEXTURE_2D, 0);
}

void TextureOpenGL::SetWrapMode(WrapMode wrapMode) {
    this->wrapMode = wrapMode;
    auto mode = wrapModes[wrapMode];
    gl::BindTexture(gl::TEXTURE_2D, textureID);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, mode);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, mode);
}

WrapMode TextureOpenGL::GetWrapMode() const {
    return wrapMode;
}

void TextureOpenGL::SetFilterMode(FilterMode filterMode) {
    this->filterMode = filterMode;
    int minFilter = minFilters[filterMode];
    int magFilter = magFilters[filterMode];
    gl::BindTexture(gl::TEXTURE_2D, textureID);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, minFilter);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, magFilter);
}

FilterMode TextureOpenGL::GetFilterMode() const {
    return filterMode;
}

void TextureOpenGL::SetAnisoLevel(float level) {
    anisoLevel = std::clamp(level, 1.0f, maxAniso);
    gl::BindTexture(gl::TEXTURE_2D, textureID);
    gl::TexParameterf(gl::TEXTURE_2D, gl::TEXTURE_MAX_ANISOTROPY, anisoLevel);
}

float TextureOpenGL::GetAnisoLevel() const {
    return anisoLevel;
}

void TextureOpenGL::AllocateGraphicsResource()
{
    if (textureID != 0)
        return;

    gl::GenTextures(1, &textureID);
    gl::BindTexture(gl::TEXTURE_2D, textureID);

    auto fmt = textureFormats[format];
    gl::TexImage2D(gl::TEXTURE_2D, 0, fmt, size.x, size.y, 0, fmt, componentTypes[format], data.get());

    if(data)
        gl::GenerateMipmap(gl::TEXTURE_2D);

    int minFilter = minFilters[filterMode];
    int magFilter = magFilters[filterMode];
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, minFilter);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, magFilter);

    auto mode = wrapModes[wrapMode];
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, mode);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, mode);

    maxAniso = 1;
    //gl::GetFloatv(gl::MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    //gl::TexParameterf(gl::TEXTURE_2D, gl::TEXTURE_MAX_ANISOTROPY, anisoLevel);

    gl::BindTexture(gl::TEXTURE_2D, 0);
}

void TextureOpenGL::ReleaseGraphicsResource()
{
    if (textureID == 0)
        return;

    gl::DeleteTextures(1, &textureID);
    textureID = 0;
}

bool TextureOpenGL::IsGraphicsResourceAllocated() const {
    return textureID != 0;
}

GraphicsDriverType TextureOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

}
}
