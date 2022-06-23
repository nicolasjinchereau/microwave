/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Data.Library.ShaderLoader;
import Microwave.Data.Library.AssetManifest;
import Microwave.Data.Library.AssetSettings;
import Microwave.IO.File;
import Microwave.Graphics.Shader;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.ShaderInfo;
import Microwave.System.Exception;
import Microwave.System.Executor;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.Task;
import <stdexcept>;
import <string>;

namespace mw {
inline namespace data {

Task<gptr<Object>> ShaderLoader::LoadAsync(
    const path& filePath,
    const AssetArtifact& artifact,
    const gptr<Executor>& executor
) const
{
    gptr<Shader> obj;

    auto graphics = GraphicsContext::GetCurrent();
    if (graphics)
    {
        try
        {
            gptr<ShaderInfo> info = co_await executor->Invoke(
                [fp = filePath, ctx = graphics->context] {
                    std::string source = File::ReadAllText(fp);
                    return gpnew<ShaderInfo>(source, ctx->GetShaderLanguage());
                });

            obj = gpnew<Shader>(info);
            obj->SetUUID(artifact.uuid);
        }
        catch (const Exception& err) {
            throw Exception({ artifact.sourcePath.string(), " ", err.what() });
        }
    }

    co_return obj;
}

} // data
} // mw
