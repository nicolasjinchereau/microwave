/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWRenderTexture;
import Microwave.Graphics.Internal.HWRenderTarget;
import Microwave.Graphics.Internal.HWTexture;
import Microwave.Math;
import Microwave.System.Object;

export namespace mw {
inline namespace gfx {

class HWRenderTexture : public HWRenderTarget
{
public:
    virtual ~HWRenderTexture(){};
    virtual IVec2 GetSize() = 0;
    virtual sptr<HWTexture> GetTexture() = 0;
};

} // gfx
} // mw
