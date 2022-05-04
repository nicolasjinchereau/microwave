/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWBufferD3D11;
import Microwave.Graphics.Internal.HWBuffer;
import Microwave.System.Pointers;
import Microwave.Utilities.Format;
import <MW/System/Internal/PlatformHeaders.h>;
import <span>;
import <stdexcept>;
import <unordered_map>;

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

export namespace mw {
inline namespace gfx {

class HWContext;
class HWContextD3D11;

class HWBufferD3D11 : public HWBuffer
{
	BufferType type{};
	size_t size = 0;
	std::span<std::byte> mapping;
	ComPtr<ID3D11Buffer> buffer;
	std::vector<std::byte> pixelBuffer;
	sptr<HWContextD3D11> context;
public:

	HWBufferD3D11(
		const sptr<HWContext>& context,
		BufferType type, BufferUsage usage,
		BufferCPUAccess cpuAccess, size_t size);

	HWBufferD3D11(
		const sptr<HWContext>& context,
		BufferType type, BufferUsage usage,
		BufferCPUAccess cpuAccess,
		const std::span<std::byte>& data);

	virtual ~HWBufferD3D11();
	virtual std::span<std::byte> Map(BufferMapAccess access) override;
	virtual bool IsMapped() const override;
	virtual void Unmap() override;
	virtual size_t GetSize() const override;

	ComPtr<ID3D11Buffer> GetBuffer() const;
};

} // gfx
} // mw
