/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.IO.MemoryStream;
import Microwave.IO.Stream;
import Microwave.System.Json;
import Microwave.System.ThreadPool;
import Microwave.Utilities.Base64;
import <algorithm>;
import <array>;
import <cstdlib>;
import <cstdint>;
import <cassert>;
import <fstream>;
import <memory>;
import <span>;
import <stdexcept>;
import <string>;
import <type_traits>;
import <vector>;

export namespace mw {
inline namespace io {

class MemoryStream : public Stream
{
    inline static Type::Pin<MemoryStream> pin;

    bool readable = true;
    bool seekable = true;
    bool writable = true;
    std::vector<std::byte> data;
    size_t position = 0;
public:

    constexpr static size_t DefaultBufferSize = 8192;

    MemoryStream() {}

    MemoryStream(size_t capacity) {
        data.reserve(capacity);
    }

    MemoryStream(const std::vector<std::byte>& data)
        : data(data) {}

    MemoryStream(std::vector<std::byte>&& data)
        : data(std::move(data)) {}

    MemoryStream(const std::vector<std::byte>& data, int offset, int count)
        : data(data.begin() + offset, data.begin() + offset + count) {}

    virtual bool CanRead() const override {
        return readable;
    }

    virtual bool CanSeek() const override {
        return seekable;
    }

    virtual bool CanWrite() const override {
        return writable;
    }

    virtual size_t GetLength() const override {
        return data.size();
    }

    virtual size_t GetPosition() const override {
        return position;
    }

    virtual size_t Seek(int64_t offset, SeekOrigin origin) override
    {
        size_t newPos = 0;

        if (origin == SeekOrigin::Begin)
            newPos = (size_t)offset;
        else if (origin == SeekOrigin::Current)
            newPos = (size_t)(position + offset);
        else if (origin == SeekOrigin::End)
            newPos = (size_t)(data.size() + offset);

        if (newPos > data.size())
            throw std::runtime_error("new position is out of bounds");

        position = newPos;

        return newPos;
    }

    virtual void SetLength(size_t length) override
    {
        data.resize(length);
        position = std::min(position, length);
    }

    virtual int Read(std::span<std::byte> buffer) override
    {
        int read = 0;
        auto src = data.begin() + position;
        auto dst = buffer.begin();

        while (src != data.end() && dst != buffer.end())
        {
            *dst++ = *src++;
            ++read;
        }

        position += read;

        return read;
    }

    virtual void Write(std::span<std::byte> buffer) override
    {
        int written = 0;
        auto src = buffer.begin();
        auto dst = data.begin() + position;

        // overwrite content but stop if end of internal buffer reached
        while (src != buffer.end() && dst != data.end())
        {
            *dst++ = *src++;
            ++written;
        }

        // push remainder onto end of stream
        while (src != buffer.end())
        {
            data.push_back(*src++);
            ++written;
        }

        position += written;
    }

    virtual void Flush() override {}

    virtual void Close() override {
        data.clear();
        position = 0;
        readable = true;
        writable = true;
        seekable = true;
    }

    virtual const std::vector<std::byte>& GetBuffer() const {
        return data;
    }

    virtual void ToJson(json& obj) const override {
        obj["data"] = Base64::Encode(data);
    }

    virtual void FromJson(const json& obj, ObjectLinker* linker) override {
        data = Base64::Decode(obj.value("data", std::string()));
        position = 0;
    }
};

}
}
