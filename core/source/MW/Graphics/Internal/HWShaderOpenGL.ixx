/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWShaderOpenGL;
import Microwave.Graphics.Buffer;
import Microwave.Graphics.Color;
import Microwave.Graphics.Texture;
import Microwave.Graphics.Internal.HWShader;
import Microwave.Graphics.ShaderInfo;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.Math;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <cstdint>;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {
inline namespace gfx {

class HWShaderOpenGL : public HWShader
{
public:
    gl::Uint hProgram = 0;

    HWShaderOpenGL(const gptr<ShaderInfo>& info);
    ~HWShaderOpenGL();

    virtual void Bind() override;
    virtual void Unbind() override;

    virtual void SetVertexBuffer(int id, const gptr<Buffer>& buffer, size_t offset, size_t stride) override;
    virtual void SetIndexBuffer(const gptr<Buffer>& buffer) override;
    
    virtual void SetUniform(int id, float value) override;
    virtual void SetUniform(int id, const Vec2& value) override;
    virtual void SetUniform(int id, const Vec3& value) override;
    virtual void SetUniform(int id, const Vec4& value) override;
    virtual void SetUniform(int id, const Mat2& value) override;
    virtual void SetUniform(int id, const Mat3& value) override;
    virtual void SetUniform(int id, const Mat4& value) override;
    virtual void SetUniform(int id, const Color& value) override;
    virtual void SetUniform(int id, const gptr<Texture>& texture) override;

    bool VerifyShaderCompilation(gl::Uint program, std::string& out_error);
};

} // gfx
} // mw
