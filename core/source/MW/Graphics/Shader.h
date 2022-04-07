/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Color.h>
#include <MW/Graphics/GraphicsResource.h>
#include <MW/Math/Math.h>
#include <MW/System/Object.h>
#include <MW/System/Pointers.h>
#include <unordered_map>
#include <vector>
#include <string>

namespace mw {
inline namespace gfx {

enum class InputSemantic : int
{
    NONE,
    BINORMAL,
    BLENDINDICES,
    BLENDWEIGHT,
    COLOR,
    NORMAL,
    POSITION,
    POSITIONT, // no index
    PSIZE,
    TANGENT,
    TEXCOORD,
};

enum class ShaderVarType : int
{
    Unknown,
    Void,
    Bool, Bool2, Bool3, Bool4,
    Int, Int2, Int3, Int4,
    Uint, Uint2, Uint3, Uint4,
    Half, Half2, Half3, Half4,
    Half2x2, Half3x3, Half4x2, Half4x3, Half4x4,
    Float, Float2, Float3, Float4,
    Float2x2, Float3x3, Float4x2, Float4x3, Float4x4,
    Texture,
    Sampler,
    Sampler2D, Sampler2DShadow, Sampler2DMS, Sampler2DArray,
    Sampler3D, SamplerCube,
    Struct,
};

struct ShaderUniform
{
    std::string name;
    ShaderVarType type;
    int size;
    int arraySize;
    int slot;
};

struct ShaderAttribute
{
    std::string name;
    std::string semantic;
    ShaderVarType type;
    int size;
    int slot;
};

enum class ShaderLanguage
{
    GLSL_100, // OpenGL ES 2.0
    GLSL_300, // OpenGL ES 3.0
    GLSL_150, // OpenGL 3.2
    HLSL
};

struct ShaderInfo
{
    ShaderLanguage language;

    std::string vertShaderSource;
    std::string fragShaderSource;
    std::string vertShaderEntryPoint;
    std::string fragShaderEntryPoint;

    std::vector<ShaderUniform> uniforms;
    std::vector<ShaderAttribute> attributes;
    std::unordered_map<std::string, int> uniformIDs;
    std::unordered_map<std::string, int> attribIDs;
    int attribStride = 0;
};

class Texture;
class DrawBuffer;

class Shader : public GraphicsResource
{
protected:
    ShaderInfo info;

public:
    Shader(const std::string& source, ShaderLanguage driverLanguage);
    virtual ~Shader(){}

    static ShaderInfo ParseShaderSource(const std::string& source, ShaderLanguage translate);

    virtual int GetAttributeID(const std::string& name);
    virtual int GetAttributeID(InputSemantic semantic, int index);
    virtual int GetUniformID(const std::string& name);

    virtual bool HasAttribute(const std::string& name);
    virtual bool HasUniform(const std::string& name);

    virtual int GetAttributeCount() const;
    virtual int GetUniformCount() const;

    virtual ShaderVarType GetAttributeType(const std::string& name);
    virtual ShaderVarType GetAttributeType(int id);

    virtual ShaderVarType GetUniformType(const std::string& name);
    virtual ShaderVarType GetUniformType(int id);
    
    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    virtual void SetVertexBuffer(const sptr<DrawBuffer>& buffer) = 0;
    virtual void SetVertexBuffer(int id, sptr<DrawBuffer>& buffer) = 0;
    virtual void SetVertexBuffer(const std::string& name, sptr<DrawBuffer>& buffer) = 0;
    virtual void SetVertexBuffer(InputSemantic semantic, int index, sptr<DrawBuffer>& buffer) = 0;
    virtual void SetIndexBuffer(const sptr<DrawBuffer>& buffer) = 0;
    
    virtual void SetUniform(int id, float value) = 0;
    virtual void SetUniform(int id, const Vec2& value) = 0;
    virtual void SetUniform(int id, const Vec3& value) = 0;
    virtual void SetUniform(int id, const Vec4& value) = 0;
    virtual void SetUniform(int id, const Mat2& value) = 0;
    virtual void SetUniform(int id, const Mat3& value) = 0;
    virtual void SetUniform(int id, const Mat4& value) = 0;
    virtual void SetUniform(int id, const Color& value) = 0;
    virtual void SetUniform(int id, const sptr<Texture>& texture) = 0;
    virtual void SetUniform(const std::string& name, float value) = 0;
    virtual void SetUniform(const std::string& name, const Vec2& value) = 0;
    virtual void SetUniform(const std::string& name, const Vec3& value) = 0;
    virtual void SetUniform(const std::string& name, const Vec4& value) = 0;
    virtual void SetUniform(const std::string& name, const Mat2& value) = 0;
    virtual void SetUniform(const std::string& name, const Mat3& value) = 0;
    virtual void SetUniform(const std::string& name, const Mat4& value) = 0;
    virtual void SetUniform(const std::string& name, const Color& value) = 0;
    virtual void SetUniform(const std::string& name, const sptr<Texture>& texture) = 0;
};

inline void to_json(json& obj, const ShaderVarType& type)
{
    static std::unordered_map<ShaderVarType, std::string> names {
        { ShaderVarType::Unknown, "Unknown" },
        { ShaderVarType::Void, "Void" },
        
        { ShaderVarType::Bool, "Bool" },
        { ShaderVarType::Bool2, "Bool2" },
        { ShaderVarType::Bool3, "Bool3" },
        { ShaderVarType::Bool4, "Bool4" },
        
        { ShaderVarType::Int, "Int" },
        { ShaderVarType::Int2, "Int2" },
        { ShaderVarType::Int3, "Int3" },
        { ShaderVarType::Int4, "Int4" },

        { ShaderVarType::Uint, "Uint" },
        { ShaderVarType::Uint2, "Uint2" },
        { ShaderVarType::Uint3, "Uint3" },
        { ShaderVarType::Uint4, "Uint4" },

        { ShaderVarType::Half, "Half" },
        { ShaderVarType::Half2, "Half2" },
        { ShaderVarType::Half3, "Half3" },
        { ShaderVarType::Half4, "Half4" },

        { ShaderVarType::Half2x2, "Half2x2" },
        { ShaderVarType::Half3x3, "Half3x3" },
        { ShaderVarType::Half4x2, "Half4x2" },
        { ShaderVarType::Half4x3, "Half4x3" },
        { ShaderVarType::Half4x4, "Half4x4" },

        { ShaderVarType::Float, "Float" },
        { ShaderVarType::Float2, "Float2" },
        { ShaderVarType::Float3, "Float3" },
        { ShaderVarType::Float4, "Float4" },

        { ShaderVarType::Float2x2, "Float2x2" },
        { ShaderVarType::Float3x3, "Float3x3" },
        { ShaderVarType::Float4x2, "Float4x2" },
        { ShaderVarType::Float4x3, "Float4x3" },
        { ShaderVarType::Float4x4, "Float4x4" },

        { ShaderVarType::Texture, "Texture" },
        { ShaderVarType::Sampler, "Sampler" },

        { ShaderVarType::Sampler2D, "Sampler2D" },
        { ShaderVarType::Sampler2DShadow, "Sampler2DShadow" },
        { ShaderVarType::Sampler2DMS, "Sampler2DMS" },
        { ShaderVarType::Sampler2DArray, "Sampler2DArray" },

        { ShaderVarType::Sampler3D, "Sampler3D" },
        { ShaderVarType::SamplerCube, "SamplerCube" },

        { ShaderVarType::Struct, "Struct" },
    };

    obj = names[type];
}

inline void from_json(const json& obj, ShaderVarType& type)
{
    static std::unordered_map<std::string, ShaderVarType> types {
        { "Unknown", ShaderVarType::Unknown },
        { "Void", ShaderVarType::Void },

        { "Bool", ShaderVarType::Bool },
        { "Bool2", ShaderVarType::Bool2 },
        { "Bool3", ShaderVarType::Bool3 },
        { "Bool4", ShaderVarType::Bool4 },

        { "Int", ShaderVarType::Int },
        { "Int2", ShaderVarType::Int2 },
        { "Int3", ShaderVarType::Int3 },
        { "Int4", ShaderVarType::Int4 },

        { "Uint", ShaderVarType::Uint },
        { "Uint2", ShaderVarType::Uint2 },
        { "Uint3", ShaderVarType::Uint3 },
        { "Uint4", ShaderVarType::Uint4 },

        { "Half", ShaderVarType::Half },
        { "Half2", ShaderVarType::Half2 },
        { "Half3", ShaderVarType::Half3 },
        { "Half4", ShaderVarType::Half4 },

        { "Half2x2", ShaderVarType::Half2x2 },
        { "Half3x3", ShaderVarType::Half3x3 },
        { "Half4x2", ShaderVarType::Half4x2 },
        { "Half4x3", ShaderVarType::Half4x3 },
        { "Half4x4", ShaderVarType::Half4x4 },

        { "Float", ShaderVarType::Float },
        { "Float2", ShaderVarType::Float2 },
        { "Float3", ShaderVarType::Float3 },
        { "Float4", ShaderVarType::Float4 },

        { "Float2x2", ShaderVarType::Float2x2 },
        { "Float3x3", ShaderVarType::Float3x3 },
        { "Float4x2", ShaderVarType::Float4x2 },
        { "Float4x3", ShaderVarType::Float4x3 },
        { "Float4x4", ShaderVarType::Float4x4 },

        { "Texture", ShaderVarType::Texture },
        { "Sampler", ShaderVarType::Sampler },

        { "Sampler2D", ShaderVarType::Sampler2D },
        { "Sampler2DShadow", ShaderVarType::Sampler2DShadow },
        { "Sampler2DMS", ShaderVarType::Sampler2DMS },
        { "Sampler2DArray", ShaderVarType::Sampler2DArray },

        { "Sampler3D", ShaderVarType::Sampler3D },
        { "SamplerCube", ShaderVarType::SamplerCube },

        { "Struct", ShaderVarType::Struct },
    };

    auto val = obj.get<std::string>();
    type = types[val];
}

}
}
