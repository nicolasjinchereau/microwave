/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWSurfaceEGL;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Graphics.Internal.HWSurface;
import Microwave.Math;
import Microwave.System.Pointers;
import Microwave.System.Internal.WindowAndroid;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {
inline namespace gfx {

class HWContextEGL;

class HWSurfaceEGL : public HWSurface
{
public:
    sptr<HWContextEGL> context;
    wptr<WindowAndroid> window;

    EGLSurface surface = EGL_NO_SURFACE;

    HWSurfaceEGL(
        const sptr<HWContextEGL>& context,
        const sptr<WindowAndroid>& window);

    ~HWSurfaceEGL();

    virtual void UpdateSize() override;
    virtual IVec2 GetSize() const override;
};

} // gfx
} // mw
