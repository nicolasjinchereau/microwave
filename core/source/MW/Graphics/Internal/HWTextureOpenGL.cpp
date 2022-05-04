/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWTextureOpenGL;
import Microwave.Graphics.Internal.HWBufferOpenGL;
import Microwave.Graphics.Internal.HWContextOpenGL;
import Microwave.System.Console;
import Microwave.System.ThreadPool;
import <algorithm>;
import <cassert>;
import <cstring>;
import <regex>;
import <stdexcept>;
import <unordered_map>;
import <MW/System/Internal/PlatformHeaders.h>;

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
    const sptr<HWContextOpenGL>& context,
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
        throw std::runtime_error("incorrect size for 'data'");

    gl::GenTextures(1, &textureID);
    gl::BindTexture(gl::TEXTURE_2D, textureID);

    auto fmt = textureFormats[format];
    auto intFmt = sizedInternalFormats[format];
    auto typ = componentTypes[format];
    gl::TexImage2D(gl::TEXTURE_2D, 0, fmt, size.x, size.y, 0, fmt, typ, data.data());

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

HWTextureOpenGL::HWTextureOpenGL(
    const sptr<HWContextOpenGL>& context,
    const IVec2& size,
    PixelDataFormat format,
    bool dynamic,
    const sptr<HWBuffer>& buffer)
    : context(context)
    , size(size)
    , format(format)
    , dynamic(dynamic)
{
    int bytesPerPixel = GetBytesPerPixel(format);
    int byteCount = size.x * size.y * bytesPerPixel;

    if (buffer->GetSize() != byteCount)
        throw std::runtime_error("incorrect buffer size");

    gl::GenTextures(1, &textureID);
    gl::BindTexture(gl::TEXTURE_2D, textureID);

    auto fmt = textureFormats[format];
    auto intFmt = sizedInternalFormats[format];
    auto typ = componentTypes[format];

    auto buff = spcast<HWBufferOpenGL>(buffer);

    buff->Bind();
    gl::TexImage2D(gl::TEXTURE_2D, 0, fmt, size.x, size.y, 0, fmt, typ, 0);
    buff->ClearBinding();

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
        throw std::runtime_error("texture is not dynamic");

    auto bytesPerPixel = GetBytesPerPixel(format);

    if (data.size() != rect.w * rect.h * bytesPerPixel)
        throw std::runtime_error("incorrect data size");

    gl::BindTexture(gl::TEXTURE_2D, textureID);

    assert(rect.x >= 0);
    assert(rect.y >= 0);
    assert(rect.x + rect.w <= size.x);
    assert(rect.y + rect.h <= size.y);

#if PLATFORM_IOS || PLATFORM_ANDROID
    auto fmt = textureFormats[format];
    auto type = componentTypes[format];
    auto stride = size.x * bytesPerPixel;

    auto pPixel = data.get() + (rect.y * size.x + rect.x) * bytesPerPixel;

    for (int y = 0; y < rect.h; ++y)
    {
        gl::TexSubImage2D(gl::TEXTURE_2D, 0, rect.x, rect.y + y, rect.w, 1, fmt, type, pPixel);
        pPixel += stride;
    }
#else
    // affects reading from client memory
    //gl::PixelStorei(gl::UNPACK_SKIP_ROWS, rect.y);
    //gl::PixelStorei(gl::UNPACK_SKIP_PIXELS, rect.x);
    //gl::PixelStorei(gl::UNPACK_ROW_LENGTH, size.x);
    //gl::PixelStorei(gl::UNPACK_ROW_LENGTH, rect.w);

    auto fmt = textureFormats[format];
    auto type = componentTypes[format];
    gl::TexSubImage2D(gl::TEXTURE_2D, 0, rect.x, rect.y, rect.w, rect.h, fmt, type, data.data());

    //gl::PixelStorei(gl::UNPACK_SKIP_ROWS, 0);
    //gl::PixelStorei(gl::UNPACK_SKIP_PIXELS, 0);
    //gl::PixelStorei(gl::UNPACK_ROW_LENGTH, 0);
#endif

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
