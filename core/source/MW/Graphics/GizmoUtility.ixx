/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.GizmoUtility;
import Microwave.Graphics.Mesh;
import Microwave.Math;
import Microwave.System.Object;
import Microwave.System.Pointers;

export namespace mw {
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
