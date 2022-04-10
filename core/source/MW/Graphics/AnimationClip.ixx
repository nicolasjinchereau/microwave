/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.AnimationClip;
import Microwave.Graphics.AnimationTrack;
import Microwave.Math;
import Microwave.SceneGraph.Node;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <cstdint>;
import <string>;
import <unordered_map>;
import <vector>;

export namespace mw {
inline namespace gfx {

enum class AnimationWrapMode
{
    Once,
    Sustain,
    Loop
};

class AnimationClip : public Object
{
    inline static Type::Pin<AnimationClip> pin;

    std::unordered_map<std::string, sptr<AnimationTrack>> tracks;
    AnimationWrapMode wrapMode = AnimationWrapMode::Once;
    float length = 0;
public:

    void AddTrack(const std::string& path, const sptr<AnimationTrack>& track);
    void ClearTracks();
    const auto& GetTracks() const { return tracks; }
    float GetLength() const;

    void SetWrapMode(AnimationWrapMode wrapMode);
    AnimationWrapMode GetWrapMode() const;

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;
};

void to_json(json& obj, const AnimationWrapMode& wrapMode)
{
    static const char* modeNames[]{ "Once", "Sustain", "Loop", };
    obj = modeNames[(int)wrapMode];
}

void from_json(const json& obj, AnimationWrapMode& wrapMode)
{
    static std::unordered_map<std::string, AnimationWrapMode> modes{
        { "Once", AnimationWrapMode::Once },
        { "Sustain", AnimationWrapMode::Sustain },
        { "Loop", AnimationWrapMode::Loop }
    };
    wrapMode = modes[obj.get<std::string>("Once")];
}

}
}
