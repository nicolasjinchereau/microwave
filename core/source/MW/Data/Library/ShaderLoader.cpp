/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Data.Library.ShaderLoader;
import Microwave.Data.Library.AssetManifest;
import Microwave.Data.Library.AssetSettings;
import Microwave.IO.File;
import Microwave.Graphics.Shader;
import Microwave.Graphics.GraphicsContext;
import Microwave.System.App;
import Microwave.System.Executor;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Task;
import Microwave.Utilities.Format;
import <stdexcept>;
import <string>;

namespace mw {
inline namespace data {

Task<sptr<Object>> ShaderLoader::LoadAsync(
    const path& filePath,
    const AssetArtifact& artifact,
    const sptr<Executor>& executor
) const
{
    sptr<Shader> obj;

    auto graphics = App::Get()->GetGraphics();
    if (graphics)
    {
        std::string source = co_await executor->Invoke([fp = filePath]{
            return File::ReadAllText(fp);
        });

        try {
            obj = graphics->CreateShader(source);
            obj->SetUUID(artifact.uuid);
        }
        catch (std::runtime_error& err) {
            throw FormattedError("% %", artifact.sourcePath.string(), err.what());
        }
    }

    co_return obj;
}

} // data
} // mw
