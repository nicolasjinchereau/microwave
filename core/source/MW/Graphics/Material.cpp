/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Material;
import Microwave.Graphics.Color;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.MaterialPropertyBlock;
import Microwave.Graphics.RenderQueue;
import Microwave.Graphics.Shader;
import Microwave.Graphics.Texture;
import Microwave.Math;
import Microwave.System.Exception;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.UUID;
import <MW/System/Debug.h>;
import <cstdint>;
import <string>;
import <unordered_map>;

namespace mw {
inline namespace gfx {

void Material::SetActive(const MaterialPropertyBlock* extra)
{
    Assert(shader);

    auto graphics = GraphicsContext::GetCurrent();
    Assert(graphics);

    shader->Bind();

    for (auto& [name, prop] : properties->properties)
        prop->Apply(shader);

    if (extra)
    {
        for (auto& [name, prop] : extra->properties)
            prop->Apply(shader);
    }

    graphics->SetBlendOperations(colorBlendOperation, alphaBlendOperation);
    graphics->SetBlendFactors(
        sourceColorBlendFactor, destColorBlendFactor,
        sourceAlphaBlendFactor, destAlphaBlendFactor);
    graphics->SetCullMode(cullMode);
    graphics->SetDepthTest(depthTest);
    graphics->SetDepthWriteEnabled(depthWriteEnabled);
}

void Material::ToJson(json& obj) const
{
    Object::ToJson(obj);

    obj["colorBlendOperation"] = colorBlendOperation;
    obj["alphaBlendOperation"] = alphaBlendOperation;
    obj["sourceColorBlendFactor"] = sourceColorBlendFactor;
    obj["sourceAlphaBlendFactor"] = sourceAlphaBlendFactor;
    obj["destColorBlendFactor"] = destColorBlendFactor;
    obj["destAlphaBlendFactor"] = destAlphaBlendFactor;
    obj["cullMode"] = cullMode;
    obj["depthTest"] = depthTest;
    obj["depthWriteEnabled"] = depthWriteEnabled;
    obj["renderQueue"] = renderQueue;
    properties->ToJson(obj["properties"]);
    ObjectLinker::SaveAsset(obj, "shader", shader);
}

void Material::FromJson(const json& obj, ObjectLinker* linker)
{
    Object::FromJson(obj, linker);

    colorBlendOperation = obj.value("colorBlendOperation", colorBlendOperation);
    alphaBlendOperation = obj.value("alphaBlendOperation", alphaBlendOperation);
    sourceColorBlendFactor = obj.value("sourceColorBlendFactor", sourceColorBlendFactor);
    sourceAlphaBlendFactor = obj.value("sourceAlphaBlendFactor", sourceAlphaBlendFactor);
    destColorBlendFactor = obj.value("destColorBlendFactor", destColorBlendFactor);
    destAlphaBlendFactor = obj.value("destAlphaBlendFactor", destAlphaBlendFactor);
    cullMode = obj.value("cullMode", cullMode);
    depthTest = obj.value("depthTest", depthTest);
    depthWriteEnabled = obj.value("depthWriteEnabled", depthWriteEnabled);
    renderQueue = obj.value("renderQueue", renderQueue);
    ObjectLinker::RestoreAsset(linker, self(this), shader, obj, "shader");

    const json& props = obj["properties"];
    properties->FromJson(props, linker);
}

} // gfx
} // mw
