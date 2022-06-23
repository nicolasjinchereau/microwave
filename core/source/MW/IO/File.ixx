/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.IO.File;
import Microwave.IO.FileStream;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Task;
import Microwave.System.ThreadPool;
import <cstddef>;
import <cstdlib>;
import <cstdint>;
import <span>;
import <string>;
import <unordered_map>;
import <vector>;

export namespace mw {
inline namespace io {

class FileResolver
{
public:
    virtual ~FileResolver() = default;
    virtual const std::string& GetScheme() = 0;
    virtual gptr<FileStream> Open(const path& path, OpenMode openMode) = 0;
};

class File
{
    static path _defaultDataPath;
    static gmap<std::string, gptr<FileResolver>> _fileResolvers;
public:

    static const path& GetDefaultDataPath();
    static void SetDefaultDataPath(const path& p);

    static void AddResolver(const gptr<FileResolver>& resolver);
    static void RemoveResolver(const std::string& scheme);
    static gptr<FileResolver> GetResolver(const std::string& scheme);

    // Open a FileStream to the specified path using optional URL scheme
    //   <path>                    = open file using default file system
    //   android.asset://<path>    = open asset stream from Android APK
    //   android.internal://<path> = open file from android internal data path
    //   android.external://<path> = open file from android external data path
    static gptr<FileStream> Open(const path& p, OpenMode openMode);

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
};

}
}
