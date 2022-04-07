/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Internal/DrawBufferOpenGL.h>
#include <MW/Graphics/Internal/ShaderOpenGL.h>
#include <MW/Graphics/Internal/TextureOpenGL.h>
#include <MW/System/Console.h>
#include <cassert>
#include <regex>
#include <algorithm>

namespace mw {
inline namespace gfx {

ShaderOpenGL::ShaderOpenGL(const std::string& source)
#if PLATFORM_IOS || PLATFORM_ANDROID
    : Shader(source, ShaderLanguage::GLSL_100)
#elif PLATFORM_MACOS || PLATFORM_WINDOWS
    : Shader(source, ShaderLanguage::GLSL_150)
#endif
{
    AllocateGraphicsResource();
}

ShaderOpenGL::~ShaderOpenGL()
{
    ReleaseGraphicsResource();
}

void ShaderOpenGL::Bind()
{
    gl::UseProgram(hProgram);

    for (auto& att : info.attributes)
        gl::EnableVertexAttribArray(att.slot);
}

void ShaderOpenGL::Unbind()
{
    for (auto& att : info.attributes)
        gl::DisableVertexAttribArray(att.slot);

    gl::UseProgram(0);
}

void ShaderOpenGL::SetVertexBuffer(const sptr<DrawBuffer>& buffer)
{
    auto pb = std::dynamic_pointer_cast<DrawBufferOpenGL>(buffer);

    gl::BindBuffer(GL_ARRAY_BUFFER, pb ? pb->hBuffer : 0);

    size_t offset = 0;

    for (auto& att : info.attributes)
    {
        gl::VertexAttribPointer(att.slot, att.size / 4, GL_FLOAT, GL_FALSE, info.attribStride, (const GLvoid*)offset);
        offset += att.size;
    }

    gl::BindBuffer(GL_ARRAY_BUFFER, 0);
}

void ShaderOpenGL::SetVertexBuffer(int id, sptr<DrawBuffer>& buffer)
{
    if (id >= 0)
    {
        auto pb = std::dynamic_pointer_cast<DrawBufferOpenGL>(buffer);

        int location = info.attributes[id].slot;
        gl::BindBuffer(GL_ARRAY_BUFFER, pb ? pb->hBuffer : 0);
        gl::VertexAttribPointer(location, info.attributes[id].size / 4, GL_FLOAT, GL_FALSE, 0, 0);
        gl::BindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void ShaderOpenGL::SetVertexBuffer(const std::string& name, sptr<DrawBuffer>& buffer) {
    SetVertexBuffer(GetAttributeID(name), buffer);
}

void ShaderOpenGL::SetVertexBuffer(InputSemantic semantic, int index, sptr<DrawBuffer>& buffer) {
    SetVertexBuffer(GetAttributeID(semantic, index), buffer);
}

void ShaderOpenGL::SetIndexBuffer(const sptr<DrawBuffer>& buffer)
{
    auto pb = std::dynamic_pointer_cast<DrawBufferOpenGL>(buffer);
    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, pb ? pb->hBuffer : 0);
}

void ShaderOpenGL::SetUniform(int id, float value) {
    if (id >= 0) {
        gl::Uniform1f(info.uniforms[id].slot, value);
    }
}

void ShaderOpenGL::SetUniform(int id, const Vec2& value) {
    if (id >= 0) {
        gl::Uniform2fv(info.uniforms[id].slot, 1, (float*)&value);
    }
}

void ShaderOpenGL::SetUniform(int id, const Vec3& value) {
    if (id >= 0) {
        gl::Uniform3fv(info.uniforms[id].slot, 1, (float*)&value);
    }
}

void ShaderOpenGL::SetUniform(int id, const Vec4& value) {
    if (id >= 0) {
        gl::Uniform4fv(info.uniforms[id].slot, 1, (float*)&value);
    }
}

void ShaderOpenGL::SetUniform(int id, const Mat2& value) {
    if (id >= 0) {
        auto transposed = value.Transposed();
        gl::UniformMatrix2fv(info.uniforms[id].slot, 1, GL_FALSE, (float*)&transposed);
    }
}

void ShaderOpenGL::SetUniform(int id, const Mat3& value) {
    if (id >= 0) {
        auto transposed = value.Transposed();
        gl::UniformMatrix3fv(info.uniforms[id].slot, 1, GL_FALSE, (float*)&transposed);
    }
}

void ShaderOpenGL::SetUniform(int id, const Mat4& value) {
    if (id >= 0) {
        auto transposed = value.Transposed();
        gl::UniformMatrix4fv(info.uniforms[id].slot, 1, GL_FALSE, (float*)&transposed);
    }
}

void ShaderOpenGL::SetUniform(int id, const Color& value) {
    if (id >= 0) {
        gl::Uniform4fv(info.uniforms[id].slot, 1, (float*)&value);
    }
}

void ShaderOpenGL::SetUniform(int id, const sptr<Texture>& texture)
{
    if (id >= 0)
    {
        auto pt = std::dynamic_pointer_cast<TextureOpenGL>(texture);

        int textureUnit = id;
        int samplerSlot = info.uniforms[id].slot;
        gl::ActiveTexture(GL_TEXTURE0 + textureUnit);
        gl::BindTexture(GL_TEXTURE_2D, pt ? pt->textureID : 0);
        gl::Uniform1i(samplerSlot, textureUnit);
    }
}

void ShaderOpenGL::SetUniform(const std::string& name, float value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderOpenGL::SetUniform(const std::string& name, const Vec2& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderOpenGL::SetUniform(const std::string& name, const Vec3& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderOpenGL::SetUniform(const std::string& name, const Vec4& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderOpenGL::SetUniform(const std::string& name, const Mat2& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderOpenGL::SetUniform(const std::string& name, const Mat3& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderOpenGL::SetUniform(const std::string& name, const Mat4& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderOpenGL::SetUniform(const std::string& name, const Color& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderOpenGL::SetUniform(const std::string& name, const sptr<Texture>& texture) {
    SetUniform(GetUniformID(name), texture);
}

bool VerifyShaderCompilation(uint32_t shader_id, std::string& out_error)
{
    GLint status;
    gl::GetShaderiv(shader_id, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE)
    {
        GLint errLength;
        gl::GetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &errLength);

        if (errLength > 1)
        {
            std::unique_ptr<char[]> error(new char[errLength]);
            GLsizei nChars;
            gl::GetShaderInfoLog(shader_id, errLength, &nChars, error.get());
            out_error = (const char*)error.get();
        }
    }

    return status == GL_TRUE;
}

void ShaderOpenGL::AllocateGraphicsResource()
{
    const char* vs = info.vertShaderSource.data();
    const char* fs = info.fragShaderSource.data();
    int vert_sizes = (int)info.vertShaderSource.size();
    int frag_sizes = (int)info.fragShaderSource.size();

    GLuint vertShaderID = gl::CreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderID = gl::CreateShader(GL_FRAGMENT_SHADER);

    gl::ShaderSource(vertShaderID, 1, (const char**)&vs, &vert_sizes);
    gl::ShaderSource(fragShaderID, 1, (const char**)&fs, &frag_sizes);

    gl::CompileShader(vertShaderID);
    gl::CompileShader(fragShaderID);

    std::string vs_error;
    std::string ps_error;
    bool vs_compiled = VerifyShaderCompilation(vertShaderID, vs_error);
    bool ps_compiled = VerifyShaderCompilation(fragShaderID, ps_error);

    if (!vs_compiled)
    {
        gl::DeleteShader(vertShaderID);
        gl::DeleteShader(fragShaderID);
        throw std::runtime_error(vs_error);
    }

    if (!ps_compiled)
    {
        gl::DeleteShader(vertShaderID);
        gl::DeleteShader(fragShaderID);
        throw std::runtime_error(ps_error);
    }

    hProgram = gl::CreateProgram();

    gl::AttachShader(hProgram, vertShaderID);
    gl::DeleteShader(vertShaderID);

    gl::AttachShader(hProgram, fragShaderID);
    gl::DeleteShader(fragShaderID);

    //#if MW_OSX
    //glBindFragDataLocation(hProgram, 0, "gl_FragData");
    //#endif

    gl::LinkProgram(hProgram);

    int linkStatus;
    gl::GetProgramiv(hProgram, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE)
    {
        gl::DeleteProgram(hProgram);
        throw std::runtime_error("error: failed to create shader program");
    }

    for (int i = 0, sz = (int)info.attributes.size(); i < sz; ++i)
    {
        auto& attrib = info.attributes[i];

        int id = gl::GetAttribLocation(hProgram, attrib.semantic.c_str());
        if (id != -1)
        {
            info.attributes[i].slot = id;
            info.attribIDs[attrib.name] = i;
        }
    }

    for (size_t i = 0, sz = (int)info.uniforms.size(); i < sz; ++i)
    {
        auto name = info.uniforms[i].name.c_str();

        int id = gl::GetUniformLocation(hProgram, name);
        if (id != -1)
        {
            info.uniforms[i].slot = id;
            info.uniformIDs[name] = (int)i;
        }
    }
}

void ShaderOpenGL::ReleaseGraphicsResource()
{
    for (auto& att : info.attributes)
        att.slot = -1;

    for (auto& attID : info.attribIDs)
        attID.second = -1;

    for (auto& uni : info.uniforms)
        uni.slot = -1;

    for (auto& uniID : info.uniformIDs)
        uniID.second = -1;

    if (gl::IsProgram(hProgram)) {
        gl::DeleteProgram(hProgram);
        hProgram = 0;
    }
}

bool ShaderOpenGL::IsGraphicsResourceAllocated() const {
    return hProgram != 0;
}

GraphicsDriverType ShaderOpenGL::GetGraphicsDriverType() const {
    return GraphicsDriverType::OpenGL;
}

}
}
