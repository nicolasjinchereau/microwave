/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWTextureOpenGL;
import Microwave.Graphics.Internal.HWBufferOpenGL;
import Microwave.Graphics.Internal.HWContextOpenGL;
import Microwave.System.Exception;
import Microwave.System.ThreadPool;
import <MW/System/Internal/PlatformHeaders.h>;
import <MW/System/Debug.h>;
import <algorithm>;
import <cstring>;
import <regex>;
import <stdexcept>;
import <unordered_map>;

namespace mw {
inline namespace gfx {

inline namespace opengl {

std::unordered_map<TextureWrapMode, gl::Int> wrapModes = {
    { TextureWrapMode::Clamp, gl::CLAMP_TO_EDGE },
    { TextureWrapMode::Repeat, gl::REPEAT }
};

std::unordered_map<PixelDataFormat, gl::Int> textureFormats = {
    { PixelDataFormat::Alpha8, gl::ALPHA },
    { PixelDataFormat::RGB24, gl::RGB },
    { PixelDataFormat::RGBA32, gl::RGBA },
    { PixelDataFormat::RGBAFloat, gl::RGBA }
};

std::unordered_map<PixelDataFormat, gl::Int> sizedInternalFormats = {
    { PixelDataFormat::Alpha8, gl::R8 },
    { PixelDataFormat::RGB24, gl::RGB8 },
    { PixelDataFormat::RGBA32, gl::RGBA8 },
    { PixelDataFormat::RGBAFloat, gl::RGBA32F }
};

std::unordered_map<PixelDataFormat, gl::Enum> componentTypes = {
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

std::unordered_map<TextureFilterMode, int> minFilters = {
    { TextureFilterMode::Point, gl::NEAREST_MIPMAP_NEAREST },
    { TextureFilterMode::Bilinear, gl::LINEAR_MIPMAP_NEAREST },
    { TextureFilterMode::Trilinear, gl::LINEAR_MIPMAP_LINEAR }
};

std::unordered_map<TextureFilterMode, int> magFilters = {
    { TextureFilterMode::Point, gl::NEAREST },
    { TextureFilterMode::Bilinear, gl::LINEAR },
    { TextureFilterMode::Trilinear, gl::LINEAR }
};

} // opengl

HWTextureOpenGL::HWTextureOpenGL(
    const gptr<HWContextOpenGL>& context,
    const IVec2& size,
    PixelDataFormat format,
    bool dynamic,
    const std::span<std::byte>& data)
    : context(context)
    , size(size)
    , format(format)
    , dynamic(dynamic)
{
    int bytesPerPixel = GetBytesPerPixel(format);
    int byteCount = size.x * size.y * bytesPerPixel;

    if (data.size() != byteCount)
        throw Exception("incorrect size for 'data'");

    gl::GenTextures(1, &textureID);
    gl::BindTexture(gl::TEXTURE_2D, textureID);

    auto fmt = textureFormats[format];
    auto intFmt = sizedInternalFormats[format];
    auto typ = componentTypes[format];

    std::unique_ptr<std::byte[]> tmp = std::make_unique<std::byte[]>(byteCount);
    Image::FlipVertical(data.data(), tmp.get(), size, format);

    gl::TexImage2D(gl::TEXTURE_2D, 0, fmt, size.x, size.y, 0, fmt, typ, tmp.get());

    gl::GenerateMipmap(gl::TEXTURE_2D);

    int minFilter = minFilters[TextureFilterMode::Bilinear];
    int magFilter = magFilters[TextureFilterMode::Bilinear];
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, minFilter);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, magFilter);

    auto mode = wrapModes[TextureWrapMode::Repeat];
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, mode);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, mode);

    //gl::GetFloatv(gl::MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    //gl::TexParameterf(gl::TEXTURE_2D, gl::TEXTURE_MAX_ANISOTROPY, anisoLevel);

    gl::BindTexture(gl::TEXTURE_2D, 0);
}

HWTextureOpenGL::~HWTextureOpenGL()
{
    if (gl::IsTexture(textureID))
        gl::DeleteTextures(1, &textureID);
}

void HWTextureOpenGL::SetPixels(const std::span<std::byte>& data, const IntRect& rect)
{
    if (rect.w == 0 || rect.h == 0)
        return;

    if (!dynamic)
        throw Exception("texture is not dynamic");

    auto bytesPerPixel = GetBytesPerPixel(format);

    if (data.size() != rect.w * rect.h * bytesPerPixel)
        throw Exception("incorrect data size");

    gl::BindTexture(gl::TEXTURE_2D, textureID);

    Assert(rect.x >= 0);
    Assert(rect.y >= 0);
    Assert(rect.x + rect.w <= size.x);
    Assert(rect.y + rect.h <= size.y);

    auto fmt = textureFormats[format];
    auto type = componentTypes[format];
    size_t byteCount = rect.w * rect.h * bytesPerPixel;

    std::unique_ptr<std::byte[]> tmp = std::make_unique<std::byte[]>(byteCount);
    Image::FlipVertical(data.data(), tmp.get(), rect.GetSize(), format);

    gl::TexSubImage2D(gl::TEXTURE_2D, 0,
        rect.x, (size.y - 1) - (rect.y + rect.h - 1), rect.w, rect.h,
        fmt, type, tmp.get());

    gl::GenerateMipmap(gl::TEXTURE_2D);

    gl::BindTexture(gl::TEXTURE_2D, 0);
}

void HWTextureOpenGL::SetWrapMode(TextureWrapMode wrapMode)
{
    gl::BindTexture(gl::TEXTURE_2D, textureID);

    auto mode = wrapModes[wrapMode];
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, mode);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, mode);

    gl::BindTexture(gl::TEXTURE_2D, 0);
}

void HWTextureOpenGL::SetFilterMode(TextureFilterMode filterMode)
{
    gl::BindTexture(gl::TEXTURE_2D, textureID);

    int minFilter = minFilters[filterMode];
    int magFilter = magFilters[filterMode];
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, minFilter);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, magFilter);

    gl::BindTexture(gl::TEXTURE_2D, 0);
}

void HWTextureOpenGL::SetAnisoLevel(float level)
{
    //gl::BindTexture(gl::TEXTURE_2D, textureID);

    //auto anisoLevel = std::clamp(level, 1.0f, maxAniso);
    //gl::TexParameterf(gl::TEXTURE_2D, gl::TEXTURE_MAX_ANISOTROPY, anisoLevel);

    //gl::BindTexture(gl::TEXTURE_2D, 0);
}

} // gfx
} // mw
