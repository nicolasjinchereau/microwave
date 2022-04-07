/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Color.h>
#include <MW/Graphics/DrawBuffer.h>
#include <MW/Graphics/Mesh.h>
#include <MW/Graphics/Material.h>
#include <MW/Math/Math.h>
#include <MW/System/Object.h>
#include <vector>

namespace mw {
inline namespace gfx {

class GizmoUtility
{
public:
    // 1 x 1 x 1
    // pivot: center
    static sptr<Mesh> CreateBox();
    
    // radius: 1
    // pivot: center
    static sptr<Mesh> CreateSphere();
    
    // radius: 1
    // pivot: bottom
    // orientation: +Y
    static sptr<Mesh> CreateHalfSphere();

    // radius: 1
    // height: 1
    // pivot: center
    // orientation: +/-Y
    static sptr<Mesh> CreateCylinder();
};

} // gfx
} // mw
