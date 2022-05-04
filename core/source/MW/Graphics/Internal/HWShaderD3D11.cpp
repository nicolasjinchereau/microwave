/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWShaderD3D11;
import Microwave.Graphics.Internal.HWBufferD3D11;
import Microwave.Graphics.Internal.HWContextD3D11;
import Microwave.Graphics.Internal.HWTextureD3D11;
import Microwave.Graphics.ShaderInfo;

namespace mw {
inline namespace gfx {

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

HWShaderD3D11::HWShaderD3D11(
    const sptr<HWContextD3D11>& context,
    const sptr<ShaderInfo>& info)
    : context(context)
    , HWShader(info)
{
    auto vsBytecode = CompileShader(info->vertShaderSource, info->vertShaderEntryPoint, "vs_4_0");
    auto psBytecode = CompileShader(info->fragShaderSource, info->fragShaderEntryPoint, "ps_4_0");

    HRESULT hr;

    hr = context->device->CreateVertexShader(
        vsBytecode->GetBufferPointer(),
        vsBytecode->GetBufferSize(),
        nullptr,
        &vertexShader);

    if(FAILED(hr))
        throw std::runtime_error("could not create vertex shader");

    hr = context->device->CreatePixelShader(
        psBytecode->GetBufferPointer(),
        psBytecode->GetBufferSize(),
        nullptr,
        &pixelShader);

    if(FAILED(hr))
        throw std::runtime_error("could not create pixel shader");

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
    std::list<std::string> semanticNames;
    UINT elementOffset = 0;

    for (uint32_t i = 0; i < info->attributes.size(); ++i)
    {
        auto& attrib = info->attributes[i];
        attrib.slot = i;

        semanticNames.push_back(ShaderInfo::GetSemanticName(attrib.semantic));

        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        auto type = info->attributes[i].type;

        if(type == ShaderVarType::Float)
            format = DXGI_FORMAT_R32_FLOAT;
        else if(type == ShaderVarType::Float2)
            format = DXGI_FORMAT_R32G32_FLOAT;
        else if(type == ShaderVarType::Float3)
            format = DXGI_FORMAT_R32G32B32_FLOAT;
        else if(type == ShaderVarType::Float4)
            format = DXGI_FORMAT_R32G32B32A32_FLOAT;

        D3D11_INPUT_ELEMENT_DESC element { 
            .SemanticName = semanticNames.back().c_str(),
            .SemanticIndex = (UINT)attrib.semanticIndex,
            .Format = format,
            .InputSlot = (uint32_t)attrib.slot,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        };

        elementOffset += info->attributes[i].size;

        inputElements.push_back(element);
    }

    hr = context->device->CreateInputLayout(
        &inputElements[0], inputElements.size(),
        vsBytecode->GetBufferPointer(), vsBytecode->GetBufferSize(),
        &inputLayout);

    if(FAILED(hr))
        throw std::runtime_error("could not create input layout for shader");

    // create a constant buffer for uniforms
    uint32_t bufferSize = 0;
    uint32_t samplerSlot = 0;

    for(uint32_t i = 0; i < info->uniforms.size(); ++i)
    {
        ShaderUniform& uniform = info->uniforms[i];

        info->uniformIDs[uniform.name] = i;

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

    hr = context->device->CreateBuffer(&cbDesc, &resData, &uniformBuffer);
    if(FAILED(hr))
        throw std::runtime_error("failed to create uniform buffer");
}

HWShaderD3D11::~HWShaderD3D11()
{
    vertexShader = nullptr;
    pixelShader = nullptr;
    inputLayout = nullptr;
    uniformBuffer = nullptr;
}

void HWShaderD3D11::Bind()
{
    context->deviceContext->IASetInputLayout(inputLayout.Get());
    context->deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    context->deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
}

void HWShaderD3D11::Unbind()
{
    context->deviceContext->IASetInputLayout(nullptr);
    context->deviceContext->VSSetShader(nullptr, nullptr, 0);
    context->deviceContext->PSSetShader(nullptr, nullptr, 0);
}

void HWShaderD3D11::SetVertexBuffer(int id, const sptr<Buffer>& buffer, size_t offset, size_t stride)
{
    if(id >= 0)
    {
        assert(buffer->GetType() == BufferType::Vertex);
        auto pb = spcast<HWBufferD3D11>(buffer->GetHWBuffer());
        assert(pb);

        auto& attrib = info->attributes[id];
        context->deviceContext->IASetVertexBuffers(
            attrib.slot, 1, pb->GetBuffer().GetAddressOf(), &stride, &offset);
    }
}

void HWShaderD3D11::SetIndexBuffer(const sptr<Buffer>& buffer)
{
    assert(buffer->GetType() == BufferType::Index);
    auto pb = spcast<HWBufferD3D11>(buffer->GetHWBuffer());
    assert(pb);

    context->deviceContext->IASetIndexBuffer(pb->GetBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
}

template<class T>
void SetUniformD3D(
    const sptr<HWContextD3D11>& context,
    std::vector<uint8_t>& uniformData,
    UINT offset,
    ID3D11Buffer* buffer,
    const T& value)
{
    memcpy(uniformData.data() + offset, &value, sizeof(T));

    D3D11_MAPPED_SUBRESOURCE resource;
    auto res = context->deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if(SUCCEEDED(res))
    {
        //assert((resource.RowPitch * resource.DepthPitch) == sizeof(T))
        memcpy(resource.pData, uniformData.data(), uniformData.size());
        context->deviceContext->Unmap(buffer, 0);
    }

    context->deviceContext->VSSetConstantBuffers(0, 1, &buffer);
    context->deviceContext->PSSetConstantBuffers(0, 1, &buffer);
}

void HWShaderD3D11::SetUniform(int id, float value)
{
    if(id >= 0)
        SetUniformD3D(context, uniformData, info->uniforms[id].slot, uniformBuffer.Get(), value);
}

void HWShaderD3D11::SetUniform(int id, const Vec2& value)
{
    if(id >= 0)
        SetUniformD3D(context, uniformData, info->uniforms[id].slot, uniformBuffer.Get(), value);
}

void HWShaderD3D11::SetUniform(int id, const Vec3& value)
{
    if(id >= 0)
        SetUniformD3D(context, uniformData, info->uniforms[id].slot, uniformBuffer.Get(), value);
}

void HWShaderD3D11::SetUniform(int id, const Vec4& value)
{
    if(id >= 0)
        SetUniformD3D(context, uniformData, info->uniforms[id].slot, uniformBuffer.Get(), value);
}

void HWShaderD3D11::SetUniform(int id, const Mat2& value)
{
    if(id >= 0)
        SetUniformD3D(context, uniformData, info->uniforms[id].slot, uniformBuffer.Get(), value);
}

void HWShaderD3D11::SetUniform(int id, const Mat3& value)
{
    if(id >= 0)
        SetUniformD3D(context, uniformData, info->uniforms[id].slot, uniformBuffer.Get(), value);
}

void HWShaderD3D11::SetUniform(int id, const Mat4& value)
{
    if(id >= 0)
        SetUniformD3D(context, uniformData, info->uniforms[id].slot, uniformBuffer.Get(), value);
}

void HWShaderD3D11::SetUniform(int id, const Color& value)
{
    if(id >= 0)
        SetUniformD3D(context, uniformData, info->uniforms[id].slot, uniformBuffer.Get(), value);
}

void HWShaderD3D11::SetUniform(int id, const sptr<Texture>& texture)
{
    if(id >= 0)
    {
        sptr<HWTexture> hwTex = texture->GetHWTexture();
        if (!hwTex)
            return;

        auto pt = spcast<HWTextureD3D11>(hwTex);
        assert(pt);

        auto& uniform = info->uniforms[id];

        uint32_t textureSlot = (uniform.slot >> 16);
        uint32_t samplerSlot = (uniform.slot & 0xFFFF);

        ID3D11ShaderResourceView* resView[1] = {
            pt->resourceView ? pt->resourceView.Get() : nullptr
        };

        ID3D11SamplerState* samplerState[1] = {
            pt->samplerState ? pt->samplerState.Get() : nullptr
        };

        context->deviceContext->VSSetShaderResources(textureSlot, 1, resView);
        context->deviceContext->VSSetSamplers(samplerSlot, 1, samplerState);

        context->deviceContext->PSSetShaderResources(textureSlot, 1, resView);
        context->deviceContext->PSSetSamplers(samplerSlot, 1, samplerState);
    }
}

} // gfx
} // mw
