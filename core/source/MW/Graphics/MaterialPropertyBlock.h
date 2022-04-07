/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <string>
#include <unordered_map>
#include <MW/Graphics/Color.h>
#include <MW/Graphics/Shader.h>
#include <MW/Graphics/Texture.h>
#include <MW/Math/Math.h>
#include <MW/System/Object.h>
#include <MW/System/path.h>
#include <MW/System/Pointers.h>
#include <MW/System/UUID.h>

namespace mw {
inline namespace gfx {

class MaterialProperty : public Object
{
public:
    virtual ShaderVarType GetUniformType() const = 0;
    virtual void Apply(const sptr<Shader>& shader) = 0;
    virtual void ToJson(json& obj) const = 0;
    virtual void FromJson(const json& obj, ObjectLinker* linker) = 0;
};

class MaterialPropertyBlock : public Object
{
    REGISTER_OBJECT(MaterialPropertyBlock);
public:
    std::unordered_map<std::string, sptr<MaterialProperty>> properties;

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

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;
};

}
}
