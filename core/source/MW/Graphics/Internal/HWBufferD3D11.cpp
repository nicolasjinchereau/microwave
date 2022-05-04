/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWBufferD3D11;
import Microwave.Graphics.Internal.HWContext;
import Microwave.Graphics.Internal.HWContextD3D11;
import <cassert>;
import <stdexcept>;

namespace mw {
inline namespace gfx {

inline namespace d3d11 {

std::unordered_map<BufferType, D3D11_BIND_FLAG> bufferTypes = {
	{ BufferType::Vertex, D3D11_BIND_VERTEX_BUFFER },
	{ BufferType::Index, D3D11_BIND_INDEX_BUFFER },
	{ BufferType::PixelPack, D3D11_BIND_FLAG{} },
	{ BufferType::PixelUnpack, D3D11_BIND_FLAG{} },
	{ BufferType::Texture, D3D11_BIND_FLAG{} },
	{ BufferType::Uniform, D3D11_BIND_FLAG{} }
};

std::unordered_map<BufferCPUAccess, UINT> bufferCPUAccess =
{
	{ BufferCPUAccess::None, 0 },
	{ BufferCPUAccess::ReadOnly, D3D11_CPU_ACCESS_READ },
	{ BufferCPUAccess::WriteOnly, D3D11_CPU_ACCESS_WRITE },
	{ BufferCPUAccess::ReadWrite, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE }
};

std::unordered_map<BufferMapAccess, D3D11_MAP> bufferMapAccess = {
	{ BufferMapAccess::ReadOnly, D3D11_MAP_READ },
	{ BufferMapAccess::WriteOnly, D3D11_MAP_WRITE },
	{ BufferMapAccess::ReadWrite, D3D11_MAP_READ_WRITE },
	{ BufferMapAccess::WriteDiscard, D3D11_MAP_WRITE_DISCARD },
	{ BufferMapAccess::WriteNoSync, D3D11_MAP_WRITE_DISCARD }
};

std::unordered_map<BufferUsage, D3D11_USAGE> bufferUsage = {
	{ BufferUsage::Default, D3D11_USAGE_DEFAULT },
	{ BufferUsage::Dynamic, D3D11_USAGE_DYNAMIC },
	{ BufferUsage::Static, D3D11_USAGE_IMMUTABLE },
	{ BufferUsage::Stream, D3D11_USAGE_STAGING }
};

} // d3d11

HWBufferD3D11::HWBufferD3D11(
	const sptr<HWContext>& context,
	BufferType type, BufferUsage usage,
	BufferCPUAccess cpuAccess, size_t size)
	: context(spcast<HWContextD3D11>(context))
	, type(type)
	, size(size)
{
	assert(this->context);

	if (type == BufferType::Vertex || type == BufferType::Index)
	{
		auto typ = bufferTypes[type];
		auto usg = bufferUsage[usage];
		auto acc = bufferCPUAccess[cpuAccess];

		std::vector<std::byte> data;
		data.resize(size);

		D3D11_SUBRESOURCE_DATA bufferData;
		bufferData.pSysMem = data.data();
		bufferData.SysMemPitch = 0;
		bufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC bufferDesc(size, typ, usg, acc);

		HRESULT res = this->context->device->CreateBuffer(&bufferDesc, &bufferData, &buffer);
		if(FAILED(res))
			throw std::runtime_error("could not create buffer");
	}
	else
	{
		pixelBuffer.resize(size);
	}
}

HWBufferD3D11::HWBufferD3D11(
	const sptr<HWContext>& context,
	BufferType type, BufferUsage usage,
	BufferCPUAccess cpuAccess,
	const std::span<std::byte>& data)
	: context(spcast<HWContextD3D11>(context))
	, type(type)
	, size(data.size())
{
	assert(this->context);

	if (data.empty())
		throw std::runtime_error("'data' cannot be empty");

	if (type == BufferType::Vertex || type == BufferType::Index)
	{
		auto typ = bufferTypes[type];
		auto usg = bufferUsage[usage];
		auto acc = bufferCPUAccess[cpuAccess];

		D3D11_SUBRESOURCE_DATA bufferData;
		bufferData.pSysMem = data.data();
		bufferData.SysMemPitch = 0;
		bufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC bufferDesc(size, typ, usg, acc);

		HRESULT res = this->context->device->CreateBuffer(&bufferDesc, &bufferData, &buffer);
		if(FAILED(res))
			throw std::runtime_error("could not create buffer");
	}
	else
	{
		pixelBuffer.assign(data.begin(), data.end());
	}
}

HWBufferD3D11::~HWBufferD3D11() {
	buffer = nullptr;
}

std::span<std::byte> HWBufferD3D11::Map(BufferMapAccess access)
{
	if (!IsMapped())
	{
		if (buffer)
		{
			auto mapAccess = bufferMapAccess[access];

			D3D11_MAPPED_SUBRESOURCE resource;
			HRESULT res = context->deviceContext->Map(buffer.Get(), 0, mapAccess, 0, &resource);
			if(FAILED(res))
				throw std::runtime_error(format("Failed to map buffer: %", GetLastError()));

			mapping = std::span<std::byte>{ (std::byte*)resource.pData, size };
		}
		else
		{
			mapping = std::span<std::byte>(pixelBuffer);
		}
	}

	return mapping;
}

bool HWBufferD3D11::IsMapped() const {
	return !mapping.empty();
}

void HWBufferD3D11::Unmap()
{
	if(IsMapped())
	{
		if (buffer)
			context->deviceContext->Unmap(buffer.Get(), 0);
		
		mapping = {};
	}
}

size_t HWBufferD3D11::GetSize() const {
	return size;
}

ComPtr<ID3D11Buffer> HWBufferD3D11::GetBuffer() const {
	assert(buffer); // buffer not available for Pixel* buffers
	return buffer;
}

} // gfx
} // mw
