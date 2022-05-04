/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWSurfaceWGL;
import Microwave.Graphics.Internal.HWSurface;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Math;
import Microwave.System.Pointers;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {

inline namespace system {
class WindowWindows;
}

inline namespace gfx {

class HWContextOpenGL;

class HWSurfaceWGL : public HWSurface
{
public:
    wptr<WindowWindows> window;

    HWSurfaceWGL(const sptr<WindowWindows>& window);
    ~HWSurfaceWGL();

    virtual void UpdateSize() override;
    virtual IVec2 GetSize() const override;
};

} // gfx
} // mw
