/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/DrawBufferOpenGL.h>
#include <MW/System/Console.h>

namespace mw {
inline namespace gfx {

DrawBufferOpenGL::DrawBufferOpenGL(uint32_t capacity, DrawBufferType type, bool dynamic, const std::span<std::byte>& data)
{
    this->capacity = capacity;
    this->type = type;
    this->dynamic = dynamic;

    this->data.clear();
    this->data.resize(capacity);

    if (!data.empty())
        memcpy(this->data.data(), data.data(), std::min((uint32_t)data.size(), capacity));

    AllocateGraphicsResource();
}

DrawBufferOpenGL::~DrawBufferOpenGL() {
    ReleaseGraphicsResource();
}

void DrawBufferOpenGL::UpdateSubData(uint32_t offset, const std::span<std::byte>& data)
{
    if (!dynamic)
        throw std::runtime_error("buffer is not dynamic");

    if (data.empty())
        throw std::runtime_error("'data' cannot be empty");

    if (offset + data.size() > capacity)
        throw std::runtime_error("data is too large for buffer");

    memcpy(this->data.data() + offset, data.data(), data.size());

    if (hBuffer != 0)
    {
        GLenum bufferTypes[3] = { 0, GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER };
        GLenum bufferType = bufferTypes[(int)type];

        gl::BindBuffer(bufferType, hBuffer);

        gl::BufferSubData(bufferType, offset, data.size(), data.data());
        //        uint8_t* pBufferData = (uint8_t*)gl::MapBuffer(bufferType, GL_WRITE_ONLY);
        //        memcpy(pBufferData + offset, data, size);
        //        gl::UnmapBuffer(bufferType);

        gl::BindBuffer(bufferType, 0);
    }
}

void DrawBufferOpenGL::Clear()
{
    capacity = 0;
    type = DrawBufferType::None;
    dynamic = false;
    data.clear();
    ReleaseGraphicsResource();
}

DrawBufferType DrawBufferOpenGL::GetType() const {
    return type;
}

uint32_t DrawBufferOpenGL::GetCapacity() const {
    return capacity;
}

bool DrawBufferOpenGL::IsDynamic() const {
    return dynamic;
}

void DrawBufferOpenGL::AllocateGraphicsResource()
{
    if (hBuffer != 0 || capacity == 0 || type == DrawBufferType::None)
        return;

    GLenum bufferTypes[3] = { 0, GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER };
    GLenum bufferType = bufferTypes[(int)type];

    gl::GenBuffers(1, &hBuffer);
    gl::BindBuffer(bufferType, hBuffer);
    gl::BufferData(bufferType, capacity, nullptr, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    gl::BufferSubData(bufferType, 0, capacity, data.data());
    //void *pBufferData = gl::MapBuffer(bufferType, GL_WRITE_ONLY);
    //memcpy(pBufferData, data.data(), capacity);
    //gl::UnmapBuffer(bufferType);

    gl::BindBuffer(bufferType, 0);
}

void DrawBufferOpenGL::ReleaseGraphicsResource()
{
    if (hBuffer == 0)
        return;

    gl::DeleteBuffers(1, &hBuffer);
    hBuffer = 0;
}

bool DrawBufferOpenGL::IsGraphicsResourceAllocated() const {
    return hBuffer != 0;
}

GraphicsDriverType DrawBufferOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

}
}
