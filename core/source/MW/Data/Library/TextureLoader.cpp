/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Data/Library/TextureLoader.h>
#include <MW/Graphics/Image.h>
#include <MW/System/App.h>

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
