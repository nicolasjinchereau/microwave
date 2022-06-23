/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.AnimationClip;
import Microwave.SceneGraph.Components.Animator;
import Microwave.SceneGraph.Node;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <string>;
import <utility>;

namespace mw {
inline namespace gfx {

void AnimationClip::ToJson(json& obj) const
{
    Object::ToJson(obj);

    json trackObjs = json::object();
    for (auto& [path, track] : tracks)
        trackObjs[path] = *track;

    obj["tracks"] = std::move(trackObjs);
    obj["wrapMode"] = wrapMode;
}

void AnimationClip::FromJson(const json& obj, ObjectLinker* linker)
{
    Object::FromJson(obj, linker);

    length = 0;

    const json& trackObjs = obj["tracks"];
    for (auto& [key, trackObj] : trackObjs.items())
    {
        auto track = gpnew<AnimationTrack>();
        from_json(trackObj, *track);
        tracks[key] = track;

        if(!track->frames.empty())
            length = std::max(length, track->frames.back().time);
    }

    wrapMode = obj.value("wrapMode", wrapMode);
}

void AnimationClip::AddTrack(const std::string& path, const gptr<AnimationTrack>& track)
{
    tracks[path] = track;

    if (!track->frames.empty())
        length = std::max(length, track->frames.back().time);
}

void AnimationClip::ClearTracks()
{
    tracks.clear();
    length = 0;
}

float AnimationClip::GetLength() const {
    return length;
}

void AnimationClip::SetWrapMode(AnimationWrapMode wrapMode) {
    this->wrapMode = wrapMode;
}

AnimationWrapMode AnimationClip::GetWrapMode() const {
    return wrapMode;
}

}
}
