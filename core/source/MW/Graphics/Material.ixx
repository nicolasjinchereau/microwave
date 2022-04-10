/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Material;
import Microwave.Graphics.Color;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.MaterialPropertyBlock;
import Microwave.Graphics.RenderQueue;
import Microwave.Graphics.Shader;
import Microwave.Graphics.Texture;
import Microwave.Math;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <cstdint>;
import <string>;

export namespace mw {
inline namespace gfx {

// *** Material Property Names ***
// 
// uCameraPos : Vec4
// uLightPos : Vec4
// uAmbientColor : Vec4
// uLightColor : Vec4;
// uDiffuseTex : sptr<Texture>
// uDiffuseTex_ST : Vec4
// uDiffuseFactor : float
// uDiffuseColor : Color
// uNormalTex : sptr<Texture>
// uNormalTex_ST : Vec4
// uNormalFactor : float
// uSpecularTex : sptr<Texture>
// uSpecularTex_ST : Vec4
// uSpecularFactor : float
// uSpecularColor : Color
// uShininess : float
// uMtxModel : Mat4
// uMtxMVP : Mat4
// uMtxNormal : Mat4

class Material : public Object
{
    inline static Type::Pin<Material> pin;
public:
    BlendOperation colorBlendOperation = BlendOperation::Add;
    BlendOperation alphaBlendOperation = BlendOperation::Add;
    BlendFactor sourceColorBlendFactor = BlendFactor::One;
    BlendFactor sourceAlphaBlendFactor = BlendFactor::One;
    BlendFactor destColorBlendFactor = BlendFactor::Zero;
    BlendFactor destAlphaBlendFactor = BlendFactor::Zero;
    CullMode cullMode = CullMode::Back;
    DepthTest depthTest = DepthTest::LessOrEqual;
    bool depthWriteEnabled = true;
    uint32_t renderQueue = RenderQueue::Opaque;
    sptr<MaterialPropertyBlock> properties = spnew<MaterialPropertyBlock>();
    sptr<Shader> shader;

    Material(){}

    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, const Vec2& value);
    void SetUniform(const std::string& name, const Vec3& value);
    void SetUniform(const std::string& name, const Vec4& value);
    void SetUniform(const std::string& name, const Mat2& value);
    void SetUniform(const std::string& name, const Mat3& value);
    void SetUniform(const std::string& name, const Mat4& value);
    void SetUniform(const std::string& name, const Color& value);
    void SetUniform(const std::string& name, const sptr<Texture>& value);
    sptr<Texture> GetTexture(const std::string& name);

    void SetBlendFactors(BlendFactor src, BlendFactor dest);

    void SetActive(const MaterialPropertyBlock* extra = nullptr);

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;
};

void Material::SetUniform(const std::string& name, float value) {
    properties->SetUniform(name, value);
}

void Material::SetUniform(const std::string& name, const Vec2& value) {
    properties->SetUniform(name, value);
}

void Material::SetUniform(const std::string& name, const Vec3& value) {
    properties->SetUniform(name, value);
}

void Material::SetUniform(const std::string& name, const Vec4& value) {
    properties->SetUniform(name, value);
}

void Material::SetUniform(const std::string& name, const Mat2& value) {
    properties->SetUniform(name, value);
}

void Material::SetUniform(const std::string& name, const Mat3& value) {
    properties->SetUniform(name, value);
}

void Material::SetUniform(const std::string& name, const Mat4& value) {
    properties->SetUniform(name, value);
}

void Material::SetUniform(const std::string& name, const Color& value) {
    properties->SetUniform(name, value);
}

void Material::SetUniform(const std::string& name, const sptr<Texture>& value) {
    properties->SetUniform(name, value);
}

sptr<Texture> Material::GetTexture(const std::string& name) {
    return properties->GetTexture(name);
}

void Material::SetBlendFactors(BlendFactor src, BlendFactor dest)
{
    sourceColorBlendFactor = src;
    sourceAlphaBlendFactor = src;
    destColorBlendFactor = dest;
    destAlphaBlendFactor = dest;
}

} // gfx
} // mw
