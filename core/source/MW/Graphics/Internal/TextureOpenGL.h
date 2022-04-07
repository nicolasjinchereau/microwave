/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <memory>
#include <string>
#include <cstdint>
#include <MW/Graphics/Texture.h>
#include <MW/Graphics/Internal/OpenGLAPI.h>

namespace mw {
inline namespace gfx {

class TextureOpenGL : public Texture
{
public:
    std::unique_ptr<uint8_t[]> data;
    IVec2 size = IVec2::Zero();
    PixelDataFormat format = PixelDataFormat::RGBA32;
    WrapMode wrapMode = WrapMode::Clamp;
    FilterMode filterMode = FilterMode::Bilinear;
    float maxAniso = 0;
    float anisoLevel = 1;
    bool isPowerOfTwo = true;
    bool dynamic = false;
    GLuint textureID = 0;

    TextureOpenGL(
        const IVec2& size,
        PixelDataFormat format,
        bool dynamic,
        const std::span<std::byte>& data);

    ~TextureOpenGL();

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
