/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.WindowSurface;
import Microwave.Graphics.RenderTarget;
import Microwave.Graphics.GraphicsResource;
import Microwave.System.Pointers;

export namespace mw {
inline namespace gfx {

class WindowSurface : public RenderTarget
{
public:
    virtual ~WindowSurface() {}
    virtual void UpdateSize() = 0;
};

} // gfx
} // mw
