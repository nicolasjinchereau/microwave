/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWSurface;
import Microwave.Graphics.Internal.HWRenderTarget;
import Microwave.Math.IVec2;
import Microwave.System.Object;

export namespace mw {
inline namespace gfx {

class HWSurface : public HWRenderTarget
{
public:
    virtual void UpdateSize() = 0;
    virtual IVec2 GetSize() const = 0;
};

} // gfx
} // mw
