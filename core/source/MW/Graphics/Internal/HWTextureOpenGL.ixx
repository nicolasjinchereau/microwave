/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWTextureOpenGL;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.Internal.HWBuffer;
import Microwave.Graphics.Internal.HWTexture;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Math;
import Microwave.System.Pointers;
import Microwave.System.Task;
import <cstdint>;
import <cstddef>;
import <memory>;
import <string>;
import <span>;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {
inline namespace gfx {

class HWContextOpenGL;

class HWTextureOpenGL : public HWTexture
{
public:
    gptr<HWContextOpenGL> context;
    gl::Uint textureID = 0;
    float maxAniso = 1;
    IVec2 size;
    PixelDataFormat format;
    bool dynamic = {};

    HWTextureOpenGL(
        const gptr<HWContextOpenGL>& context,
        const IVec2& size,
        PixelDataFormat format,
        bool dynamic,
        const std::span<std::byte>& data);

    HWTextureOpenGL(
        const gptr<HWContextOpenGL>& context,
        const IVec2& size,
        PixelDataFormat format,
        bool dynamic,
        const gptr<HWBuffer>& buffer);

    virtual ~HWTextureOpenGL();

    virtual void SetPixels(const std::span<std::byte>& data, const IntRect& rect) override;
    virtual void SetWrapMode(TextureWrapMode wrapMode) override;
    virtual void SetFilterMode(TextureFilterMode filterMode) override;
    virtual void SetAnisoLevel(float level) override;
};

} // gfx
} // mw
