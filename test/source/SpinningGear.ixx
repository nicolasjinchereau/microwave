/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Test.SpinningGear;
import Microwave;
import <string>;

using namespace mw;

export namespace Test {

class SpinningGear : public Script
                   , public IUserEvents
{
    gptr<RigidBody> body;
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

} // Test
