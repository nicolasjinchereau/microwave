/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.GraphicsResource;
import Microwave.Graphics.GraphicsTypes;
import Microwave.System.Object;

export namespace mw {
inline namespace gfx {

class GraphicsResource : public Object
{
public:
    virtual ~GraphicsResource() = default;
    virtual void AllocateGraphicsResource() = 0;
    virtual void ReleaseGraphicsResource() = 0;
    virtual bool IsGraphicsResourceAllocated() const = 0;
    virtual GraphicsDriverType GetGraphicsDriverType() const = 0;
};

} // gfx
} // mw
