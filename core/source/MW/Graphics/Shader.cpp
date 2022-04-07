/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Graphics/Shader.h>
#include <MW/System/Console.h>
#include <HLSLParser.h>
#include <GLSLGenerator.h>
#include <HLSLGenerator.h>
#include <MSLGenerator.h>
#include <cassert>
#include <regex>
#include <algorithm>
#include <unordered_map>
#include <MW/Utilities/Format.h>

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

std::unordered_map<InputSemantic, std::string> inputSemanticNames =
{
    { InputSemantic::BINORMAL, "BINORMAL" },
    { InputSemantic::BLENDINDICES, "BLENDINDICES" },
    { InputSemantic::BLENDWEIGHT, "BLENDWEIGHT" },
    { InputSemantic::COLOR, "COLOR" },
    { InputSemantic::NORMAL, "NORMAL" },
    { InputSemantic::POSITION, "POSITION" },
    { InputSemantic::POSITIONT, "POSITIONT" },
    { InputSemantic::PSIZE, "PSIZE" },
    { InputSemantic::TANGENT, "TANGENT" },
    { InputSemantic::TEXCOORD, "TEXCOORD" }
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

Shader::Shader(const std::string& source, ShaderLanguage driverLanguage)
{
    info = ParseShaderSource(source, driverLanguage);
}

ShaderInfo Shader::ParseShaderSource(const std::string& source, ShaderLanguage translate)
{
    ShaderInfo result;
    
    result.language = translate;

    Allocator allocator;
    HLSLParser parser(&allocator, "filename", source.data(), source.size());
    HLSLTree tree(&allocator);

    ParserError error;
    if(!parser.Parse(&tree, &error))
        throw std::runtime_error(format("(%): %", error.line, error.msg));

    // find entry points
    std::regex vs_reg("#pragma[\\s\\n]+vertex[\\s\\n]+(\\w+)");
    std::regex fs_reg("#pragma[\\s\\n]+fragment[\\s\\n]+(\\w+)");

    std::smatch vs_match;
    std::smatch fs_match;

    if(!regex_search(source.cbegin(), source.cend(), vs_match, vs_reg))
        throw std::runtime_error("error: entry point not found for vertex shader.");

    if(!regex_search(source.cbegin(), source.cend(), fs_match, fs_reg))
        throw std::runtime_error("error: entry point not found for fragment shader.");

    result.vertShaderEntryPoint = vs_match[1];
    result.fragShaderEntryPoint = fs_match[1];

    // get attribute information
    HLSLFunction* vsEntryFunc = tree.FindFunction(result.vertShaderEntryPoint.c_str());
    
    result.attribStride = 0;

    for(auto arg = vsEntryFunc->argument; arg; arg = arg->nextArgument)
    {
        if(arg->type.baseType == HLSLBaseType_UserDefined)
        {
            HLSLStruct* userStruct = tree.FindGlobalStruct(arg->type.typeName);

            for(auto field = userStruct->field; field; field = field->nextField)
            {
                ShaderAttribute attrib;
                attrib.name = field->name;
                attrib.semantic = field->semantic ? field->semantic : field->sv_semantic;
                attrib.type = ToShaderVarType(field->type.baseType);
                attrib.size = GetTypeSize(field->type.baseType);
                attrib.slot = -1;

                result.attribStride += attrib.size;
                result.attributes.push_back(std::move(attrib));
            }
        }
        else
        {
            ShaderAttribute attrib;
            attrib.name = arg->name;
            attrib.semantic = arg->semantic ? arg->semantic : arg->sv_semantic;
            attrib.type = ToShaderVarType(arg->type.baseType);
            attrib.size = GetTypeSize(arg->type.baseType);
            attrib.slot = -1;

            result.attribStride += attrib.size;
            result.attributes.push_back(std::move(attrib));
        }
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

        result.uniforms.push_back(std::move(uni));
    }
    
    std::unordered_map<ShaderLanguage, GLSLGenerator::Version> glslVersions {
        { ShaderLanguage::GLSL_100, GLSLGenerator::Version::Version_100_ES },
        { ShaderLanguage::GLSL_150, GLSLGenerator::Version::Version_150 },
        { ShaderLanguage::GLSL_300, GLSLGenerator::Version::Version_300_ES }
    };

    if (glslVersions.count(translate) != 0)
    {
        GLSLGenerator::Version version = glslVersions[translate];
        
        GLSLGenerator vsGenerator;
        if (!vsGenerator.Generate(&tree, GLSLGenerator::Target_VertexShader, version, result.vertShaderEntryPoint.c_str()))
            throw std::runtime_error("failed to generate vertex shader");

        GLSLGenerator fsGenerator;
        if (!fsGenerator.Generate(&tree, GLSLGenerator::Target_FragmentShader, version, result.fragShaderEntryPoint.c_str()))
            throw std::runtime_error("failed to generate fragment shader");

        result.vertShaderSource = vsGenerator.GetResult();
        result.fragShaderSource = fsGenerator.GetResult();
    }
    else if (translate == ShaderLanguage::HLSL)
    {
        HLSLGenerator vsGenerator;
        if (!vsGenerator.Generate(&tree, HLSLGenerator::Target_VertexShader, result.vertShaderEntryPoint.c_str(), false))
            throw std::runtime_error("failed to generate vertex shader");

        HLSLGenerator fsGenerator;
        if (!fsGenerator.Generate(&tree, HLSLGenerator::Target_PixelShader, result.fragShaderEntryPoint.c_str(), false))
            throw std::runtime_error("failed to generate fragment shader");

        result.vertShaderSource = vsGenerator.GetResult();
        result.fragShaderSource = fsGenerator.GetResult();
    }

    return result;
}

int Shader::GetAttributeID(const std::string& name)
{
    auto it = info.attribIDs.find(name);
    if(it == info.attribIDs.end()) {
        Console::WriteLine("Warning: Couldn't find ID for attribute %", name);
        return -1;
    }

    return it->second;
}

int Shader::GetAttributeID(InputSemantic semantic, int index)
{
    auto& semName = inputSemanticNames[semantic];
    
    for(size_t i = 0; i != info.attributes.size(); ++i)
    {
        auto& sem = info.attributes[i].semantic;
        if (sem.find(semName) == 0)
        {
            char c = sem.back();
            int semIdx = isdigit(c) ? (int)(c - '0') : 0;

            if (index == semIdx)
            {
                return (int)i;
            }
        }
    }

    Console::WriteLine("Warning: Couldn't find ID for attribute %[%]", semName, index);
    return -1;
}

int Shader::GetUniformID(const std::string& name)
{
    auto it = info.uniformIDs.find(name);
    if(it == info.uniformIDs.end())
        return -1;

    return it->second;
}

bool Shader::HasAttribute(const std::string& name) {
    return info.attribIDs.count(name) != 0;
}

bool Shader::HasUniform(const std::string& name) {
    return info.uniformIDs.count(name) != 0;
}

int Shader::GetAttributeCount() const {
    return (int)info.attributes.size();
}

int Shader::GetUniformCount() const {
    return (int)info.attributes.size();
}

ShaderVarType Shader::GetAttributeType(const std::string& name)
{
    int id = GetAttributeID(name);
    if(id == -1)
        return ShaderVarType::Void;

    return info.attributes[id].type;
}

ShaderVarType Shader::GetAttributeType(int id) {
    return info.attributes[id].type;
}

ShaderVarType Shader::GetUniformType(const std::string& name) {
    int id = GetUniformID(name);
    if(id == -1)
        return ShaderVarType::Void;

    return info.uniforms[id].type;
}

ShaderVarType Shader::GetUniformType(int id) {
    return info.uniforms[id].type;
}

}
}
