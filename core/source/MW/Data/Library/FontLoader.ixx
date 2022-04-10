/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Library.FontLoader;
import Microwave.Data.Library.AssetLoader;
import Microwave.Data.Library.AssetManifest;
import Microwave.Data.Library.AssetSettings;
import Microwave.Graphics.Font;
import Microwave.IO.MemoryStream;
import Microwave.IO.File;
import Microwave.System.Executor;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Task;

export namespace mw {
inline namespace data {

class FontLoader : public AssetLoader
{
public:
    virtual Task<sptr<Object>> LoadAsync(
        const path& filePath,
        const AssetArtifact& artifact,
        const sptr<Executor>& executor
    ) const override
    {
        FontSettings fontSettings = artifact.settings;
        auto data = File::ReadAllBytes(filePath);

        sptr<Font> font = spnew<Font>(
            data,
            fontSettings.atlasSize,
            fontSettings.fontMode,
            fontSettings.spacesPerTab,
            fontSettings.padding,
            fontSettings.margin);

        font->SetUUID(artifact.uuid);

        co_return font;
    }
};

}
}
