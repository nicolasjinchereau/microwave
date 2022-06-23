/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Buffer;
import Microwave.Graphics.GraphicsContext;
import Microwave.System.Exception;
import <stdexcept>;

namespace mw {
inline namespace gfx {

Buffer::Buffer(BufferType type, BufferUsage usage, BufferCPUAccess cpuAccess, size_t size)
    : type(type), usage(usage), cpuAccess(cpuAccess), size(size)
{
    if (size == 0)
        throw Exception("cannot create an empty buffer");

    auto graphics = GraphicsContext::GetCurrent();
    if (!graphics)
        throw Exception("no active graphics context");

    buffer = graphics->context->CreateBuffer(type, usage, cpuAccess, size);
}

Buffer::Buffer(BufferType type, BufferUsage usage, BufferCPUAccess cpuAccess, const std::span<std::byte>& data)
    : type(type), usage(usage), cpuAccess(cpuAccess), size(data.size())
{
    if (size == 0)
        throw Exception("cannot create an empty buffer");

    auto graphics = GraphicsContext::GetCurrent();
    if (!graphics)
        throw Exception("no active graphics context");

    buffer = graphics->context->CreateBuffer(type, usage, cpuAccess, data);
}

void Buffer::UpdateSubData(size_t offset, const std::span<std::byte>& data)
{
    if (offset + data.size() > size)
        throw Exception("specified range is out of bounds");

    if (usage == BufferUsage::Static)
        throw Exception("cannot update static buffer");

    if (!data.empty())
    {
        std::span<std::byte> mapping = buffer->Map(BufferMapAccess::WriteNoSync);
        std::copy(data.begin(), data.end(), mapping.begin() + offset);
        buffer->Unmap();
    }
}

void Buffer::Clear()
{
    buffer = nullptr;
    type = {};
    usage = {};
    cpuAccess = {};
    size = {};
}

BufferType Buffer::GetType() const {
    return type;
}

BufferUsage Buffer::GetUsage() const {
    return usage;
}

BufferCPUAccess Buffer::GetCPUAccess() const {
    return cpuAccess;
}

size_t Buffer::GetSize() const {
    return size;
}

gptr<HWBuffer> Buffer::GetHWBuffer() const {
    return buffer;
}

} // gfx
} // mw
