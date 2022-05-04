/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWBufferOpenGL;
import Microwave.Graphics.Internal.HWBuffer;
import Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.System.Pointers;
import <MW/System/Internal/PlatformHeaders.h>;
import <span>;
import <stdexcept>;
import <unordered_map>;

export namespace mw {
inline namespace gfx {

class HWBufferOpenGL : public HWBuffer
{
	BufferType type{};
	size_t size = 0;
	std::span<std::byte> mapping;
	gl::Uint bufferID = 0;
public:

	HWBufferOpenGL(
		BufferType type, BufferUsage usage,
		BufferCPUAccess cpuAccess, size_t size);

	HWBufferOpenGL(
		BufferType type, BufferUsage usage,
		BufferCPUAccess cpuAccess,
		const std::span<std::byte>& data);

	virtual ~HWBufferOpenGL();
	virtual std::span<std::byte> Map(BufferMapAccess access) override;
	virtual bool IsMapped() const override;
	virtual void Unmap() override;
	virtual size_t GetSize() const override;

	void Bind();
	void ClearBinding();
};

} // gfx
} // mw
