/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.RenderTarget;
import Microwave.Graphics.GraphicsResource;
import Microwave.System.Pointers;
import Microwave.Math;

export namespace mw {
inline namespace gfx {

class GraphicsContext;

class RenderTarget : public GraphicsResource
{
public:
    virtual ~RenderTarget(){}
    virtual void Bind(const sptr<GraphicsContext>& graphics) = 0;
    virtual IVec2 GetSize() = 0;
};

}
}
