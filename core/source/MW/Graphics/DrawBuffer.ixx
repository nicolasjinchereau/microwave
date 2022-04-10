/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.DrawBuffer;
import Microwave.Graphics.GraphicsResource;
import Microwave.Math;
import Microwave.System.Pointers;
import <cstddef>;
import <cstdint>;
import <span>;
import <string>;

export namespace mw {
inline namespace gfx {

enum class DrawBufferType : int
{
    None,
    Vertex,
    Index,
};

class DrawBuffer : public GraphicsResource
{
public:
    virtual ~DrawBuffer() {}

    //virtual void SetData(const std::span<std::byte>& data) = 0; // update if possible, or set
    virtual void UpdateSubData(uint32_t offset, const std::span<std::byte>& data) = 0;
    virtual void Clear() = 0;
    
    virtual DrawBufferType GetType() const = 0;
    virtual uint32_t GetCapacity() const = 0;
    virtual bool IsDynamic() const = 0;
};

}
}
