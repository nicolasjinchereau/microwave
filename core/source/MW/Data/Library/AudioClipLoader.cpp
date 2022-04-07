/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Data/Library/AudioClipLoader.h>
#include <MW/Audio/AudioClip.h>
#include <MW/System/App.h>

namespace mw {
inline namespace data {

Task<sptr<Object>> AudioClipLoader::LoadAsync(
    const path& filePath,
    const AssetArtifact& artifact,
    const sptr<Executor>& executor
) const
{
    sptr<AudioClip> obj;

    auto audio = AudioContext::GetCurrent();
    if (audio)
    {
        AudioClipSettings clipSettings = artifact.settings;

        obj = co_await executor->Invoke(
            [fp = filePath, fmt = clipSettings.fileFormat, strm = clipSettings.streamFromDisk]{
                return spnew<AudioClip>(
                    fp,
                    fmt,
                    strm);
            });

        obj->SetUUID(artifact.uuid);
    }

    co_return obj;
}

}
}
