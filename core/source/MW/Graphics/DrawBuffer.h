/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/GraphicsResource.h>
#include <MW/Math/Math.h>
#include <MW/System/Pointers.h>
#include <cstdint>
#include <span>
#include <string>

namespace mw {
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
