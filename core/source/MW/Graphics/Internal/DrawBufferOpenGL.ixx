/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.DrawBufferOpenGL;
import Microwave.Math;
import Microwave.System.Pointers;
import Microwave.Graphics.DrawBuffer;
import Microwave.Graphics.Internal.OpenGLAPI;
import <cstddef>;
import <cstdint>;
import <span>;
import <string>;
import <vector>;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {
inline namespace gfx {

class DrawBufferOpenGL : public DrawBuffer
{
public:
    std::vector<std::byte> data;
    DrawBufferType type = DrawBufferType::None;
    uint32_t capacity = 0;
    bool dynamic = false;
    GLuint hBuffer = 0;

    DrawBufferOpenGL(
        uint32_t capacity,
        DrawBufferType type,
        bool dynamic,
        const std::span<std::byte>& data);

    virtual ~DrawBufferOpenGL();

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

} // gfx
} // mw
