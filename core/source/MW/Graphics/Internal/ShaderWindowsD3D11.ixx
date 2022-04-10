/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.ShaderWindowsD3D11;
import Microwave.Graphics.Color;
import Microwave.Graphics.DrawBuffer;
import Microwave.Graphics.Shader;
import Microwave.Graphics.Texture;
import Microwave.Math;
import Microwave.System.Pointers;
import <cstdint>;
import <string>;
import <vector>;
import <unordered_map>;
import <MW/System/Internal/PlatformHeaders.h>;

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

export namespace mw {
inline namespace gfx {

class GraphicsContextWindowsD3D11;

class ShaderWindowsD3D11 : public Shader
{
public:
    sptr<GraphicsContextWindowsD3D11> graphics;
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;
    ComPtr<ID3D11Buffer> uniformBuffer;
    std::vector<uint8_t> uniformData;

    ShaderWindowsD3D11(
        const sptr<GraphicsContextWindowsD3D11>& graphics,
        const std::string& source);

    ~ShaderWindowsD3D11();
    
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
