/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#import <UIKit/UIKit.h>

export module Microwave.Graphics.Internal.HWSurfaceEAGL;
import Microwave.Graphics.Internal.HWSurface;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Math;
import Microwave.System.Pointers;
import Microwave.System.Internal.WindowIOS;

export namespace mw {
inline namespace gfx {

class HWContextEAGL;

class HWSurfaceEAGL : public HWSurface
{
public:
    sptr<HWContextEAGL> context;
    wptr<WindowIOS> window;
    IVec2 size;

    int depthBits = 0;
    gl::Uint frameBuffer = 0;
    gl::Uint colorBuffer = 0;
    gl::Uint depthBuffer = 0;

    HWSurfaceEAGL(
        const sptr<HWContextEAGL>& context,
        const sptr<WindowIOS>& window);

    ~HWSurfaceEAGL();

    virtual void UpdateSize() override;
    virtual IVec2 GetSize() const override;
};

} // gfx
} // mw
