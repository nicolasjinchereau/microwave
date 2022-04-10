/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.MaterialPropertyBlock;
import Microwave.Graphics.GraphicsContext;

namespace mw {
inline namespace gfx {

template<class T> struct ShaderVarLangType { const static ShaderVarType type = ShaderVarType::Unknown; };
template<> struct ShaderVarLangType<float> { const static ShaderVarType type = ShaderVarType::Float; };
template<> struct ShaderVarLangType<Vec2> { const static ShaderVarType type = ShaderVarType::Float2; };
template<> struct ShaderVarLangType<Vec3> { const static ShaderVarType type = ShaderVarType::Float3; };
template<> struct ShaderVarLangType<Vec4> { const static ShaderVarType type = ShaderVarType::Float4; };
template<> struct ShaderVarLangType<Mat2> { const static ShaderVarType type = ShaderVarType::Float2x2; };
template<> struct ShaderVarLangType<Mat3> { const static ShaderVarType type = ShaderVarType::Float3x3; };
template<> struct ShaderVarLangType<Mat4> { const static ShaderVarType type = ShaderVarType::Float4x4; };
template<> struct ShaderVarLangType<Color> { const static ShaderVarType type = ShaderVarType::Float4; };
template<> struct ShaderVarLangType<sptr<Texture>> { const static ShaderVarType type = ShaderVarType::Sampler2D; };

template<class T>
class MaterialPropertyT : public MaterialProperty
{
public:
    typedef T value_type;
    constexpr static ShaderVarType var_type = ShaderVarLangType<T>::type;

    T value = {};

    virtual ShaderVarType GetUniformType() const override {
        return ShaderVarLangType<T>::type;
    }

    virtual void Apply(const sptr<Shader>& shader) override {
        shader->SetUniform(name, value);
    }

    virtual void ToJson(json& obj) const override {
        Object::ToJson(obj);
        obj["value"] = value;
    }

    virtual void FromJson(const json& obj, ObjectLinker* linker) override {
        Object::FromJson(obj, linker);
        value = obj.value("value", value);
    }
};

class MaterialPropertyFloat : public MaterialPropertyT<float>{
    inline static Type::Pin<MaterialPropertyFloat> pin;
};
class MaterialPropertyFloat2 : public MaterialPropertyT<Vec2> {
    inline static Type::Pin<MaterialPropertyFloat2> pin;
};
class MaterialPropertyFloat3 : public MaterialPropertyT<Vec3> {
    inline static Type::Pin<MaterialPropertyFloat3> pin;
};
class MaterialPropertyFloat4 : public MaterialPropertyT<Vec4> {
    inline static Type::Pin<MaterialPropertyFloat4> pin;
};
class MaterialPropertyFloat2x2 : public MaterialPropertyT<Mat2> {
    inline static Type::Pin<MaterialPropertyFloat2x2> pin;
};
class MaterialPropertyFloat3x3 : public MaterialPropertyT<Mat3> {
    inline static Type::Pin<MaterialPropertyFloat3x3> pin;
};
class MaterialPropertyFloat4x4 : public MaterialPropertyT<Mat4> {
    inline static Type::Pin<MaterialPropertyFloat4x4> pin;
};
class MaterialPropertyColor : public MaterialPropertyT<Vec4> {
    inline static Type::Pin<MaterialPropertyColor> pin;
};

class MaterialPropertyTexture : public MaterialProperty
{
    inline static Type::Pin<MaterialPropertyTexture> pin;
public:
    typedef sptr<Texture> value_type;
    constexpr static ShaderVarType var_type = ShaderVarLangType<sptr<Texture>>::type;

    sptr<Texture> value = {};

    virtual ShaderVarType GetUniformType() const override {
        return ShaderVarLangType<sptr<Texture>>::type;
    }

    virtual void Apply(const sptr<Shader>& shader) override {
        shader->SetUniform(name, value);
    }

    virtual void ToJson(json& obj) const override {
        Object::ToJson(obj);
        ObjectLinker::SaveAsset(obj, "value", value);
    }

    virtual void FromJson(const json& obj, ObjectLinker* linker) override {
        Object::FromJson(obj, linker);
        ObjectLinker::RestoreAsset(linker, This(), value, obj, "value");
    }
};

template<class PTy>
void SetUniformImpl(MaterialPropertyBlock& block, const std::string& name, const typename PTy::value_type& value)
{
    sptr<PTy> prop;

    auto it = block.properties.find(name);
    if (it == block.properties.end() || it->second->GetUniformType() != ShaderVarLangType<typename PTy::value_type>::type)
    {
        prop = spnew<PTy>();
        prop->SetName(name);
        block.properties[name] = prop;
    }
    else
    {
        prop = std::dynamic_pointer_cast<PTy>(it->second);
    }

    assert(prop);
    prop->value = value;
}

void MaterialPropertyBlock::SetUniform(const std::string& name, float value) {
    SetUniformImpl<MaterialPropertyFloat>(*this, name, value);
}

void MaterialPropertyBlock::SetUniform(const std::string& name, const Vec2& value) {
    SetUniformImpl<MaterialPropertyFloat2>(*this, name, value);
}

void MaterialPropertyBlock::SetUniform(const std::string& name, const Vec3& value) {
    SetUniformImpl<MaterialPropertyFloat3>(*this, name, value);
}

void MaterialPropertyBlock::SetUniform(const std::string& name, const Vec4& value) {
    SetUniformImpl<MaterialPropertyFloat4>(*this, name, value);
}

void MaterialPropertyBlock::SetUniform(const std::string& name, const Mat2& value) {
    SetUniformImpl<MaterialPropertyFloat2x2>(*this, name, value);
}

void MaterialPropertyBlock::SetUniform(const std::string& name, const Mat3& value) {
    SetUniformImpl<MaterialPropertyFloat3x3>(*this, name, value);
}

void MaterialPropertyBlock::SetUniform(const std::string& name, const Mat4& value) {
    SetUniformImpl<MaterialPropertyFloat4x4>(*this, name, value);
}

void MaterialPropertyBlock::SetUniform(const std::string& name, const Color& value) {
    SetUniformImpl<MaterialPropertyColor>(*this, name, (Vec4&)value);
}

void MaterialPropertyBlock::SetUniform(const std::string& name, const sptr<Texture>& value) {
    SetUniformImpl<MaterialPropertyTexture>(*this, name, value);
}

sptr<Texture> MaterialPropertyBlock::GetTexture(const std::string& name)
{
    sptr<Texture> ret;

    auto it = properties.find(name);
    if (it != properties.end() && it->second->GetUniformType() == ShaderVarType::Sampler2D)
    {
        auto prop = (MaterialPropertyTexture*)it->second.get();
        ret = prop->value;
    }

    return ret;
}

void MaterialPropertyBlock::ToJson(json& obj) const
{
    Object::ToJson(obj);

    auto props = json::object();

    for (auto& [key, val] : properties)
        val->ToJson(props[key]);

    obj["properties"] = std::move(props);
}

void MaterialPropertyBlock::FromJson(const json& obj, ObjectLinker* linker)
{
    Object::FromJson(obj, linker);

    for (auto& [key, value] : obj["properties"].items())
        properties[key] = Object::CreateFromJson<MaterialProperty>(value, linker);
}

} // gfx
} // mw
