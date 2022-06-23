/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWShaderOpenGL;
import Microwave.Graphics.Internal.HWBufferOpenGL;
import Microwave.Graphics.Internal.HWTextureOpenGL;
import Microwave.Graphics.Internal.HWShader;
import Microwave.System.Exception;
import <MW/System/Debug.h>;

namespace mw {
inline namespace gfx {

HWShaderOpenGL::HWShaderOpenGL(const gptr<ShaderInfo>& info)
    : HWShader(info)
{
    const char* vs = info->vertShaderSource.data();
    const char* fs = info->fragShaderSource.data();
    int vert_sizes = (int)info->vertShaderSource.size();
    int frag_sizes = (int)info->fragShaderSource.size();

    gl::Uint vertShaderID = gl::CreateShader(gl::VERTEX_SHADER);
    gl::Uint fragShaderID = gl::CreateShader(gl::FRAGMENT_SHADER);

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
        throw Exception(vs_error);
    }

    if (!ps_compiled)
    {
        gl::DeleteShader(vertShaderID);
        gl::DeleteShader(fragShaderID);
        throw Exception(ps_error);
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
    gl::GetProgramiv(hProgram, gl::LINK_STATUS, &linkStatus);

    if (linkStatus == gl::FALSE)
    {
        gl::DeleteProgram(hProgram);
        throw Exception("error: failed to create shader program");
    }

    for (int i = 0, sz = (int)info->attributes.size(); i < sz; ++i)
    {
        auto& attrib = info->attributes[i];

        int id = gl::GetAttribLocation(hProgram, attrib.identifier.c_str());
        if (id != -1) {
            info->attributes[i].slot = id;
        }
    }

    for (size_t i = 0, sz = (int)info->uniforms.size(); i < sz; ++i)
    {
        auto name = info->uniforms[i].name.c_str();

        int id = gl::GetUniformLocation(hProgram, name);
        if (id != -1)
        {
            info->uniforms[i].slot = id;
            info->uniformIDs[name] = (int)i;
        }
    }
}

HWShaderOpenGL::~HWShaderOpenGL()
{
    if (gl::IsProgram(hProgram))
        gl::DeleteProgram(hProgram);
}

void HWShaderOpenGL::Bind()
{
    gl::UseProgram(hProgram);

    for (auto& att : info->attributes)
        gl::EnableVertexAttribArray(att.slot);
}

void HWShaderOpenGL::Unbind()
{
    for (auto& att : info->attributes)
        gl::DisableVertexAttribArray(att.slot);

    gl::UseProgram(0);
}

void HWShaderOpenGL::SetVertexBuffer(int id, const gptr<Buffer>& buffer, size_t offset, size_t stride)
{
    if (id >= 0)
    {
        Assert(buffer->GetType() == BufferType::Vertex);
        auto pb = gpcast<HWBufferOpenGL>(buffer->GetHWBuffer());
        Assert(pb);

        auto& attrib = info->attributes[id];

        pb->Bind();
        gl::VertexAttribPointer(attrib.slot, attrib.size / 4, gl::FLOAT, gl::FALSE, stride, (void*)offset);
        pb->ClearBinding();
    }
}

void HWShaderOpenGL::SetIndexBuffer(const gptr<Buffer>& buffer)
{
    Assert(buffer->GetType() == BufferType::Index);
    auto pb = gpcast<HWBufferOpenGL>(buffer->GetHWBuffer());
    Assert(pb);
    pb->Bind();
}

void HWShaderOpenGL::SetUniform(int id, float value)
{
    if (id >= 0) {
        gl::Uniform1f(info->uniforms[id].slot, value);
    }
}

void HWShaderOpenGL::SetUniform(int id, const Vec2& value)
{
    if (id >= 0) {
        gl::Uniform2fv(info->uniforms[id].slot, 1, (float*)&value);
    }
}

void HWShaderOpenGL::SetUniform(int id, const Vec3& value)
{
    if (id >= 0) {
        gl::Uniform3fv(info->uniforms[id].slot, 1, (float*)&value);
    }
}

void HWShaderOpenGL::SetUniform(int id, const Vec4& value)
{
    if (id >= 0) {
        gl::Uniform4fv(info->uniforms[id].slot, 1, (float*)&value);
    }
}

void HWShaderOpenGL::SetUniform(int id, const Mat2& value)
{
    if (id >= 0) {
        auto transposed = value.Transposed();
        gl::UniformMatrix2fv(info->uniforms[id].slot, 1, gl::FALSE, (float*)&transposed);
    }
}

void HWShaderOpenGL::SetUniform(int id, const Mat3& value)
{
    if (id >= 0) {
        auto transposed = value.Transposed();
        gl::UniformMatrix3fv(info->uniforms[id].slot, 1, gl::FALSE, (float*)&transposed);
    }
}

void HWShaderOpenGL::SetUniform(int id, const Mat4& value)
{
    if (id >= 0) {
        auto transposed = value.Transposed();
        gl::UniformMatrix4fv(info->uniforms[id].slot, 1, gl::FALSE, (float*)&transposed);
    }
}

void HWShaderOpenGL::SetUniform(int id, const Color& value)
{
    if (id >= 0) {
        gl::Uniform4fv(info->uniforms[id].slot, 1, (float*)&value);
    }
}

void HWShaderOpenGL::SetUniform(int id, const gptr<Texture>& texture)
{
    if (id >= 0)
    {
        gptr<HWTexture> hwTex = texture->GetHWTexture();
        if (!hwTex)
            return;

        auto pt = gpcast<HWTextureOpenGL>(hwTex);
        Assert(pt);

        int textureUnit = id;
        int samplerSlot = info->uniforms[id].slot;
        gl::ActiveTexture(gl::TEXTURE0 + textureUnit);
        gl::BindTexture(gl::TEXTURE_2D, pt->textureID);
        gl::Uniform1i(samplerSlot, textureUnit);
        //gl::BindTexture(gl::TEXTURE_2D, 0);
    }
}

bool HWShaderOpenGL::VerifyShaderCompilation(gl::Uint program, std::string& out_error)
{
    gl::Int status;
    gl::GetShaderiv(program, gl::COMPILE_STATUS, &status);

    if (status == gl::FALSE)
    {
        gl::Int errLength{};
        gl::GetShaderiv(program, gl::INFO_LOG_LENGTH, &errLength);

        if (errLength > 1)
        {
            std::string error;
            error.resize(errLength);
            
            gl::Sizei nChars{};
            gl::GetShaderInfoLog(program, errLength, &nChars, error.data());
            
            if(nChars < (gl::Sizei)error.size())
                error.resize(nChars);

            out_error = std::move(error);
        }
    }

    return status == gl::TRUE;
}

} // gfx
} // mw
