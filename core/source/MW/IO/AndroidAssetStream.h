/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/ThreadPool.h>
#include <MW/IO/Stream.h>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <array>
#include <fstream>
#include <MW/System/path.h>
#include <MW/IO/FileStream.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>

namespace mw {
    extern android_app* androidApp;
}

namespace mw::io
{

class AndroidAssetStream : public FileStream
{
    AAsset* asset = nullptr;
    size_t position = 0;

public:

    AndroidAssetStream(){}

    AndroidAssetStream(const mw::path& path, OpenMode openmode)
    {
        try
        {
            if (path.empty())
                throw std::runtime_error("'filename' cannot be empty");

            this->path = path;
            
            if ((openmode & OpenMode::Out) != 0)
                throw std::runtime_error("OpenMode::Out not supported for AndroidAssetStream");

            if ((openmode & OpenMode::Append) != 0)
                throw std::runtime_error("OpenMode::Append not supported for AndroidAssetStream");

            if ((openmode & OpenMode::Truncate) != 0)
                throw std::runtime_error("OpenMode::Truncate not supported for AndroidAssetStream");

            if ((openmode & OpenMode::AtEnd) != 0)
                throw std::runtime_error("OpenMode::AtEnd not supported for AndroidAssetStream");

            readable = true;
            seekable = true;
            writable = false;
            position = 0;
            
            asset = AAssetManager_open(androidApp->activity->assetManager, path.string().c_str(), AASSET_MODE_RANDOM);
            if (!asset)
                throw std::runtime_error(format("failed to open file: %", path.string()));
        }
        catch (...)
        {
            if (asset) {
                AAsset_close(asset);
                asset = nullptr;
            }

            throw;
        }
    }
    
    virtual ~AndroidAssetStream()
    {
        if (asset) {
            AAsset_close(asset);
            asset = nullptr;
        }
    }

    virtual size_t GetLength() const override {
        return (size_t)AAsset_getLength(asset);
    }

    virtual size_t GetPosition() const override {
        return position;
    }

    virtual void SetPosition(size_t position) override
    {
        auto res = AAsset_seek(asset, position, SEEK_SET);
        if(res == -1)
            throw std::runtime_error(format("failed to set file position: %", errno));

        this->position = position;
    }

    virtual void Close() override
    {
        if (asset) {
            AAsset_close(asset);
            asset = nullptr;
        }

        readable = false;
        writable = false;
        seekable = false;
        position = 0;
    }

    virtual void Flush() override {
        throw std::runtime_error("Flush is not supported by AndroidAssetStream");
    }

    virtual Task<void> FlushAsync() override {
        throw std::runtime_error("FlushAsync is not supported by AndroidAssetStream");
    }

    virtual int Read(mw::span<std::byte> buffer, int offset, int count) override
    {
        if (buffer.size() - offset < count)
            throw std::runtime_error("requested range is out of bounds of 'buffer'");
        
        int read = AAsset_read(asset, buffer.data() + offset, count);
        if(read > 0)
            position += read;
        
        return read;
    }

    virtual Task<int> ReadAsync(mw::span<std::byte> buffer, int offset, int count) override
    {
        int read = 0;
        co_await ThreadPool::InvokeAsync([&]{ read = Read(buffer, offset, count); });
        co_return read;
    }

    virtual size_t Seek(int64_t offset, SeekOrigin origin) override
    {
        static std::unordered_map<SeekOrigin, int> origins {
            { SeekOrigin::Begin, SEEK_SET },
            { SeekOrigin::Current, SEEK_CUR },
            { SeekOrigin::End, SEEK_END }
        };

        auto res = AAsset_seek(asset, (long)offset, origins[origin]);
        if(res == -1)
            throw std::runtime_error(format("failed to seek to specified position: %", errno));

        position = res;

        return position;
    }

    virtual void SetLength(size_t length) override {
        throw std::runtime_error("SetLength is not supported by AndroidAssetStream");
    }

    virtual void Write(mw::span<std::byte> buffer, int offset, int count) override {
        throw std::runtime_error("Write is not supported by AndroidAssetStream");
    }

    virtual Task<void> WriteAsync(mw::span<std::byte> buffer, int offset, int count) override {
        throw std::runtime_error("WriteAsync is not supported by AndroidAssetStream");
    }
};

}

#pragma warning(pop)
