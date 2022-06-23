/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.MaterialPropertyBlock;
import Microwave.Graphics.Color;
import Microwave.Graphics.Shader;
import Microwave.Graphics.Texture;
import Microwave.Graphics.ShaderInfo;
import Microwave.Math;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.UUID;
import <string>;
import <unordered_map>;

export namespace mw {
inline namespace gfx {

class MaterialProperty : public Object
{
public:
    virtual ShaderVarType GetUniformType() const = 0;
    virtual void Apply(const gptr<Shader>& shader) = 0;
    virtual void ToJson(json& obj) const = 0;
    virtual void FromJson(const json& obj, ObjectLinker* linker) = 0;
};

class MaterialPropertyBlock : public Object
{
    inline static Type::Pin<MaterialPropertyBlock> pin;
public:

    MaterialPropertyBlock(){}

    gmap<std::string, gptr<MaterialProperty>> properties;

    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, const Vec2& value);
    void SetUniform(const std::string& name, const Vec3& value);
    void SetUniform(const std::string& name, const Vec4& value);
    void SetUniform(const std::string& name, const Mat2& value);
    void SetUniform(const std::string& name, const Mat3& value);
    void SetUniform(const std::string& name, const Mat4& value);
    void SetUniform(const std::string& name, const Color& value);
    void SetUniform(const std::string& name, const gptr<Texture>& value);

    gptr<Texture> GetTexture(const std::string& name);

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;
};

} // gfx
} // mw
