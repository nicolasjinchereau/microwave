/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Data/Library/AssetSettings.h>
#include <MW/Data/Library/AssetLoader.h>
#include <MW/Graphics/Font.h>
#include <MW/IO/MemoryStream.h>
#include <MW/IO/File.h>

namespace mw {
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
