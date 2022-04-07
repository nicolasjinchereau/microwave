/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Events.h>
#include <MW/Scene/Scene.h>
#include <MW/Scene/Components/RigidBody.h>
#include <MW/Scene/Components/SphereCollider.h>
#include <MW/Scene/Components/Script.h>

using namespace mw;

class Game;

class Coin : public Script
           , public IUserEvents
           , public ICollisionEvents
{
public:
    wptr<Game> game;
    float rotationSpeed = 180.0f;

    virtual void Start() override;
    virtual void Update() override;
    virtual void OnCollisionStart(const Collision& collision) override;
};
