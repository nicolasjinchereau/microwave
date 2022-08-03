/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.RenderTexture;
import Microwave.Graphics.RenderTarget;
import Microwave.Graphics.Texture;

export namespace mw {
inline namespace gfx {

class HWRenderTarget;
class HWRenderTexture;
class HWTexture;

class RenderTexture : public Texture, public RenderTarget
{
    gptr<HWRenderTexture> renderTexture;
public:
    RenderTexture(const IVec2& size);

    virtual ~RenderTexture(){};
    virtual IVec2 GetSize() const override;
    virtual gptr<HWRenderTarget> GetHWRenderTarget() override;
};

} // gfx
} // mw
