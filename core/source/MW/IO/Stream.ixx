/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.IO.Stream;
import Microwave.System.Exception;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.System.Task;
import Microwave.System.ThreadPool;
import std;

export namespace mw {
inline namespace io {

enum class SeekOrigin
{
    Begin,
    Current,
    End
};

class Stream : public Object
{
public:
    constexpr static std::size_t DefaultBufferSize = 8192;

    virtual ~Stream() {}

    virtual bool CanRead() const = 0;
    virtual bool CanSeek() const = 0;
    virtual bool CanWrite() const = 0;
    virtual std::size_t GetLength() const = 0;
    virtual std::size_t GetPosition() const = 0;
    virtual std::size_t Seek(std::int64_t offset, SeekOrigin origin) = 0;
    virtual void SetLength(std::size_t length) = 0;
    virtual int Read(std::span<std::byte> buffer) = 0;
    virtual void Write(std::span<std::byte> buffer) = 0;
    virtual void Flush() = 0;
    virtual void Close() = 0;
    
    // implemented through calls to above functions
    void Ignore(std::size_t bytes);
    void SetPosition(std::size_t position);
    Task<void> FlushAsync();
    Task<int> ReadAsync(std::span<std::byte> buffer);
    Task<void> WriteAsync(std::span<std::byte> buffer);
    void CopyTo(gptr<Stream>& stream);
    Task<void> CopyToAsync(gptr<Stream>& stream);
    template<class T> T ReadValue();
    template<class T> bool ReadValue(T& output);
    template<class T> void WriteValue(const T& value);
};

void Stream::Ignore(std::size_t bytes) {
    Seek(bytes, SeekOrigin::Current);
}

void Stream::SetPosition(std::size_t position) {
    Seek(position, SeekOrigin::Begin);
}

Task<void> Stream::FlushAsync() {
    co_await ThreadPool::InvokeAsync([this] { Flush(); });
}

Task<int> Stream::ReadAsync(std::span<std::byte> buffer)
{
    int read = 0;
    co_await ThreadPool::InvokeAsync([&] { read = Read(buffer); });
    co_return read;
}

Task<void> Stream::WriteAsync(std::span<std::byte> buffer)
{
    co_await ThreadPool::InvokeAsync([&] { Write(buffer); });
}

void Stream::CopyTo(gptr<Stream>& stream)
{
    std::array<std::byte, DefaultBufferSize> buffer;

    int count;
    while ((count = Read(buffer)) > 0)
    {
        stream->Write(std::span<std::byte>(buffer.data(), count));
    }
}

Task<void> Stream::CopyToAsync(gptr<Stream>& stream) {
    co_await ThreadPool::InvokeAsync([&] { CopyTo(stream); });
}

template<class T>
T Stream::ReadValue()
{
    T ret;
    std::size_t read = Read({ (std::byte*)&ret, sizeof(T) });
    if (read < sizeof(T)) throw Exception("unexpected end of stream");
    return ret;
}

template<class T>
bool Stream::ReadValue(T& output)
{
    std::size_t read = Read({ (std::byte*)&output, sizeof(T) });
    return read == sizeof(T);
}

template<class T>
void Stream::WriteValue(const T& value) {
    Write({ (std::byte*)&value, sizeof(T) });
}

} // io
} // mw
