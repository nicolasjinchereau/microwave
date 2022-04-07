/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Data/Library/AssetSettings.h>
#include <MW/Data/Library/AssetLoader.h>
#include <MW/IO/MemoryStream.h>
#include <MW/IO/File.h>

namespace mw {
inline namespace data {

class BinaryLoader : public AssetLoader
{
public:
    virtual Task<sptr<Object>> LoadAsync(
        const path& filePath,
        const AssetArtifact& artifact,
        const sptr<Executor>& executor
    ) const override
    {
        std::vector<std::byte> data = File::ReadAllBytes(filePath);
        auto obj = spnew<MemoryStream>(std::move(data));
        obj->SetUUID(artifact.uuid);
        co_return obj;
    }
};

}
}
