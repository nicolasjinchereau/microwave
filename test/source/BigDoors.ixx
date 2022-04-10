/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Test.BigDoors;
import Microwave;
import <vector>;

using namespace mw;

export namespace Test {

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

}
