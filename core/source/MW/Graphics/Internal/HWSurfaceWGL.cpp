/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWSurfaceWGL;
import Microwave.Graphics.Internal.HWContextOpenGL;
import Microwave.System.Internal.WindowWindows;

namespace mw {
inline namespace gfx {

HWSurfaceWGL::HWSurfaceWGL(const sptr<WindowWindows>& window)
    : window(spcast<WindowWindows>(window))
{

}

HWSurfaceWGL::~HWSurfaceWGL()
{
    
}

void HWSurfaceWGL::UpdateSize()
{
    
}

IVec2 HWSurfaceWGL::GetSize() const {
    return window.lock()->GetSize();
}

} // gfx
} // mw
