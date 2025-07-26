/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <MW/System/Internal/PlatformHeaders.h>

export module Microwave.Graphics.Internal.HWBufferD3D11;
import Microwave.Graphics.Internal.HWBuffer;
import Microwave.System.Pointers;
import std;

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

export namespace mw {
inline namespace gfx {

class HWContext;
class HWContextD3D11;

class HWBufferD3D11 : public HWBuffer
{
	BufferType type{};
	std::size_t size = 0;
	std::span<std::byte> mapping;
	ComPtr<ID3D11Buffer> buffer;
	std::vector<std::byte> pixelBuffer;
	gptr<HWContextD3D11> context;
public:

	HWBufferD3D11(
		const gptr<HWContext>& context,
		BufferType type, BufferUsage usage,
		BufferCPUAccess cpuAccess, std::size_t size);

	HWBufferD3D11(
		const gptr<HWContext>& context,
		BufferType type, BufferUsage usage,
		BufferCPUAccess cpuAccess,
		const std::span<std::byte>& data);

	virtual ~HWBufferD3D11();
	virtual std::span<std::byte> Map(BufferMapAccess access) override;
	virtual bool IsMapped() const override;
	virtual void Unmap() override;
	virtual std::size_t GetSize() const override;

	ComPtr<ID3D11Buffer> GetBuffer() const;
};

} // gfx
} // mw
