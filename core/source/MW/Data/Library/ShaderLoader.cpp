/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Data/Library/ShaderLoader.h>
#include <MW/Graphics/Image.h>
#include <MW/System/App.h>
#include <MW/Utilities/Format.h>

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
