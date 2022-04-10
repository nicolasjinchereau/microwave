/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Data.Library.TextureLoader;
import Microwave.Data.Library.AssetManifest;
import Microwave.Data.Library.AssetSettings;
import Microwave.Graphics.Image;
import Microwave.Graphics.Texture;
import Microwave.System.App;
import Microwave.System.Executor;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Task;
import Microwave.Utilities.Format;

namespace mw {
inline namespace data {

Task<sptr<Object>> TextureLoader::LoadAsync(
    const path& filePath,
    const AssetArtifact& artifact,
    const sptr<Executor>& executor
) const
{
    sptr<Texture> obj;

    auto graphics = App::Get()->GetGraphics();
    if (graphics)
    {
        TextureSettings texSettings = artifact.settings;
        
        sptr<Image> img = co_await executor->Invoke(
            [fp = filePath, fmt = texSettings.fileFormat] {
                return spnew<Image>(fp, fmt);
            });

        obj = graphics->CreateTexture(img->GetSize(), img->GetFormat(), false, img->GetData());
        obj->SetWrapMode(texSettings.wrapMode);
        obj->SetFilterMode(texSettings.filterMode);
        obj->SetUUID(artifact.uuid);
    }

    co_return obj;
}

}
}
