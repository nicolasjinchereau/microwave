/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <MW/System/Internal/PlatformHeaders.h>

export module Microwave.Graphics.Internal.HWBufferOpenGL;
import Microwave.Graphics.Internal.HWBuffer;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.System.Pointers;
import std;

export namespace mw {
inline namespace gfx {

class HWBufferOpenGL : public HWBuffer
{
	BufferType type{};
	std::size_t size = 0;
	std::span<std::byte> mapping;
	gl::Uint bufferID = 0;
public:

	HWBufferOpenGL(
		BufferType type, BufferUsage usage,
		BufferCPUAccess cpuAccess, std::size_t size);

	HWBufferOpenGL(
		BufferType type, BufferUsage usage,
		BufferCPUAccess cpuAccess,
		const std::span<std::byte>& data);

	virtual ~HWBufferOpenGL();
	virtual std::span<std::byte> Map(BufferMapAccess access) override;
	virtual bool IsMapped() const override;
	virtual void Unmap() override;
	virtual std::size_t GetSize() const override;

	void Bind();
	void ClearBinding();
};

} // gfx
} // mw
