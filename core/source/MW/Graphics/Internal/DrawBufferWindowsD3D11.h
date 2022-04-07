/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/Math.h>
#include <MW/System/Pointers.h>
#include <MW/Graphics/DrawBuffer.h>
#include <cstdint>
#include <d3d11_1.h>
#include <span>
#include <string>
#include <wrl/client.h>
#include <vector>

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace mw {
inline namespace gfx {

class GraphicsContextWindowsD3D11;

class DrawBufferWindowsD3D11 : public DrawBuffer
{
public:
    std::vector<std::byte> data;
    DrawBufferType type = DrawBufferType::None;
    uint32_t capacity = 0;
    bool dynamic = false;
    ComPtr<ID3D11Buffer> buffer;
    sptr<GraphicsContextWindowsD3D11> graphics;

    DrawBufferWindowsD3D11(
        const sptr<GraphicsContextWindowsD3D11>& graphics,
        uint32_t capacity,
        DrawBufferType type,
        bool dynamic,
        const std::span<std::byte>& data);

    virtual ~DrawBufferWindowsD3D11();

    virtual void UpdateSubData(uint32_t offset, const std::span<std::byte>& data) override;
    virtual void Clear() override;

    virtual DrawBufferType GetType() const override;
    virtual uint32_t GetCapacity() const override;
    virtual bool IsDynamic() const override;

    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
};

}
}
