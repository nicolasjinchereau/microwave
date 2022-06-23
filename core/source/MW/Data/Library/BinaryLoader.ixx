/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Library.BinaryLoader;
import Microwave.Data.Library.AssetLoader;
import Microwave.Data.Library.AssetManifest;
import Microwave.Data.Library.AssetSettings;
import Microwave.System.Executor;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Task;
import Microwave.IO.MemoryStream;
import Microwave.IO.File;
import <cstddef>;
import <vector>;
import <utility>;

export namespace mw {
inline namespace data {

class BinaryLoader : public AssetLoader
{
public:
    virtual Task<gptr<Object>> LoadAsync(
        const path& filePath,
        const AssetArtifact& artifact,
        const gptr<Executor>& executor
    ) const override
    {
        std::vector<std::byte> data = File::ReadAllBytes(filePath);
        auto obj = gpnew<MemoryStream>(std::move(data));
        obj->SetUUID(artifact.uuid);
        co_return obj;
    }
};

} // data
} // mw
