/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.ShaderInfo;
import Microwave.System.Exception;
import <HLSLParser.h>;
import <GLSLGenerator.h>;
import <HLSLGenerator.h>;
import <MSLGenerator.h>;
import <regex>;

using namespace M4;

namespace mw {
inline namespace gfx {

std::unordered_map<HLSLBaseType, int> typeSizes =
{
    { HLSLBaseType_Unknown, 0 },
    { HLSLBaseType_Void, 0 },
    { HLSLBaseType_Bool, 4 },
    { HLSLBaseType_Bool2, 8 },
    { HLSLBaseType_Bool3, 12 },
    { HLSLBaseType_Bool4, 16 },
    { HLSLBaseType_Int, 4 },
    { HLSLBaseType_Int2, 8 },
    { HLSLBaseType_Int3, 12 },
    { HLSLBaseType_Int4, 16 },
    { HLSLBaseType_Uint, 4 },
    { HLSLBaseType_Uint2, 8 },
    { HLSLBaseType_Uint3, 12 },
    { HLSLBaseType_Uint4, 16 },
    { HLSLBaseType_Half, 4 },
    { HLSLBaseType_Half2, 8 },
    { HLSLBaseType_Half3, 12 },
    { HLSLBaseType_Half4, 16 },
    { HLSLBaseType_Half2x2, 16 },
    { HLSLBaseType_Half3x3, 36 },
    { HLSLBaseType_Half4x4, 64 },
    { HLSLBaseType_Half4x3, 48 },
    { HLSLBaseType_Half4x2, 32 },
    { HLSLBaseType_Float, 4 },
    { HLSLBaseType_Float2, 8 },
    { HLSLBaseType_Float3, 12 },
    { HLSLBaseType_Float4, 16 },
    { HLSLBaseType_Float2x2, 16 },
    { HLSLBaseType_Float3x3, 36 },
    { HLSLBaseType_Float4x4, 64 },
    { HLSLBaseType_Float4x3, 48 },
    { HLSLBaseType_Float4x2, 32 },
};

std::unordered_map<HLSLBaseType, ShaderVarType> typeMapping =
{
    { HLSLBaseType_Unknown, ShaderVarType::Unknown },
    { HLSLBaseType_Void, ShaderVarType::Void },
    { HLSLBaseType_Bool, ShaderVarType::Bool },
    { HLSLBaseType_Bool2, ShaderVarType::Bool2 },
    { HLSLBaseType_Bool3, ShaderVarType::Bool3 },
    { HLSLBaseType_Bool4, ShaderVarType::Bool4 },
    { HLSLBaseType_Int, ShaderVarType::Int },
    { HLSLBaseType_Int2, ShaderVarType::Int2 },
    { HLSLBaseType_Int3, ShaderVarType::Int3 },
    { HLSLBaseType_Int4, ShaderVarType::Int4 },
    { HLSLBaseType_Uint, ShaderVarType::Uint },
    { HLSLBaseType_Uint2, ShaderVarType::Uint2 },
    { HLSLBaseType_Uint3, ShaderVarType::Uint3 },
    { HLSLBaseType_Uint4, ShaderVarType::Uint4 },
    { HLSLBaseType_Half, ShaderVarType::Half },
    { HLSLBaseType_Half2, ShaderVarType::Half2 },
    { HLSLBaseType_Half3, ShaderVarType::Half3 },
    { HLSLBaseType_Half4, ShaderVarType::Half4 },
    { HLSLBaseType_Half2x2, ShaderVarType::Half2x2 },
    { HLSLBaseType_Half3x3, ShaderVarType::Half3x3 },
    { HLSLBaseType_Half4x2, ShaderVarType::Half4x2 },
    { HLSLBaseType_Half4x3, ShaderVarType::Half4x3 },
    { HLSLBaseType_Half4x4, ShaderVarType::Half4x4 },
    { HLSLBaseType_Float, ShaderVarType::Float },
    { HLSLBaseType_Float2, ShaderVarType::Float2 },
    { HLSLBaseType_Float3, ShaderVarType::Float3 },
    { HLSLBaseType_Float4, ShaderVarType::Float4 },
    { HLSLBaseType_Float2x2, ShaderVarType::Float2x2 },
    { HLSLBaseType_Float3x3, ShaderVarType::Float3x3 },
    { HLSLBaseType_Float4x2, ShaderVarType::Float4x2 },
    { HLSLBaseType_Float4x3, ShaderVarType::Float4x3 },
    { HLSLBaseType_Float4x4, ShaderVarType::Float4x4 },
    { HLSLBaseType_Texture, ShaderVarType::Texture },
    { HLSLBaseType_Sampler, ShaderVarType::Sampler },
    { HLSLBaseType_Sampler2D, ShaderVarType::Sampler2D },
    { HLSLBaseType_Sampler3D, ShaderVarType::Sampler3D },
    { HLSLBaseType_SamplerCube, ShaderVarType::SamplerCube },
    { HLSLBaseType_Sampler2DShadow, ShaderVarType::Sampler2DShadow },
    { HLSLBaseType_Sampler2DMS, ShaderVarType::Sampler2DMS },
    { HLSLBaseType_Sampler2DArray, ShaderVarType::Sampler2DArray },
    { HLSLBaseType_UserDefined, ShaderVarType::Struct },
};

std::unordered_map<Semantic, std::string> semanticNames =
{
    { Semantic::BINORMAL, "BINORMAL" },
    { Semantic::BLENDINDICES, "BLENDINDICES" },
    { Semantic::BLENDWEIGHT, "BLENDWEIGHT" },
    { Semantic::COLOR, "COLOR" },
    { Semantic::NORMAL, "NORMAL" },
    { Semantic::POSITION, "POSITION" },
    { Semantic::POSITIONT, "POSITIONT" },
    { Semantic::PSIZE, "PSIZE" },
    { Semantic::TANGENT, "TANGENT" },
    { Semantic::TEXCOORD, "TEXCOORD" }
};

std::unordered_map<std::string, Semantic> semanticValues =
{
    { "BINORMAL", Semantic::BINORMAL },
    { "BLENDINDICES", Semantic::BLENDINDICES },
    { "BLENDWEIGHT", Semantic::BLENDWEIGHT },
    { "COLOR", Semantic::COLOR },
    { "NORMAL", Semantic::NORMAL },
    { "POSITION", Semantic::POSITION },
    { "POSITIONT", Semantic::POSITIONT },
    { "PSIZE", Semantic::PSIZE },
    { "TANGENT", Semantic::TANGENT },
    { "TEXCOORD", Semantic::TEXCOORD }
};

ShaderVarType ToShaderVarType(HLSLBaseType ty)
{
    auto it = typeMapping.find(ty);
    return it != typeMapping.end() ? it->second : ShaderVarType::Unknown;
}

int GetTypeSize(HLSLBaseType ty)
{
    auto it = typeSizes.find(ty);
    return it != typeSizes.end() ? it->second : 0;
}

ShaderInfo::ShaderInfo(const std::string& source, ShaderLanguage lang)
{
    this->language = lang;

    Allocator allocator;
    HLSLParser parser(&allocator, "<source code>", source.data(), source.size());
    HLSLTree tree(&allocator);

    ParserError error;
    if(!parser.Parse(&tree, &error))
        throw Exception({ "(", error.line, "): ", error.msg });

    // find entry points
    std::regex vs_reg("#pragma[\\s\\n]+vertex[\\s\\n]+(\\w+)");
    std::regex fs_reg("#pragma[\\s\\n]+fragment[\\s\\n]+(\\w+)");

    std::smatch vs_match;
    std::smatch fs_match;

    if(!regex_search(source.cbegin(), source.cend(), vs_match, vs_reg))
        throw Exception("error: entry point not found for vertex shader.");

    if(!regex_search(source.cbegin(), source.cend(), fs_match, fs_reg))
        throw Exception("error: entry point not found for fragment shader.");

    this->vertShaderEntryPoint = vs_match[1];
    this->fragShaderEntryPoint = fs_match[1];

    // get attribute information
    HLSLFunction* vsEntryFunc = tree.FindFunction(this->vertShaderEntryPoint.c_str());

    this->attribStride = 0;

    for(auto arg = vsEntryFunc->argument; arg; arg = arg->nextArgument)
    {
        if(arg->type.baseType == HLSLBaseType_UserDefined)
        {
            HLSLStruct* userStruct = tree.FindGlobalStruct(arg->type.typeName);

            for(auto field = userStruct->field; field; field = field->nextField)
            {
                auto semantic = field->semantic ? field->semantic : field->sv_semantic;
                auto identifier = (lang == ShaderLanguage::HLSL) ? field->name : semantic;
                auto [sem, idx] = ParseSemantic(semantic);

                ShaderAttribute attrib;
                attrib.name = field->name;
                attrib.identifier = identifier;
                attrib.semantic = sem;
                attrib.semanticIndex = idx;
                attrib.type = ToShaderVarType(field->type.baseType);
                attrib.size = GetTypeSize(field->type.baseType);
                attrib.slot = -1;

                this->attribStride += attrib.size;
                this->attributes.push_back(std::move(attrib));
            }
        }
        else
        {
            auto semantic = arg->semantic ? arg->semantic : arg->sv_semantic;
            auto identifier = (lang == ShaderLanguage::HLSL) ? arg->name : semantic;
            auto [sem, idx] = ParseSemantic(semantic);

            ShaderAttribute attrib;
            attrib.name = arg->name;
            attrib.identifier = identifier;
            attrib.semantic = sem;
            attrib.semanticIndex = idx;
            attrib.type = ToShaderVarType(arg->type.baseType);
            attrib.size = GetTypeSize(arg->type.baseType);
            attrib.slot = -1;

            this->attribStride += attrib.size;
            this->attributes.push_back(std::move(attrib));
        }
    }

    for (size_t i = 0; i < this->attributes.size(); ++i) {
        this->attribIDs[ this->attributes[i].name ] = i;
    }

    // get uniform information
    for(int i = 0; i < parser.m_variables.GetSize(); ++i)
    {
        auto var = parser.m_variables[i];

        ShaderUniform uni;
        uni.name = var.name;
        uni.type = ToShaderVarType(var.type.baseType);
        uni.size = GetTypeSize(var.type.baseType);
        uni.slot = -1;

        if(var.type.array)
            tree.GetExpressionValue(var.type.arraySize, uni.arraySize);
        else
            uni.arraySize = 0;

        this->uniforms.push_back(std::move(uni));
    }

    std::unordered_map<ShaderLanguage, GLSLGenerator::Version> glslVersions {
        { ShaderLanguage::GLSL_100, GLSLGenerator::Version::Version_100_ES },
        { ShaderLanguage::GLSL_150, GLSLGenerator::Version::Version_150 },
        { ShaderLanguage::GLSL_300, GLSLGenerator::Version::Version_300_ES }
    };

    if (glslVersions.count(lang) != 0)
    {
        GLSLGenerator::Version version = glslVersions[lang];

        GLSLGenerator vsGenerator;
        if (!vsGenerator.Generate(&tree, GLSLGenerator::Target_VertexShader, version, this->vertShaderEntryPoint.c_str()))
            throw Exception("failed to generate vertex shader");

        GLSLGenerator fsGenerator;
        if (!fsGenerator.Generate(&tree, GLSLGenerator::Target_FragmentShader, version, this->fragShaderEntryPoint.c_str()))
            throw Exception("failed to generate fragment shader");

        this->vertShaderSource = vsGenerator.GetResult();
        this->fragShaderSource = fsGenerator.GetResult();
    }
    else if (lang == ShaderLanguage::HLSL)
    {
        HLSLGenerator vsGenerator;
        if (!vsGenerator.Generate(&tree, HLSLGenerator::Target_VertexShader, this->vertShaderEntryPoint.c_str(), false))
            throw Exception("failed to generate vertex shader");

        HLSLGenerator fsGenerator;
        if (!fsGenerator.Generate(&tree, HLSLGenerator::Target_PixelShader, this->fragShaderEntryPoint.c_str(), false))
            throw Exception("failed to generate fragment shader");

        this->vertShaderSource = vsGenerator.GetResult();
        this->fragShaderSource = fsGenerator.GetResult();
    }
}

Semantic ShaderInfo::GetSemanticValue(const std::string& semantic) {
    return semanticValues[semantic];
}

std::string ShaderInfo::GetSemanticName(Semantic semantic) {
    return semanticNames[semantic];
}

std::pair<Semantic, int> ShaderInfo::ParseSemantic(const char* sem)
{
    std::string semanticName = sem;
    int semanticIndex = 0;

    if(isdigit(semanticName.back())) {
        semanticIndex = semanticName.back() - '0';
        semanticName.pop_back();
    }

    return { semanticValues[semanticName], semanticIndex };
}

void to_json(json& obj, const ShaderVarType& type)
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

void from_json(const json& obj, ShaderVarType& type)
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

} // gfx
} // mw
