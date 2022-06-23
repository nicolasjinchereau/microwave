/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#import <AppKit/AppKit.h>

export module Microwave.Graphics.Internal.HWSurfaceNSGL;
import Microwave.Graphics.Internal.HWSurface;
import Microwave.Math;
import Microwave.System.Pointers;
import Microwave.System.Internal.WindowMacOS;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {
inline namespace gfx {

class HWContextNSGL;

class HWSurfaceNSGL : public HWSurface
{
public:
    gptr<HWContextNSGL> context;
    wgptr<WindowMacOS> window;

    IVec2 size;
    int depthBits = 0;

    HWSurfaceNSGL(
        const gptr<HWContextNSGL>& context,
        const gptr<WindowMacOS>& window);

    ~HWSurfaceNSGL();

    virtual void UpdateSize() override;
    virtual IVec2 GetSize() const override;
};

} // gfx
} // mw
