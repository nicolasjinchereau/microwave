/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWShaderD3D11;
import Microwave.Graphics.Buffer;
import Microwave.Graphics.Color;
import Microwave.Graphics.Texture;
import Microwave.Graphics.Internal.HWShader;
import Microwave.Math;
import Microwave.System.Console;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <algorithm>;
import <cstdint>;
import <cassert>;
import <stdexcept>;
import <string>;
import <MW/System/Internal/PlatformHeaders.h>;

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

export namespace mw {
inline namespace gfx {

class HWContextD3D11;

class HWShaderD3D11 : public HWShader
{
public:
    sptr<HWContextD3D11> context;
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;
    ComPtr<ID3D11Buffer> uniformBuffer;
    std::vector<uint8_t> uniformData;

    HWShaderD3D11(
        const sptr<HWContextD3D11>& context,
        const sptr<ShaderInfo>& info);

    ~HWShaderD3D11();

    virtual void Bind() override;
    virtual void Unbind() override;

    virtual void SetVertexBuffer(int id, const sptr<Buffer>& buffer, size_t offset, size_t stride) override;
    virtual void SetIndexBuffer(const sptr<Buffer>& buffer) override;

    virtual void SetUniform(int id, float value) override;
    virtual void SetUniform(int id, const Vec2& value) override;
    virtual void SetUniform(int id, const Vec3& value) override;
    virtual void SetUniform(int id, const Vec4& value) override;
    virtual void SetUniform(int id, const Mat2& value) override;
    virtual void SetUniform(int id, const Mat3& value) override;
    virtual void SetUniform(int id, const Mat4& value) override;
    virtual void SetUniform(int id, const Color& value) override;
    virtual void SetUniform(int id, const sptr<Texture>& texture) override;
};

} // gfx
} // mw
