/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Data.Library.AssetType;
import Microwave.System.Json;
import <unordered_map>;
import <string>;

export namespace mw {
inline namespace data {

enum class AssetType
{
    AnimationClip, // mw::gfx::AnimationClip
    AudioClip,     // mw::audio::AudioClip
    Binary,        // mw::io::MemoryStream
    Font,          // mw::ui::Font
    Material,      // mw::gfx::Material
    Mesh,          // mw::gfx::Mesh
    Node,          // mw::scene::Node
    Shader,        // mw::gfx::Shader
    Texture,       // mw::gfx::Texture
};

void to_json(json& obj, const AssetType& type)
{
    std::unordered_map<AssetType, std::string> names {
        { AssetType::AnimationClip, "AnimationClip" },
        { AssetType::AudioClip, "AudioClip" },
        { AssetType::Binary, "Binary" },
        { AssetType::Font, "Font" },
        { AssetType::Material, "Material" },
        { AssetType::Mesh, "Mesh" },
        { AssetType::Node, "Node" },
        { AssetType::Shader, "Shader" },
        { AssetType::Texture, "Texture" }
    };

    obj = names[type];
}

void from_json(const json& obj, AssetType& type)
{
    std::unordered_map<std::string, AssetType> types {
        { "AnimationClip", AssetType::AnimationClip },
        { "AudioClip", AssetType::AudioClip },
        { "Binary", AssetType::Binary },
        { "Font", AssetType::Font },
        { "Material", AssetType::Material },
        { "Mesh", AssetType::Mesh },
        { "Node", AssetType::Node },
        { "Shader", AssetType::Shader },
        { "Texture", AssetType::Texture }
    };

    type = types[obj.get<std::string>()];
}

} // data
} // mw
