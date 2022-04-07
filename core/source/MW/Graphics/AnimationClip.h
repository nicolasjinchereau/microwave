/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <MW/System/Json.h>
#include <MW/Graphics/AnimationTrack.h>
#include <MW/Math/Math.h>
#include <MW/Scene/Node.h>
#include <MW/System/Object.h>

namespace mw {
inline namespace scene {
    class Animator;
}
}

namespace mw {
inline namespace gfx {

enum class AnimationWrapMode
{
    Once,
    Sustain,
    Loop
};

class AnimationClip : public Object
{
    REGISTER_OBJECT(AnimationClip);

    std::unordered_map<std::string, sptr<AnimationTrack>> tracks;
    AnimationWrapMode wrapMode = AnimationWrapMode::Once;
    float length = 0;

    friend class mw::scene::Animator;
public:

    void AddTrack(const std::string& path, const sptr<AnimationTrack>& track);
    void ClearTracks();
    float GetLength() const;

    void SetWrapMode(AnimationWrapMode wrapMode);
    AnimationWrapMode GetWrapMode() const;

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;
};

inline void to_json(json& obj, const AnimationWrapMode& wrapMode)
{
    static const char* modeNames[]{ "Once", "Sustain", "Loop", };
    obj = modeNames[(int)wrapMode];
}

inline void from_json(const json& obj, AnimationWrapMode& wrapMode)
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
