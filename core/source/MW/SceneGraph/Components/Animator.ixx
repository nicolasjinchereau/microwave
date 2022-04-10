/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.Animator;
import Microwave.Graphics.AnimationClip;
import Microwave.Math;
import Microwave.SceneGraph.Components.Component;
import Microwave.SceneGraph.Events;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import <string>;
import <unordered_map>;
import <vector>;

export namespace mw {
inline namespace scene {

class AnimationState
{
public:
    sptr<AnimationClip> clip;
    float speed = 1.0f;

    float time = 0.0f;
    float weight = 0.0f;
    float targetWeight = 0.0f;
    float weightFadeRate = 0.0f;
    bool enabled = false;
    bool fadingOut = false;
};

struct Influence
{
    Vec3 localPos = {};
    Quat localRot = {0,0,0,0};
    Vec3 localScale = {};
    float weight = {};
};

class Animator : public Component, public ISystemEvents
{
    inline static Type::Pin<Animator> pin;

    std::unordered_map<std::string, sptr<AnimationState>> animationStates;
    std::unordered_map<path, Influence> influences;
    bool dirtyInfluences = true;

    void StopAnimation(AnimationState& state);
    void UpdateInfluences();
    void UpdateAnimationStates();
public:

    Animator(){}

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void AddClip(const sptr<AnimationClip>& clip, const std::string& clipName);
    void RemoveClip(const sptr<AnimationClip>& clip);
    void RemoveClip(const std::string& clipName);
    int GetClipCount() const;

    bool IsPlaying() const;
    void Stop();
    void Stop(std::string_view clipName);
    void Play(std::string_view clipName);
    void Blend(std::string_view clipName, float targetWeight, float fadeLength = 0.3f);
    void CrossFade(std::string_view clipName, float fadeLength = 0.3f);
    void Sample();
    AnimationState& GetAnimationState(std::string_view name);
    AnimationState& operator[](std::string_view name);

    virtual void SystemUpdate1() override;
};

}
}
