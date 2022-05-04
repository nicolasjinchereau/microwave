/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWRenderTextureOpenGL;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Graphics.Internal.HWRenderTexture;
import Microwave.Graphics.Internal.HWTexture;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Math;
import Microwave.System.Pointers;
import <MW/System/Internal/PlatformHeaders.h>;

namespace mw {
inline namespace gfx {

class HWTextureOpenGL;

}
}

export namespace mw {
inline namespace gfx {

class HWRenderTextureOpenGL : public HWRenderTexture
{
public:
    sptr<HWTextureOpenGL> tex;
    RenderTextureFormat format = RenderTextureFormat::RGBA32;
    gl::Uint frameBuffer = 0;
    gl::Uint depthBuffer = 0;
    int depthBits = 24;
    IVec2 size;

    HWRenderTextureOpenGL(const sptr<HWTexture>& tex);
    ~HWRenderTextureOpenGL();

    virtual IVec2 GetSize() override;
    virtual sptr<HWTexture> GetTexture() override;
};

} // gfx
} // mw
