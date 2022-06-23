/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <MW/System/Internal/Platform.h>

module Microwave.IO.File;
import Microwave.System.Exception;
import <MW/System/Debug.h>;
import <chrono>;
import <fstream>;
import <functional>;
import <regex>;
import <stdexcept>;
import <utility>;

#if PLATFORM_IOS || PLATFORM_MACOS
#  import <Foundation/Foundation.h>
#elif PLATFORM_ANDROID
  import Microwave.IO.AndroidAssetStream;
  import <android/native_activity.h>;
  import <android_native_app_glue.h>;
namespace mw {
    extern android_app* androidApp;
}
#elif PLATFORM_WINDOWS
  import <filesystem>;
#endif

namespace mw {
inline namespace io {

class StandardFileResolver : public FileResolver
{
    std::string scheme;
public:
    virtual const std::string& GetScheme() override {
        return scheme;
    }

    virtual gptr<FileStream> Open(const path& path, OpenMode openMode) override {
        return gpnew<FileStream>(path, openMode);
    }
};

#if PLATFORM_ANDROID
class AndroidAssetResolver : public FileResolver
{
    std::string scheme = "android.asset";
public:
    virtual const std::string& GetScheme() override {
        return scheme;
    }

    virtual gptr<FileStream> Open(const path& path, OpenMode openMode) override {
        return gpnew<AndroidAssetStream>(path, openMode);
    }
};
#endif

class RelativeFileResolver : public FileResolver
{
    std::string scheme;
    path basePath;
    std::function<path(const path&)> pathCompleter;
public:

    RelativeFileResolver(const std::string& scheme, const std::function<path(const path&)>& pathCompleter)
        : scheme(scheme), pathCompleter(pathCompleter)
    {
        Assert(!!pathCompleter);
    }

    virtual const std::string& GetScheme() override {
        return scheme;
    }

    virtual gptr<FileStream> Open(const path& path, OpenMode openMode) override {
        auto fullPath = pathCompleter(path);
        return gpnew<FileStream>(fullPath, openMode);
    }
};

path File::_defaultDataPath = {};

gmap<std::string, gptr<FileResolver>> File::_fileResolvers =
{
    { "", gpnew<StandardFileResolver>() },

#if PLATFORM_ANDROID
    { "android.asset", gpnew<AndroidAssetResolver>() },
    { "android.internal", gpnew<RelativeFileResolver>("android.internal", [](const path& relativePath) { return path((androidApp && androidApp->activity) ? androidApp->activity->internalDataPath : nullptr) / relativePath; }) },
    { "android.external", gpnew<RelativeFileResolver>("android.external", [](const path& relativePath) { return path((androidApp && androidApp->activity) ? androidApp->activity->externalDataPath : nullptr) / relativePath; }) }
#endif
};

const path& File::GetDefaultDataPath()
{
    if (_defaultDataPath.empty())
    {
#if PLATFORM_IOS || PLATFORM_MACOS
        _defaultDataPath = [NSBundle mainBundle].resourcePath.UTF8String;
#elif PLATFORM_ANDROID
        _defaultDataPath = "android.asset://";
#elif PLATFORM_WINDOWS
        _defaultDataPath = std::filesystem::current_path().string();
        _defaultDataPath.make_preferred();
#else
#  error not supported
#endif
    }

    return _defaultDataPath;
}

void File::SetDefaultDataPath(const path& p)
{
    _defaultDataPath = !p.empty() ? p : ".";
}

void File::AddResolver(const gptr<FileResolver>& resolver)
{
    if (!resolver)
        throw Exception("resolver cannot be null");

    auto scheme = resolver->GetScheme();
    Assert(!scheme.empty());

    _fileResolvers[scheme] = resolver;
}

void File::RemoveResolver(const std::string& scheme)
{
    _fileResolvers.erase(scheme);
}

gptr<FileResolver> File::GetResolver(const std::string& scheme)
{
    auto it = _fileResolvers.find(scheme);
    return it != _fileResolvers.end() ? it->second : nullptr;
}

std::tuple<std::string, path> SplitSchemeAndPath(const path& p)
{
    std::string subject = p.begin()->string();
    std::smatch matches;
    std::regex pattern("([\\w.]+)://");

    if (auto hasScheme = std::regex_match(subject, matches, pattern))
    {
        std::string scheme = matches[1].str();
        path relativePath;

        for (auto it = ++p.begin(); it != p.end(); ++it)
            relativePath /= *it;

        return std::make_tuple(scheme, relativePath);
    }
    else
    {
        return std::make_tuple(std::string(), p);
    }
}

gptr<FileStream> File::Open(const path& p, OpenMode openMode)
{
    if (p.empty())
        throw Exception("File::Open -'path' cannot be empty");

    auto [scheme, relativePath] = SplitSchemeAndPath(p);

    auto it = _fileResolvers.find(scheme);
    if (it == _fileResolvers.end())
        throw Exception("file resolver not found for specified scheme");

    auto& resolver = it->second;
    return resolver->Open(relativePath, openMode);
}

std::vector<std::byte> File::ReadAllBytes(const path& p)
{
    auto stream = File::Open(p, OpenMode::In | OpenMode::Binary);
    auto size = stream->GetLength();

    std::vector<std::byte> buffer;
    buffer.resize(size);

    int read = stream->Read(buffer);
    Assert(read == size);

    return buffer;
}

Task<std::vector<std::byte>> File::ReadAllBytesAsync(const path& p)
{
    std::vector<std::byte> buffer;

    co_await ThreadPool::InvokeAsync([&]
    {
        auto stream = File::Open(p, OpenMode::In | OpenMode::Binary);
        auto size = stream->GetLength();

        buffer.resize(size);

        int read = stream->Read(buffer);
        Assert(read == size);
    });

    co_return buffer;
}

void File::WriteAllBytes(const path& p, const std::span<std::byte>& data)
{
    auto stream = File::Open(p, OpenMode::Out | OpenMode::Binary);
    stream->Write(data);
}

Task<void> File::WriteAllBytesAsync(const path& p, const std::span<std::byte>& data)
{
    co_await ThreadPool::InvokeAsync([&]
    {
        auto stream = File::Open(p, OpenMode::Out | OpenMode::Binary);
        stream->Write(data);
    });
}

std::string File::ReadAllText(const path& p)
{
    auto stream = File::Open(p, OpenMode::In | OpenMode::Binary);
    auto size = stream->GetLength();

    std::string text;
    text.resize(size);

    int read = stream->Read(std::span<std::byte>((std::byte*)text.data(), text.size()));
    Assert(read == size);

    return text;
}

Task<std::string> File::ReadAllTextAsync(const path& p)
{
    std::string text;

    co_await ThreadPool::InvokeAsync([&]
    {
        auto stream = File::Open(p, OpenMode::In | OpenMode::Binary);
        auto size = stream->GetLength();

        text.resize(size);

        int read = stream->Read(std::span<std::byte>((std::byte*)text.data(), text.size()));
        Assert(read == size);
    });

    co_return text;
}

void File::WriteAllText(const path& p, std::string_view text) {
    WriteAllBytes(p, std::span<std::byte>((std::byte*)text.data(), text.size()));
}

Task<void> File::WriteAllTextAsync(const path& p, std::string_view text) {
    co_return co_await WriteAllBytesAsync(p, std::span<std::byte>((std::byte*)text.data(), text.size()));
}

uint64_t File::GetLastWriteTime(const path& filePath)
{
    auto lastWrite = path::last_write_time(filePath);

#if defined(_WIN32)
    const std::chrono::seconds SecondsToUnixEpoch = std::chrono::seconds(11644473600);
#else
    const std::chrono::seconds SecondsToUnixEpoch = std::chrono::seconds(0);
#endif

    auto unixTimestamp = lastWrite.time_since_epoch() - SecondsToUnixEpoch;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(unixTimestamp).count();
}

} // io
} // mw
