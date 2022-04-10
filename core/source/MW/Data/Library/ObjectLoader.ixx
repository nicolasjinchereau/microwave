/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Library.ObjectLoader;
import Microwave.Data.Library.AssetLoader;
import Microwave.Data.Library.AssetManifest;
import Microwave.Data.Library.AssetSettings;
import Microwave.IO.MemoryStream;
import Microwave.IO.File;
import Microwave.System.Executor;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Task;

export namespace mw {
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
        auto obj = Object::CreateFromJson(val, &linker);
        
        if(obj) {
            obj->SetUUID(artifact.uuid);
            co_await ObjectLinker::LinkAsync(&linker, executor);
        }
        co_return obj;
    }
};

} // data
} // mw
