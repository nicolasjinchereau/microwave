/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Database.AssetImporter;
import Microwave.Data.Database.AssetDatabase;
import Microwave.Data.Library.AssetSettings;
import Microwave.IO.File;
import Microwave.IO.Stream;
import Microwave.System.Json;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.UUID;
import Microwave.Utilities.Util;
import <filesystem>;
import <fstream>;
import <span>;
import <string>;

export namespace mw {
inline namespace data {

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
