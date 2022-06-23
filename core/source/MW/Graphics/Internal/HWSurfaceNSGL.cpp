/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWSurfaceNSGL;
import Microwave.Graphics.Internal.HWContextNSGL;

namespace mw {
inline namespace gfx {

HWSurfaceNSGL::HWSurfaceNSGL(
    const gptr<HWContextNSGL>& context,
    const gptr<WindowMacOS>& window)
    : context(context)
    , window(window)
{

}

HWSurfaceNSGL::~HWSurfaceNSGL()
{
    
}

void HWSurfaceNSGL::UpdateSize()
{
    
}

IVec2 HWSurfaceNSGL::GetSize() const
{
    return size;
}

} // gfx
} // mw

