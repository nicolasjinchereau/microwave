/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Events.h>
#include <MW/Scene/Components/RigidBody.h>
#include <MW/Scene/Components/Script.h>

using namespace mw;

class Player;

class PlayerWheel : public Script
                  , public ICollisionEvents
{
public:
    wptr<Player> player;
    virtual void OnCollisionStart(const Collision& collision) override;
    virtual void OnCollisionUpdate(const Collision& collision) override;
};
