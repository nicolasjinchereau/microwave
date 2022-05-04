/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Shader;
import Microwave.Graphics.Internal.HWShader;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.ShaderInfo;
import Microwave.System.Console;
import Microwave.Utilities.Format;
import <cassert>;
import <regex>;
import <algorithm>;
import <unordered_map>;

namespace mw {
inline namespace gfx {

Shader::Shader(const std::string& source)
{
    auto graphics = GraphicsContext::GetCurrent();
    if (!graphics)
        throw std::runtime_error("no active graphics context");

    auto info = spnew<ShaderInfo>(source, graphics->context->GetShaderLanguage());
    shader = graphics->context->CreateShader(info);
}

Shader::Shader(const sptr<ShaderInfo>& info)
{
    auto graphics = GraphicsContext::GetCurrent();
    if (!graphics)
        throw std::runtime_error("no active graphics context");

    shader = graphics->context->CreateShader(info);
}

int Shader::GetAttributeID(const std::string& name)
{
    auto it = shader->info->attribIDs.find(name);
    if(it == shader->info->attribIDs.end()) {
        Console::WriteLine("Warning: Couldn't find ID for attribute %", name);
        return -1;
    }

    return it->second;
}

int Shader::GetAttributeID(Semantic semantic, int index)
{
    for(size_t i = 0; i != shader->info->attributes.size(); ++i)
    {
        auto& attrib = shader->info->attributes[i];

        if (attrib.semantic == semantic &&
            attrib.semanticIndex == index)
        {
            return i;
        }
    }

    return -1;
}

int Shader::GetUniformID(const std::string& name)
{
    auto it = shader->info->uniformIDs.find(name);
    if(it == shader->info->uniformIDs.end())
        return -1;

    return it->second;
}

bool Shader::HasAttribute(const std::string& name) {
    return shader->info->attribIDs.count(name) != 0;
}

bool Shader::HasUniform(const std::string& name) {
    return shader->info->uniformIDs.count(name) != 0;
}

int Shader::GetAttributeCount() const {
    return (int)shader->info->attributes.size();
}

int Shader::GetUniformCount() const {
    return (int)shader->info->attributes.size();
}

ShaderVarType Shader::GetAttributeType(const std::string& name)
{
    int id = GetAttributeID(name);
    if(id == -1)
        return ShaderVarType::Void;

    return shader->info->attributes[id].type;
}

ShaderVarType Shader::GetAttributeType(int id) {
    return shader->info->attributes[id].type;
}

ShaderVarType Shader::GetUniformType(const std::string& name) {
    int id = GetUniformID(name);
    if(id == -1)
        return ShaderVarType::Void;

    return shader->info->uniforms[id].type;
}

ShaderVarType Shader::GetUniformType(int id) {
    return shader->info->uniforms[id].type;
}

void Shader::Bind() {
    shader->Bind();
}

void Shader::Unbind() {
    shader->Unbind();
}

void Shader::SetVertexBuffer(const VertexMapping& vm)
{
    for(auto& vs : vm.slots) {
        SetVertexBuffer(vs.semantic, vs.semanticIndex, vs.buffer, vs.offset, vs.stride);
    }
}

void Shader::SetVertexBuffer(Semantic semantic, int semanticIndex, const sptr<Buffer>& buffer, size_t offset, size_t stride) {
    shader->SetVertexBuffer(GetAttributeID(semantic, semanticIndex), buffer, offset, stride);
}

void Shader::SetVertexBuffer(const std::string& name, const sptr<Buffer>& buffer, size_t offset, size_t stride) {
    shader->SetVertexBuffer(GetAttributeID(name), buffer, offset, stride);
}

void Shader::SetIndexBuffer(const sptr<Buffer>& buffer) {
    shader->SetIndexBuffer(buffer);
}

void Shader::SetUniform(const std::string& name, float value) {
    shader->SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const std::string& name, const Vec2& value) {
    shader->SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const std::string& name, const Vec3& value) {
    shader->SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const std::string& name, const Vec4& value) {
    shader->SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const std::string& name, const Mat2& value) {
    shader->SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const std::string& name, const Mat3& value) {
    shader->SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const std::string& name, const Mat4& value) {
    shader->SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const std::string& name, const Color& value) {
    shader->SetUniform(GetUniformID(name), value);
}

void Shader::SetUniform(const std::string& name, const sptr<Texture>& texture) {
    shader->SetUniform(GetUniformID(name), texture);
}

} // gfx
} // mw
