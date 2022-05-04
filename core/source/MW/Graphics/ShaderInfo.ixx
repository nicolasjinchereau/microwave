/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.ShaderInfo;
import Microwave.Graphics.Color;
import Microwave.Math;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <initializer_list>;
import <string>;
import <unordered_map>;
import <vector>;

export namespace mw {
inline namespace gfx {

enum class Semantic : int
{
    NONE,
    BINORMAL,
    BLENDINDICES,
    BLENDWEIGHT,
    COLOR,
    NORMAL,
    POSITION,
    POSITIONT,
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
    std::string identifier;
    Semantic semantic;
    int semanticIndex;
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
    ShaderLanguage language{};

    std::string vertShaderSource;
    std::string fragShaderSource;
    std::string vertShaderEntryPoint;
    std::string fragShaderEntryPoint;

    std::vector<ShaderUniform> uniforms;
    std::vector<ShaderAttribute> attributes;
    std::unordered_map<std::string, int> uniformIDs;
    std::unordered_map<std::string, int> attribIDs;
    int attribStride = 0;

    ShaderInfo(const std::string& source, ShaderLanguage lang);
    
    std::pair<Semantic, int> ParseSemantic(const char* sem);
    static Semantic GetSemanticValue(const std::string& name);
    static std::string GetSemanticName(Semantic semantic);
};

void to_json(json& obj, const ShaderVarType& type);
void from_json(const json& obj, ShaderVarType& type);

} // gfx
} // mw
