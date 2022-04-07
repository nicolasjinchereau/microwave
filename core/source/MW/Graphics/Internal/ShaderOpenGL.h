/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Shader.h>
#include <MW/Graphics/Internal/OpenGLAPI.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace mw {
inline namespace gfx {

class ShaderOpenGL : public Shader
{
public:
    GLuint hProgram = 0;

    ShaderOpenGL(const std::string& source);
    ~ShaderOpenGL();

    virtual void Bind() override;
    virtual void Unbind() override;

    virtual void SetVertexBuffer(const sptr<DrawBuffer>& buffer) override;
    virtual void SetVertexBuffer(int id, sptr<DrawBuffer>& buffer) override;
    virtual void SetVertexBuffer(const std::string& name, sptr<DrawBuffer>& buffer) override;
    virtual void SetVertexBuffer(InputSemantic semantic, int index, sptr<DrawBuffer>& buffer) override;
    virtual void SetIndexBuffer(const sptr<DrawBuffer>& buffer) override;

    virtual void SetUniform(int id, float value) override;
    virtual void SetUniform(int id, const Vec2& value) override;
    virtual void SetUniform(int id, const Vec3& value) override;
    virtual void SetUniform(int id, const Vec4& value) override;
    virtual void SetUniform(int id, const Mat2& value) override;
    virtual void SetUniform(int id, const Mat3& value) override;
    virtual void SetUniform(int id, const Mat4& value) override;
    virtual void SetUniform(int id, const Color& value) override;
    virtual void SetUniform(int id, const sptr<Texture>& texture) override;
    virtual void SetUniform(const std::string& name, float value) override;
    virtual void SetUniform(const std::string& name, const Vec2& value) override;
    virtual void SetUniform(const std::string& name, const Vec3& value) override;
    virtual void SetUniform(const std::string& name, const Vec4& value) override;
    virtual void SetUniform(const std::string& name, const Mat2& value) override;
    virtual void SetUniform(const std::string& name, const Mat3& value) override;
    virtual void SetUniform(const std::string& name, const Mat4& value) override;
    virtual void SetUniform(const std::string& name, const Color& value) override;
    virtual void SetUniform(const std::string& name, const sptr<Texture>& texture) override;

    virtual void AllocateGraphicsResource() override;
    virtual void ReleaseGraphicsResource() override;
    virtual bool IsGraphicsResourceAllocated() const override;
    virtual GraphicsDriverType GetGraphicsDriverType() const override;
};

}
}
