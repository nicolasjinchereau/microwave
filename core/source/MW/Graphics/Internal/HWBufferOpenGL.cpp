/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWBufferOpenGL;
import Microwave.System.Exception;

namespace mw {
inline namespace gfx {

inline namespace opengl {

std::unordered_map<BufferType, gl::Enum> bufferTypes = {
	{ BufferType::Vertex, gl::ARRAY_BUFFER },
	{ BufferType::Index, gl::ELEMENT_ARRAY_BUFFER },
	{ BufferType::PixelPack, gl::PIXEL_PACK_BUFFER },
	{ BufferType::PixelUnpack, gl::PIXEL_UNPACK_BUFFER },
	{ BufferType::Texture, gl::TEXTURE_BUFFER },
	{ BufferType::Uniform, gl::UNIFORM_BUFFER }
};

std::unordered_map<BufferCPUAccess, gl::Bitfield> bufferCPUAccess = {
	{ BufferCPUAccess::None, 0 },
	{ BufferCPUAccess::ReadOnly, gl::MAP_READ_BIT },
	{ BufferCPUAccess::WriteOnly, gl::DYNAMIC_STORAGE_BIT | gl::MAP_WRITE_BIT },
	{ BufferCPUAccess::ReadWrite, gl::DYNAMIC_STORAGE_BIT | gl::MAP_READ_BIT | gl::MAP_WRITE_BIT }
};

std::unordered_map<BufferMapAccess, gl::Bitfield> bufferMapAccess = {
	{ BufferMapAccess::ReadOnly, gl::MAP_READ_BIT },
	{ BufferMapAccess::WriteOnly, gl::MAP_WRITE_BIT },
	{ BufferMapAccess::ReadWrite, gl::MAP_READ_BIT | gl::MAP_WRITE_BIT },
	{ BufferMapAccess::WriteDiscard, gl::MAP_WRITE_BIT | gl::MAP_INVALIDATE_BUFFER_BIT },
	{ BufferMapAccess::WriteNoSync, gl::MAP_WRITE_BIT | gl::MAP_UNSYNCHRONIZED_BIT }
};

std::unordered_map<BufferUsage, gl::Enum> bufferUsage = {
	{ BufferUsage::Default, gl::DYNAMIC_DRAW },
	{ BufferUsage::Dynamic, gl::DYNAMIC_DRAW },
	{ BufferUsage::Static, gl::STATIC_DRAW },
	{ BufferUsage::Stream, gl::STREAM_DRAW }
};

} // opengl

HWBufferOpenGL::HWBufferOpenGL(
	BufferType type, BufferUsage usage,
	BufferCPUAccess cpuAccess, size_t size)
	: type(type), size(size)
{
	auto typ = bufferTypes[type];
	auto usg = bufferUsage[usage];
	auto acc = bufferCPUAccess[cpuAccess];

	gl::GenBuffers(1, &bufferID);
	gl::BindBuffer(typ, bufferID);
	//gl::BufferStorage(typ, size, nullptr, acc);
	gl::BufferData(typ, size, nullptr, usg);
	gl::BindBuffer(typ, 0);
}

HWBufferOpenGL::HWBufferOpenGL(
	BufferType type, BufferUsage usage,
	BufferCPUAccess cpuAccess, const std::span<std::byte>& data)
	: type(type), size(data.size())
{
	if (data.empty())
		throw Exception("'data' cannot be empty");

	auto typ = bufferTypes[type];
	auto usg = bufferUsage[usage];
	auto acc = bufferCPUAccess[cpuAccess];

	gl::GenBuffers(1, &bufferID);
	gl::BindBuffer(typ, bufferID);
	//gl::BufferStorage(typ, size, data.data(), acc);
	gl::BufferData(typ, size, data.data(), usg);
	gl::BindBuffer(typ, 0);
}

HWBufferOpenGL::~HWBufferOpenGL()
{
	if (gl::IsBuffer(bufferID))
		gl::DeleteBuffers(1, &bufferID);
}

std::span<std::byte> HWBufferOpenGL::Map(BufferMapAccess access)
{
	if (!IsMapped())
	{
		auto typ = bufferTypes[type];
		auto acc = bufferMapAccess[access];

		gl::BindBuffer(typ, bufferID);
		void* pData = gl::MapBufferRange(typ, 0, size, acc);
		gl::BindBuffer(typ, 0);

		mapping = std::span<std::byte>{ (std::byte*)pData, size };
	}

	return mapping;
}

bool HWBufferOpenGL::IsMapped() const {
	return !mapping.empty();
}

void HWBufferOpenGL::Unmap()
{
	if(IsMapped())
	{
		auto typ = bufferTypes[type];
		gl::BindBuffer(typ, bufferID);
		gl::UnmapBuffer(typ);
		gl::BindBuffer(typ, 0);

		mapping = {};
	}
}

size_t HWBufferOpenGL::GetSize() const {
	return size;
}

void HWBufferOpenGL::Bind() {
	gl::BindBuffer(bufferTypes[type], bufferID);
}

void HWBufferOpenGL::ClearBinding() {
	gl::BindBuffer(bufferTypes[type], 0);
}

} // gfx
} // mw
