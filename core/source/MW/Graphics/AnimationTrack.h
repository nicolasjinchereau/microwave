/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <cstdint>
#include <vector>
#include <MW/System/Json.h>
#include <MW/Math/Math.h>

namespace mw {
inline namespace gfx {

class Keyframe
{
public:
    float time;
    Transform value;
};

class AnimationTrack
{
public:
    std::vector<Keyframe> frames;

    Transform Evaluate(float time)
    {
        if (frames.empty())
            return {};

        if (frames.size() == 1)
            return frames.begin()->value;

        auto it = std::lower_bound(
            frames.begin(), frames.end(), time,
            [](const Keyframe& k, float t) { return k.time < t; });

        if (it == frames.begin())
        {
            return it->value;
        }
        else if (it == frames.end())
        {
            return (it - 1)->value;
        }
        else
        {
            const auto& a = *(it - 1);
            const auto& b = *it;
            assert((b.time - a.time) >= FLT_EPSILON);
            auto t = (time - a.time) / (b.time - a.time);

            Transform ret;
            ret.position = Vec3::Lerp(a.value.position, b.value.position, t);
            ret.rotation = Quat::Slerp(a.value.rotation, b.value.rotation, t);
            ret.scale = Vec3::Lerp(a.value.scale, b.value.scale, t);
            return ret;
        }
    }

    float GetLength() const {
        return frames.empty() ? 0 : frames.back().time;
    }
};

inline void to_json(json& obj, const Keyframe& keyframe)
{
    obj["time"] = keyframe.time;
    obj["value"] = keyframe.value;
}

inline void from_json(const json& obj, Keyframe& keyframe)
{
    keyframe.time = obj["time"];
    keyframe.value = obj["value"];
}

inline void to_json(json& obj, const AnimationTrack& track)
{
    obj["frames"] = track.frames;
}

inline void from_json(const json& obj, AnimationTrack& track)
{
    track.frames = obj.value("frames", std::vector<Keyframe>());
}

}
}
