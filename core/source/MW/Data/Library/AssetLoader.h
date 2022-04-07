/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <span>
#include <string>
#include <vector>
#include <MW/Data/Library/AssetType.h>
#include <MW/Data/Library/AssetManifest.h>
#include <MW/System/Json.h>
#include <MW/System/Object.h>
#include <MW/System/path.h>
#include <MW/System/Executor.h>

namespace mw {
inline namespace data {

class AssetLoader
{
public:
    virtual Task<sptr<Object>> LoadAsync(
        const path& filePath,
        const AssetArtifact& artifact,
        const sptr<Executor>& executor
    ) const = 0;
};

}
}
