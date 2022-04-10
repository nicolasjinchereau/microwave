/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.ShaderWindowsD3D11;
import Microwave.Graphics.Internal.DrawBufferWindowsD3D11;
import Microwave.Graphics.Internal.GraphicsContextWindowsD3D11;
import Microwave.Graphics.Internal.TextureWindowsD3D11;
import Microwave.System.Console;
import <cassert>;
import <regex>;
import <algorithm>;
import <MW/System/Internal/PlatformHeaders.h>;

ComPtr<ID3DBlob> CompileShader(const std::string& source, const std::string& entryPoint, const std::string& profile)
{
    ComPtr<ID3DBlob> result;
    ComPtr<ID3DBlob> error;

    UINT compileFlags = 0;
    compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
    //compileFlags |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
    //compileFlags |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;
    
    HRESULT hr = D3DCompile(
        source.c_str(),
        source.length(),
        NULL, NULL, NULL,
        entryPoint.c_str(),
        profile.c_str(),
        compileFlags, 0,
        &result,
        &error);
    
    if (FAILED(hr)) {
        std::string err = (const char*)error->GetBufferPointer();
        throw std::runtime_error(err);
    }

    return result;
}

namespace mw {
inline namespace gfx {

ShaderWindowsD3D11::ShaderWindowsD3D11(
    const sptr<GraphicsContextWindowsD3D11>& graphics,
    const std::string& source)
    : graphics(graphics), Shader(source, ShaderLanguage::HLSL)
{
    AllocateGraphicsResource();
}

ShaderWindowsD3D11::~ShaderWindowsD3D11()
{

}

void ShaderWindowsD3D11::Bind()
{
    graphics->context->IASetInputLayout(inputLayout.Get());
    graphics->context->VSSetShader(vertexShader.Get(), nullptr, 0);
    graphics->context->PSSetShader(pixelShader.Get(), nullptr, 0);
}

void ShaderWindowsD3D11::Unbind()
{
    graphics->context->IASetInputLayout(nullptr);
    graphics->context->VSSetShader(nullptr, nullptr, 0);
    graphics->context->PSSetShader(nullptr, nullptr, 0);
}

void ShaderWindowsD3D11::SetVertexBuffer(const sptr<DrawBuffer>& buffer)
{
    auto pb = std::dynamic_pointer_cast<DrawBufferWindowsD3D11>(buffer);
    UINT stride = info.attribStride;
    UINT offset = 0;
    graphics->context->IASetVertexBuffers(0, 1, pb->buffer.GetAddressOf(), &stride, &offset);
}

void ShaderWindowsD3D11::SetVertexBuffer(int id, sptr<DrawBuffer>& buffer)
{
    if(id >= 0)
    {
        auto pb = std::dynamic_pointer_cast<DrawBufferWindowsD3D11>(buffer);
        auto& attrib = info.attributes[id];
        UINT stride = attrib.size;
        UINT offset = 0;
        graphics->context->IASetVertexBuffers(attrib.slot, 1, pb->buffer.GetAddressOf(), &stride, &offset);
    }
}

void ShaderWindowsD3D11::SetVertexBuffer(const std::string& name, sptr<DrawBuffer>& buffer) {
    SetVertexBuffer(GetAttributeID(name), buffer);
}

void ShaderWindowsD3D11::SetVertexBuffer(InputSemantic semantic, int index, sptr<DrawBuffer>& buffer) {
    SetVertexBuffer(GetAttributeID(semantic, index), buffer);
}

void ShaderWindowsD3D11::SetIndexBuffer(const sptr<DrawBuffer>& buffer)
{
    auto pb = std::dynamic_pointer_cast<DrawBufferWindowsD3D11>(buffer);
    graphics->context->IASetIndexBuffer(pb->buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

template<class T>
void SetUniformD3D(const sptr<GraphicsContextWindowsD3D11>& graphics, std::vector<uint8_t>& uniformData, UINT offset, ID3D11Buffer* buffer, const T& value)
{
    memcpy(uniformData.data() + offset, &value, sizeof(T));

    D3D11_MAPPED_SUBRESOURCE resource;
    auto res = graphics->context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if(SUCCEEDED(res))
    {
        //assert((resource.RowPitch * resource.DepthPitch) == sizeof(T))
        memcpy(resource.pData, uniformData.data(), uniformData.size());
        graphics->context->Unmap(buffer, 0);
    }

    graphics->context->VSSetConstantBuffers(0, 1, &buffer);
    graphics->context->PSSetConstantBuffers(0, 1, &buffer);
}

void ShaderWindowsD3D11::SetUniform(int id, float value)
{
    if(id >= 0) SetUniformD3D(graphics, uniformData, info.uniforms[id].slot, uniformBuffer.Get(), value);
}

void ShaderWindowsD3D11::SetUniform(int id, const Vec2& value) {
    if(id >= 0) SetUniformD3D(graphics, uniformData, info.uniforms[id].slot, uniformBuffer.Get(), value);
}

void ShaderWindowsD3D11::SetUniform(int id, const Vec3& value) {
    if(id >= 0) SetUniformD3D(graphics, uniformData, info.uniforms[id].slot, uniformBuffer.Get(), value);
}

void ShaderWindowsD3D11::SetUniform(int id, const Vec4& value) {
    if(id >= 0) SetUniformD3D(graphics, uniformData, info.uniforms[id].slot, uniformBuffer.Get(), value);
}

void ShaderWindowsD3D11::SetUniform(int id, const Mat2& value) {
    if(id >= 0) SetUniformD3D(graphics, uniformData, info.uniforms[id].slot, uniformBuffer.Get(), value);
}

void ShaderWindowsD3D11::SetUniform(int id, const Mat3& value) {
    if(id >= 0) SetUniformD3D(graphics, uniformData, info.uniforms[id].slot, uniformBuffer.Get(), value);
}

void ShaderWindowsD3D11::SetUniform(int id, const Mat4& value) {
    if(id >= 0) SetUniformD3D(graphics, uniformData, info.uniforms[id].slot, uniformBuffer.Get(), value);
}

void ShaderWindowsD3D11::SetUniform(int id, const Color& value) {
    if(id >= 0) SetUniformD3D(graphics, uniformData, info.uniforms[id].slot, uniformBuffer.Get(), value);
}

void ShaderWindowsD3D11::SetUniform(int id, const sptr<Texture>& texture)
{
    if(id >= 0)
    {
        auto pt = std::dynamic_pointer_cast<TextureWindowsD3D11>(texture);
        auto& uniform = info.uniforms[id];

        uint32_t textureSlot = (uniform.slot >> 16);
        uint32_t samplerSlot = (uniform.slot & 0xFFFF);

        ID3D11ShaderResourceView* resView[1] = { pt->resourceView ? pt->resourceView.Get() : nullptr };
        ID3D11SamplerState* samplerState[1] = { pt->samplerState ? pt->samplerState.Get() : nullptr };

        graphics->context->VSSetShaderResources(textureSlot, 1, resView);
        graphics->context->VSSetSamplers(samplerSlot, 1, samplerState);

        graphics->context->PSSetShaderResources(textureSlot, 1, resView);
        graphics->context->PSSetSamplers(samplerSlot, 1, samplerState);
    }
}

void ShaderWindowsD3D11::SetUniform(const std::string& name, float value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderWindowsD3D11::SetUniform(const std::string& name, const Vec2& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderWindowsD3D11::SetUniform(const std::string& name, const Vec3& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderWindowsD3D11::SetUniform(const std::string& name, const Vec4& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderWindowsD3D11::SetUniform(const std::string& name, const Mat2& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderWindowsD3D11::SetUniform(const std::string& name, const Mat3& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderWindowsD3D11::SetUniform(const std::string& name, const Mat4& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderWindowsD3D11::SetUniform(const std::string& name, const Color& value) {
    SetUniform(GetUniformID(name), value);
}

void ShaderWindowsD3D11::SetUniform(const std::string& name, const sptr<Texture>& texture) {
    SetUniform(GetUniformID(name), texture);
}

void ShaderWindowsD3D11::AllocateGraphicsResource()
{
    auto vsBytecode = CompileShader(info.vertShaderSource, info.vertShaderEntryPoint, "vs_4_0");
    auto psBytecode = CompileShader(info.fragShaderSource, info.fragShaderEntryPoint, "ps_4_0");

    HRESULT hr;
    
    hr = graphics->device->CreateVertexShader(
        vsBytecode->GetBufferPointer(),
        vsBytecode->GetBufferSize(),
        nullptr,
        &vertexShader);

    if(FAILED(hr))
        throw std::runtime_error("could not create vertex shader");

    hr = graphics->device->CreatePixelShader(
        psBytecode->GetBufferPointer(),
        psBytecode->GetBufferSize(),
        nullptr,
        &pixelShader);

    if(FAILED(hr))
        throw std::runtime_error("could not create pixel shader");

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
    std::vector<std::unique_ptr<std::string>> semanticNames;
    UINT elementOffset = 0;

    for (uint32_t i = 0; i < info.attributes.size(); ++i)
    {
        auto& attrib = info.attributes[i];
        
        std::string semanticName = attrib.semantic;
        UINT semanticIndex = 0;

        if(isdigit(semanticName.back())) {
            semanticIndex = semanticName.back() - '0';
            semanticName.pop_back();
        }

        semanticNames.push_back(std::make_unique<std::string>(std::move(semanticName)));

        attrib.slot = i;
        info.attribIDs[attrib.name] = i;

        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        
        auto type = info.attributes[i].type;
        
        if(type == ShaderVarType::Float)
            format = DXGI_FORMAT_R32_FLOAT;
        else if(type == ShaderVarType::Float2)
            format = DXGI_FORMAT_R32G32_FLOAT;
        else if(type == ShaderVarType::Float3)
            format = DXGI_FORMAT_R32G32B32_FLOAT;
        else if(type == ShaderVarType::Float4)
            format = DXGI_FORMAT_R32G32B32A32_FLOAT;

        D3D11_INPUT_ELEMENT_DESC element;

        element.SemanticName = semanticNames.back()->c_str();
        element.SemanticIndex = semanticIndex;
        element.Format = format;
        element.InputSlot = attrib.slot;
        //element.AlignedByteOffset = elementOffset;
        element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        element.InstanceDataStepRate = 0;
        
        inputLayoutDesc.push_back(element);

        elementOffset += info.attributes[i].size;
    }

    hr = graphics->device->CreateInputLayout(
        &inputLayoutDesc[0],
        inputLayoutDesc.size(),
        vsBytecode->GetBufferPointer(),
        vsBytecode->GetBufferSize(),
        &inputLayout);

    if(FAILED(hr))
        throw std::runtime_error("could not create input layout for shader");
    
    // create a constant buffer for uniforms
    uint32_t bufferSize = 0;
    uint32_t samplerSlot = 0;

    for(uint32_t i = 0; i < info.uniforms.size(); ++i)
    {
        ShaderUniform& uniform = info.uniforms[i];

        info.uniformIDs[uniform.name] = i;

        if(uniform.size > 0)
        {
            uniform.slot = bufferSize;
            bufferSize += uniform.size;
            //bufferSize = (bufferSize + 15) & ~15;
        }
        else
        {
            uniform.slot = samplerSlot++;
        }
    }

    bufferSize = (bufferSize + 15) & ~15;
    uniformData.resize(bufferSize);

    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = bufferSize;
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA resData;
    resData.pSysMem = uniformData.data();
    resData.SysMemPitch = 0;
    resData.SysMemSlicePitch = 0;

    hr = graphics->device->CreateBuffer(&cbDesc, &resData, &uniformBuffer);
    if(FAILED(hr))
        throw std::runtime_error("failed to create uniform buffer");
}

void ShaderWindowsD3D11::ReleaseGraphicsResource()
{
    for(auto& att : info.attributes)
        att.slot = -1;

    for(auto& attID : info.attribIDs)
        attID.second = -1;

    for(auto& uni : info.uniforms)
        uni.slot = -1;

    for(auto& uniID : info.uniformIDs)
        uniID.second = -1;

    vertexShader = nullptr;
    pixelShader = nullptr;
    inputLayout = nullptr;
    uniformBuffer = nullptr;
}

bool ShaderWindowsD3D11::IsGraphicsResourceAllocated() const {
    return vertexShader && pixelShader && inputLayout;
}

GraphicsDriverType ShaderWindowsD3D11::GetGraphicsDriverType() const {
    return GraphicsDriverType::Direct3D11;
}

}
}
