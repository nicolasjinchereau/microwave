/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWDriverContext;
import Microwave.Graphics.Internal.HWRenderTarget;
import Microwave.System.Pointers;

export namespace mw {

inline namespace system {
class Window;
}

inline namespace gfx {

class HWSurface;

class HWDriverContext
{
public:
    virtual ~HWDriverContext(){}
    virtual void SetActive(bool active) = 0;
    virtual void SetRenderTarget(const gptr<HWRenderTarget>& target) = 0;
    virtual void Flip(const gptr<HWRenderTarget>& target) = 0;
    virtual void SetSwapInterval(int interval) = 0;
    virtual gptr<HWSurface> CreateSurface(const gptr<Window>& window) = 0;
};

} // gfx
} // mw
