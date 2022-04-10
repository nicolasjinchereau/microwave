/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.DrawBufferWindowsD3D11;
import Microwave.Graphics.Internal.GraphicsContextWindowsD3D11;
import Microwave.System.Console;
import Microwave.Utilities.Format;
import <algorithm>;
import <cstring>;
import <unordered_map>;
import <MW/System/Internal/PlatformHeaders.h>;

namespace mw {
inline namespace gfx {

std::unordered_map<DrawBufferType, UINT> bufferTypes = {
    { DrawBufferType::Vertex, D3D11_BIND_VERTEX_BUFFER },
    { DrawBufferType::Index, D3D11_BIND_INDEX_BUFFER }
};

DrawBufferWindowsD3D11::DrawBufferWindowsD3D11(
    const sptr<GraphicsContextWindowsD3D11>& graphics,
    uint32_t capacity,
    DrawBufferType type,
    bool dynamic,
    const std::span<std::byte>& data)
    : graphics(graphics)
{
    this->capacity = capacity;
    this->type = type;
    this->dynamic = dynamic;

    this->data.clear();
    this->data.resize(capacity);

    if (!data.empty()) {
        memcpy(this->data.data(), data.data(), std::min((uint32_t)data.size(), capacity));
    }

    AllocateGraphicsResource();
}

DrawBufferWindowsD3D11::~DrawBufferWindowsD3D11() {
    ReleaseGraphicsResource();
}

void DrawBufferWindowsD3D11::UpdateSubData(uint32_t offset, const std::span<std::byte>& data)
{
    if (!dynamic)
        throw std::runtime_error("buffer is not dynamic");

    if(data.empty())
        throw std::runtime_error("'data' cannot be empty");

    if (offset + data.size() > capacity)
        throw std::runtime_error("data is too large for buffer");

    memcpy(this->data.data() + offset, data.data(), data.size());

    if (buffer)
    {
        //D3D11_BOX box;
        //box.left = offset;
        //box.right = offset + size;
        //box.top = 0;
        //box.bottom = 1;
        //box.front = 0;
        //box.back = 1;
        //graphics->context->UpdateSubresource(buffer.Get(), 0, &box, data, 0, 0);

        D3D11_MAPPED_SUBRESOURCE resource;
        HRESULT res = graphics->context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
        if(FAILED(res))
            throw std::runtime_error(format("Failed to map buffer: %", GetLastError()));
        
        void *start = (void*)(((char*)resource.pData) + offset);
        memcpy(start, data.data(), data.size());
        graphics->context->Unmap(buffer.Get(), 0);
    }
}

void DrawBufferWindowsD3D11::Clear()
{
    capacity = 0;
    type = DrawBufferType::None;
    dynamic = false;
    this->data.clear();
    ReleaseGraphicsResource();
}

DrawBufferType DrawBufferWindowsD3D11::GetType() const {
    return type;
}
uint32_t DrawBufferWindowsD3D11::GetCapacity() const {
    return capacity;
}

bool DrawBufferWindowsD3D11::IsDynamic() const {
    return dynamic;
}

void DrawBufferWindowsD3D11::AllocateGraphicsResource()
{
    if(buffer || capacity == 0 || type == DrawBufferType::None)
        return;

    D3D11_SUBRESOURCE_DATA bufferData;
    bufferData.pSysMem = data.data();
    bufferData.SysMemPitch = 0;
    bufferData.SysMemSlicePitch = 0;

    UINT bufferType = bufferTypes[type];
    D3D11_USAGE usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
    UINT cpuAccess = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    
    CD3D11_BUFFER_DESC bufferDesc(capacity, bufferType, usage, cpuAccess);

    HRESULT res = graphics->device->CreateBuffer(&bufferDesc, &bufferData, &buffer);
    if(FAILED(res))
        throw std::runtime_error("could not create buffer");
}

void DrawBufferWindowsD3D11::ReleaseGraphicsResource() {
    buffer = nullptr;
}

bool DrawBufferWindowsD3D11::IsGraphicsResourceAllocated() const
{
    return !!buffer;
}

GraphicsDriverType DrawBufferWindowsD3D11::GetGraphicsDriverType() const {
    return GraphicsDriverType::Direct3D11;
}

} // gfx
} // mw
