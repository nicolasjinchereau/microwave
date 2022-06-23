/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.RenderTexture;
import Microwave.Graphics.RenderTarget;

export namespace mw {
inline namespace gfx {

class HWRenderTarget;
class HWRenderTexture;

class RenderTexture : public RenderTarget
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
