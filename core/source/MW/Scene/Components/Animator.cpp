/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Scene/Components/Animator.h>
#include <MW/System/App.h>
#include <MW/System/Json.h>

namespace mw {
inline namespace scene {

void Animator::ToJson(json& obj) const
{
    Component::ToJson(obj);

    json animStateObjs = json::object();

    for(auto& [name, state] : animationStates)
    {
        json stateObj;
        ObjectLinker::SaveAsset(stateObj, "clip", state->clip);
        stateObj["speed"] = state->speed;
        animStateObjs[name] = stateObj;
    }
    
    obj["animationStates"] = animStateObjs;
}

void Animator::FromJson(const json& obj, ObjectLinker* linker)
{
    Component::FromJson(obj, linker);
    
    auto assetLib = App::Get()->GetAssetLibrary();

    const json& animStateObjs = obj["animationStates"];

    for (auto& [name, stateObj] : animStateObjs.items())
    {
        auto state = spnew<AnimationState>();
        ObjectLinker::RestoreAsset(linker, This(), state->clip, stateObj, "clip");
        state->speed = stateObj["speed"];
        animationStates[name] = std::move(state);
    }

    dirtyInfluences = true;
}

void Animator::StopAnimation(AnimationState& state)
{
    state.time = 0;
    state.weight = 0;
    state.targetWeight = 0;
    state.weightFadeRate = 0;
    state.enabled = false;
    state.fadingOut = false;
}

void Animator::UpdateInfluences()
{
    if(dirtyInfluences)
    {
        influences.clear();

        for (auto& [name, state] : animationStates)
        {
            for(auto& [path, track] : state->clip->tracks)
                influences[path] = {};
        }

        dirtyInfluences = false;
    }
}

void Animator::AddClip(const sptr<AnimationClip>& clip, const std::string& clipName)
{
    auto state = spnew<AnimationState>();
    state->clip = clip;
    animationStates[clipName] = state;
    dirtyInfluences = true;
}

void Animator::RemoveClip(const sptr<AnimationClip>& clip)
{
    for(auto it = animationStates.begin();
        it != animationStates.end(); )
    {
        if(it->second->clip == clip)
            it = animationStates.erase(it);
        else
            ++it;
    }

    dirtyInfluences = true;
}

void Animator::RemoveClip(const std::string& clipName) {
    animationStates.erase(clipName);
    dirtyInfluences = true;
}

int Animator::GetClipCount() const {
    return animationStates.size();
}

void Animator::UpdateAnimationStates()
{
    auto deltaTime = GetClock()->GetDeltaTime();

    for (auto& [name, state] : animationStates)
    {
        if (state->enabled)
        {
            // update time
            state->time += deltaTime * state->speed;

            float length = state->clip->GetLength();
            if (state->time >= length)
            {
                if (state->clip->GetWrapMode() == AnimationWrapMode::Loop)
                {
                    state->time = fmod(state->time, length);
                }
                else if (state->clip->GetWrapMode() == AnimationWrapMode::Sustain)
                {
                    state->time = length;
                }
                else //if (state->clip->GetWrapMode() == AnimationWrapMode::Once)
                {
                    StopAnimation(*state);
                    continue; // clip stopped
                }
            }

            // update weight
            if (state->weightFadeRate > 0)
            {
                if (state->weight < state->targetWeight)
                {
                    state->weight += state->weightFadeRate * deltaTime;

                    if (state->weight >= state->targetWeight)
                    {
                        state->weight = state->targetWeight;
                        state->weightFadeRate = 0;
                    }
                }
                else //if (state->weight >= state->targetWeight)
                {
                    state->weight -= state->weightFadeRate * deltaTime;

                    if (state->weight <= state->targetWeight)
                    {
                        state->weight = state->targetWeight;
                        state->weightFadeRate = 0;

                        if(state->fadingOut)
                        {
                            StopAnimation(*state);
                        }
                    }
                }
            }
        }
    }
}

void Animator::Sample()
{
    sptr<Node> rootNode = GetNode();

    UpdateInfluences();

    // reset influences
    for (auto& [path, inf] : influences) {
        inf = {};
    }

    // accumulate influences
    for (auto& [name, state] : animationStates)
    {
        if (state->enabled && state->clip)
        {
            for(auto& [path, track] : state->clip->tracks)
            {
                auto& inf = influences[path];

                float time = state->time;
                Transform localTransform = track->Evaluate(time);
                
                auto localRot = localTransform.rotation;
                if(inf.weight > 0 && localRot.Dot(inf.localRot) < 0)
                    localRot = -localRot;

                inf.localPos += localTransform.position * state->weight;
                inf.localRot += localRot * state->weight;
                inf.localScale += localTransform.scale * state->weight;
                inf.weight += state->weight;
            }
        }
    }

    // apply influences
    for(auto& [path, inf] : influences)
    {
        if (inf.weight >= FLT_EPSILON)
        {
            inf.localPos /= inf.weight;
            //inf.localRot /= inf.weight;
            inf.localRot.Normalize();
            inf.localScale /= inf.weight;

            sptr<Node> node = rootNode->GetChild(path);
            if (node)
                node->SetLocalTransform(inf.localPos, inf.localRot, inf.localScale);
        }
    }
}

AnimationState& Animator::GetAnimationState(std::string_view clipName)
{
    for (auto& [name, state] : animationStates)
    {
        if (state->clip->GetName() == clipName)
            return *state;
    }

    throw std::runtime_error("Animator::operator[] -> clip not found");
}

AnimationState& Animator::operator[](std::string_view clipName) {
    return GetAnimationState(clipName);
}

void Animator::Stop()
{
    for (auto& [name, state] : animationStates) {
        StopAnimation(*state);
    }
}

void Animator::Stop(std::string_view clipName)
{
    auto& state = GetAnimationState(clipName);
    StopAnimation(state);
}

void Animator::Play(std::string_view clipName)
{
    Stop();
    auto& state = GetAnimationState(clipName);
    state.weight = 1.0f;
    state.enabled = true;
}

void Animator::Blend(std::string_view clipName, float targetWeight, float fadeLength)
{
    auto& state = GetAnimationState(clipName);
    state.enabled = true;
    state.fadingOut = false;
    state.targetWeight = targetWeight;

    if (abs(state.targetWeight - state.weight) >= FLT_EPSILON)
    {
        state.weightFadeRate = 1.0f / fadeLength;
    }
    else
    {
        state.weight = state.targetWeight;
        state.weightFadeRate = 0;
    }
}

void Animator::CrossFade(std::string_view clipName, float fadeLength)
{
    for (auto& [name, state] : animationStates)
    {
        if(state->clip->GetName() == clipName)
        {
            state->enabled = true;
            state->fadingOut = false;
            state->targetWeight = 1.0f;

            if(abs(state->targetWeight - state->weight) >= FLT_EPSILON)
            {
                state->weightFadeRate = 1.0f / fadeLength;
            }
            else
            {
                state->weight = state->targetWeight;
                state->weightFadeRate = 0;
            }
        }
        else if(state->enabled)
        {
            state->targetWeight = 0.0f;

            if (abs(state->targetWeight - state->weight) >= FLT_EPSILON)
            {
                state->weightFadeRate = 1.0f / fadeLength;
                state->fadingOut = true;
            }
            else
            {
                StopAnimation(*state);
            }
        }
    }
}

bool Animator::IsPlaying() const
{
    using std::ranges::any_of;

    return any_of(
        animationStates,
        [](auto& kv) { return kv.second->enabled; }
    );
}

void Animator::SystemUpdate1()
{
    UpdateAnimationStates();
    Sample();
}

}
}
