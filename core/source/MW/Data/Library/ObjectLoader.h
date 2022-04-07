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

class ObjectLoader : public AssetLoader
{
public:
    virtual Task<sptr<Object>> LoadAsync(
        const path& filePath,
        const AssetArtifact& artifact,
        const sptr<Executor>& executor
    ) const override
    {
        json val = co_await executor->Invoke(
            [fp = filePath]{
                auto text = File::ReadAllText(fp);
                return json::parse(text);
            });

        ObjectLinker linker;
        auto obj = ObjectFactory::FromJson(val, &linker);
        
        if(obj) {
            obj->SetUUID(artifact.uuid);
            co_await ObjectLinker::LinkAsync(&linker, executor);
        }
        co_return obj;
    }
};

}
}
