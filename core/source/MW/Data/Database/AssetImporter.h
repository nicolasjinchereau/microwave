/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Json.h>
#include <MW/System/path.h>
#include <MW/System/Platforms.h>
#include <MW/System/Pointers.h>
#include <MW/System/UUID.h>
#include <MW/Utilities/Util.h>
#include <MW/IO/File.h>
#include <filesystem>
#include <fstream>
#include <chrono>

namespace mw {
inline namespace data {

class AssetDatabase;
struct AssetMetadata;

class AssetImporter
{
public:
    virtual ~AssetImporter() {}

    virtual std::span<std::string> GetSupportedFileTypes() = 0;

    // should generate artifact files and update metadata
    virtual void ImportFile(
        AssetMetadata& meta,
        const sptr<Stream>& file,
        const path& dataDir) = 0;

    virtual bool Resolve(
        AssetDatabase& assetDatabase,
        AssetMetadata& meta,
        const path& sourceDir,
        const path& dataDir)
    {
        return false;
    }
};

}
}
