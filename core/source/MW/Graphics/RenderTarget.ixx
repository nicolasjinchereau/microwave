/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.RenderTarget;
import Microwave.Math;
import Microwave.System.Object;

export namespace mw {
inline namespace gfx {

class HWRenderTarget;

class RenderTarget : public Object
{
public:
    virtual ~RenderTarget(){}
    virtual IVec2 GetSize() const = 0;
    virtual sptr<HWRenderTarget> GetHWRenderTarget() = 0;
};

} // gfx
} // mw
