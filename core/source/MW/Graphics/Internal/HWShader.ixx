/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWShader;
import Microwave.Graphics.Buffer;
import Microwave.Graphics.Color;
import Microwave.Graphics.Texture;
import Microwave.Graphics.ShaderInfo;
import Microwave.Math;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <cstdint>;

export namespace mw {
inline namespace gfx {

class HWShader : public Object
{
public:
    sptr<ShaderInfo> info;

    HWShader(const sptr<ShaderInfo>& info)
        : info(info){}

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void SetVertexBuffer(int id, const sptr<Buffer>& buffer, size_t offset, size_t stride) = 0;
    virtual void SetIndexBuffer(const sptr<Buffer>& buffer) = 0;
    virtual void SetUniform(int id, float value) = 0;
    virtual void SetUniform(int id, const Vec2& value) = 0;
    virtual void SetUniform(int id, const Vec3& value) = 0;
    virtual void SetUniform(int id, const Vec4& value) = 0;
    virtual void SetUniform(int id, const Mat2& value) = 0;
    virtual void SetUniform(int id, const Mat3& value) = 0;
    virtual void SetUniform(int id, const Mat4& value) = 0;
    virtual void SetUniform(int id, const Color& value) = 0;
    virtual void SetUniform(int id, const sptr<Texture>& texture) = 0;
};

} // gfx
} // mw
