/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Buffer;
import Microwave.Graphics.Internal.HWBuffer;
import Microwave.Math;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <cstddef>;
import <cstdint>;
import <span>;

export namespace mw {
inline namespace gfx {

class Buffer : public Object
{
    sptr<HWBuffer> buffer;
    BufferType type = {};
    BufferUsage usage = {};
    BufferCPUAccess cpuAccess = {};
    size_t size = {};
public:
    Buffer(BufferType type, BufferUsage usage, BufferCPUAccess cpuAccess, size_t size);
    Buffer(BufferType type, BufferUsage usage, BufferCPUAccess cpuAccess, const std::span<std::byte>& data);

    void UpdateSubData(size_t offset, const std::span<std::byte>& data);
    void Clear();

    BufferType GetType() const;
    BufferUsage GetUsage() const;
    BufferCPUAccess GetCPUAccess() const;
    size_t GetSize() const;
    sptr<HWBuffer> GetHWBuffer() const;
};

} // gfx
} // mw
