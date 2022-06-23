/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Data.Library.AudioClipLoader;
import Microwave.Data.Library.AssetSettings;
import Microwave.Data.Library.AssetManifest;
import Microwave.Audio.AudioClip;
import Microwave.Audio.AudioContext;
import Microwave.System.App;
import Microwave.System.Object;
import Microwave.System.Executor;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Task;
import Microwave.System.UUID;

namespace mw {
inline namespace data {

Task<gptr<Object>> AudioClipLoader::LoadAsync(
    const path& filePath,
    const AssetArtifact& artifact,
    const gptr<Executor>& executor
) const
{
    gptr<AudioClip> obj;

    auto audio = AudioContext::GetCurrent();
    if (audio)
    {
        AudioClipSettings clipSettings = artifact.settings;

        obj = co_await executor->Invoke(
            [fp = filePath, fmt = clipSettings.fileFormat, strm = clipSettings.streamFromDisk]{
                return gpnew<AudioClip>(
                    fp,
                    fmt,
                    strm);
            });

        obj->SetUUID(artifact.uuid);
    }

    co_return obj;
}

} // data
} // mw
