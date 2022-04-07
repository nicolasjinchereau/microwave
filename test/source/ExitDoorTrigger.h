/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Events.h>
#include <MW/Scene/Components/ImageView.h>
#include <MW/Scene/Components/RigidBody.h>
#include <MW/Scene/Components/Script.h>
#include <vector>

using namespace mw;

class BigDoors;
class Game;

class ExitDoorTrigger : public Script
                      , public IUserEvents
                      , public ICollisionEvents
{
public:
    wptr<BigDoors> exitDoor;
    wptr<Game> game;

    virtual void Start() override;
    virtual void OnCollisionStart(const Collision& collision) override;
    virtual void OnCollisionStop(const Collision& collision) override;
};
