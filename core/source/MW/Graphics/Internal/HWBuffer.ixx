/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.HWBuffer;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <span>;

export namespace mw {
inline namespace gfx {

enum class BufferType
{
	Vertex,
	Index,
	PixelPack,
	PixelUnpack,
	Texture,
	Uniform
};

enum class BufferCPUAccess
{
	None,
	ReadOnly,
	WriteOnly,
	ReadWrite
};

enum class BufferMapAccess
{
	ReadOnly,
	WriteOnly,
	ReadWrite,
	WriteDiscard,
	WriteNoSync,
};

enum class BufferUsage
{
	Default,
	Stream,
	Static,
	Dynamic
};

class HWBuffer : public Object
{
public:
	virtual ~HWBuffer(){}
	virtual std::span<std::byte> Map(BufferMapAccess access) = 0;
	virtual bool IsMapped() const = 0;
	virtual void Unmap() = 0;
	virtual size_t GetSize() const = 0;
};

} // gfx
} // mw
