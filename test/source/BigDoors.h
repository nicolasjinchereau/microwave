/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Components/ImageView.h>
#include <MW/Scene/Components/Script.h>
#include <MW/Scene/Components/Animator.h>
#include <MW/Scene/Components/AudioSource.h>
#include <MW/Scene/Events.h>
#include <MW/System/App.h>
#include <vector>

using namespace mw;

class BigDoors : public Script
               , public IUserEvents
{
public:
    sptr<Animator> anim;
    sptr<AudioSource> doorSound;

    virtual void Start() override
    {
        anim = GetNode()->GetComponent<Animator>();
        anim->CrossFade("OpenDoors");

        auto assetLib = App::Get()->GetAssetLibrary();
        doorSound = GetNode()->AddComponent<AudioSource>();
        doorSound->SetClip(assetLib->GetAsset<AudioClip>("Audio/Sounds/DoorsClosing.wav"));
    }

    void OpenDoors() {
        doorSound->Play();
        anim->CrossFade("OpenDoors");
    }

    void CloseDoors() {
        doorSound->Play();
        anim->CrossFade("CloseDoors");
    }
};
