/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/path.h>
#include <MW/System/ThreadPool.h>
#include <MW/IO/FileStream.h>
#include <cstdlib>
#include <cstddef>
#include <span>
#include <string>
#include <vector>

namespace mw {
inline namespace io {

class FileResolver
{
public:
    virtual ~FileResolver() = default;
    virtual const std::string& GetScheme() = 0;
    virtual sptr<FileStream> Open(const path& path, OpenMode openMode) = 0;
};

class File
{
    static path _defaultDataPath;
    static std::unordered_map<std::string, sptr<FileResolver>> _fileResolvers;
public:

    static const path& GetDefaultDataPath();
    static void SetDefaultDataPath(const path& p);

    static void AddResolver(const sptr<FileResolver>& resolver);
    static void RemoveResolver(const std::string& scheme);
    static sptr<FileResolver> GetResolver(const std::string& scheme);

    // Open a FileStream to the specified path using optional URL scheme
    //   <path>                    = open file using default file system
    //   android.asset://<path>    = open asset stream from Android APK
    //   android.internal://<path> = open file from android internal data path
    //   android.external://<path> = open file from android external data path
    static sptr<FileStream> Open(const path& p, OpenMode openMode);

    static std::vector<std::byte> ReadAllBytes(const path& p);
    static Task<std::vector<std::byte>> ReadAllBytesAsync(const path& p);

    static void WriteAllBytes(const path& p, const std::span<std::byte>& data);
    static Task<void> WriteAllBytesAsync(const path& p, const std::span<std::byte>& data);

    static std::string ReadAllText(const path& p);
    static Task<std::string> ReadAllTextAsync(const path& p);

    static void WriteAllText(const path& p, std::string_view text);
    static Task<void> WriteAllTextAsync(const path& p, std::string_view text);

    // returns unix nanoseconds
    static uint64_t GetLastWriteTime(const path& filePath);
    static uint64_t GetTimestamp();
};

}
}
