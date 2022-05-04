/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <MW/System/Internal/Platform.h>

export module Microwave.IO.FileStream;
export import Microwave.Utilities.EnumFlags;
import Microwave.IO.Stream;
import Microwave.System.Path;
import Microwave.System.ThreadPool;
import Microwave.Utilities.Format;
import <algorithm>;
import <array>;
import <cassert>;
import <cstdlib>;
import <cstdint>;
import <cstdio>;
import <fstream>;
import <filesystem>;
import <memory>;
import <string>;
import <stdexcept>;
import <type_traits>;
import <unordered_map>;
import <vector>;

#if PLATFORM_WINDOWS
    import <io.h>;
    #pragma warning(push)
    #pragma warning(disable : 4996) // disables "Consider using fopen_s instead."
#else
    import <unistd.h>;
    import <sys/stat.h>;
#endif

export namespace mw {
inline namespace io {

enum class OpenMode
{
    In = std::ios_base::in,
    Out = std::ios_base::out,
    AtEnd = std::ios_base::ate,
    Append = std::ios_base::app,
    Truncate = std::ios_base::trunc,
    Binary = std::ios_base::binary,
};

class FileStream : public Stream
{
protected:
    bool readable = true;
    bool seekable = true;
    bool writable = true;
    path p;
    path resolvedPath;
    FILE* file = nullptr;
public:

    FileStream() {}

    FileStream(const path& p, OpenMode openmode)
        : p(p)
    {
        try
        {
            if (p.empty())
                throw std::runtime_error("'path' cannot be empty");

            if ((openmode & (OpenMode::In | OpenMode::Out)) != 0) {
                readable = true;
                writable = true;
            }
            else if ((openmode & OpenMode::Out) != 0) {
                readable = false;
                writable = true;
            }
            else if ((openmode & OpenMode::In) != 0) {
                readable = true;
                writable = false;
            }

            static std::unordered_map<OpenMode, const char*> fopenModes = {
                { OpenMode::In, "r" },
                { OpenMode::Out, "w" },
                { OpenMode::Append, "a" },
                { OpenMode::Out | OpenMode::In, "r+" },
                { OpenMode::Out | OpenMode::In | OpenMode::Truncate, "w+" },
                { OpenMode::Out | OpenMode::In | OpenMode::Append, "a+" },
                { OpenMode::Binary | OpenMode::In, "rb" },
                { OpenMode::Binary | OpenMode::Out, "wb" },
                { OpenMode::Binary | OpenMode::Append, "ab" },
                { OpenMode::Binary | OpenMode::Out | OpenMode::In, "r+b" },
                { OpenMode::Binary | OpenMode::Out | OpenMode::In | OpenMode::Truncate, "w+b" },
                { OpenMode::Binary | OpenMode::Out | OpenMode::In | OpenMode::Append, "a+b" }
            };

            auto it = fopenModes.find(openmode & ~OpenMode::AtEnd);
            if (it == fopenModes.end())
                throw std::runtime_error("invalid OpenMode");

            file = fopen(p.string().c_str(), it->second);
            if (!file)
                throw std::runtime_error(format("failed to open file: %", errno));

            if ((openmode & OpenMode::AtEnd) != 0)
            {
                auto res = FSeek(file, 0, SEEK_END);
                if (res == -1)
                    throw std::runtime_error("could not seek to end of file");
            }
        }
        catch (std::exception&)
        {
            if (file) {
                std::fclose(file);
                file = nullptr;
            }

            this->p.clear();
            throw;
        }
    }

    virtual ~FileStream()
    {
        if (file) {
            std::fclose(file);
            file = nullptr;
        }
    }

    virtual const path& GetPath() const {
        return p;
    }

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
        std::error_code ec;
        return (size_t)std::filesystem::file_size(p.string(), ec);
    }

    virtual size_t GetPosition() const override
    {
        auto pos = FTell(file);
        if (pos == -1)
            throw std::runtime_error(format("failed to retrieve file position: %", errno));

        return (size_t)pos;
    }

    virtual size_t Seek(int64_t offset, SeekOrigin origin) override
    {
        static std::array<int, 3> origins{
            SEEK_SET, // SeekOrigin::Begin
            SEEK_CUR, // SeekOrigin::Current
            SEEK_END  // SeekOrigin::End
        };

        auto res = FSeek(file, offset, origins[(int)origin]);
        if (res == -1)
            throw std::runtime_error(format("failed seek to specified position: %", errno));

        return (size_t)FTell(file);
    }

    virtual void SetLength(size_t length) override
    {
        int ret = FTruncate(file, length);
        if (ret != 0)
            throw std::runtime_error("failed to resize file");

        ret = FSeek(file, 0, SEEK_SET);
        if (ret == -1)
            throw std::runtime_error(format("failed seek to beginning of file: %", errno));
    }

    virtual int Read(std::span<std::byte> buffer) override
    {
        return std::fread(buffer.data(), 1, buffer.size(), file);
    }

    virtual void Write(std::span<std::byte> buffer) override
    {
        size_t written = fwrite(buffer.data(), 1, buffer.size(), file);
        if (written != buffer.size())
            throw std::runtime_error(format("failed to write to file: %", ferror(file)));
    }

    virtual void Flush() override
    {
        auto res = std::fflush(file);
        if (res == -1)
            throw std::runtime_error(format("failed to close file: %", errno));
    }

    virtual void Close() override
    {
        if (file)
        {
            if (std::fclose(file) == -1)
                throw std::runtime_error(format("failed to close file: %", errno));

            file = nullptr;
        }

        readable = true;
        writable = true;
        seekable = true;
    }

private:
    static int FTruncate(FILE* file, size_t size)
    {
#if PLATFORM_WINDOWS
        int ret = _chsize_s(fileno(file), size);
#else
        int ret = ftruncate(fileno(file), size);
#endif
        return ret;
    }
    
    static int FSeek(FILE* file, int64_t offset, int origin) {
#if PLATFORM_WINDOWS
        return _fseeki64(file, offset, origin);
#else
        return fseeko64(file, offset, origin);
#endif
    }

    static int64_t FTell(FILE* file) {
#if PLATFORM_WINDOWS
        return _ftelli64(file);
#else
        return ftello64(file);
#endif
    }
};

} // io
} // mws

#if PLATFORM_WINDOWS
#  pragma warning(pop)
#endif
