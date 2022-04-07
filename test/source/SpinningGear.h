/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Components/Script.h>
#include <MW/Scene/Components/SphereCollider.h>
#include <MW/Scene/Events.h>
#include <MW/Scene/Scene.h>

using namespace mw;

class SpinningGear : public Script
                   , public IUserEvents
{
    sptr<RigidBody> body;
    float rotationSpeed = 30.0f;
    bool reverse = false;
public:
    virtual void Start() override
    {
        auto& name = GetNode()->GetName();
        reverse = name.ends_with("CCW");
    }

    virtual void Update() override
    {
        float rotSpeed = reverse ? -rotationSpeed : rotationSpeed;

        auto rot = GetNode()->GetLocalRotation();
        rot *= Quat(0, rotSpeed * GetClock()->GetDeltaTime(), 0);
        GetNode()->SetLocalRotation(rot);
    }
};
