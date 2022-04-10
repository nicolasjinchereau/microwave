/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Library.ShaderLoader;
import Microwave.Data.Library.AssetLoader;
import Microwave.Data.Library.AssetManifest;
import Microwave.Data.Library.AssetSettings;
import Microwave.System.Executor;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Task;

export namespace mw {
inline namespace data {

class ShaderLoader : public AssetLoader
{
public:
    virtual Task<sptr<Object>> LoadAsync(
        const path& filePath,
        const AssetArtifact& artifact,
        const sptr<Executor>& executor
    ) const override;
};

}
}
