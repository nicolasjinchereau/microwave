/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Test.PlayerWheel;
import Microwave;

using namespace mw;

export namespace Test {

class Player;

class PlayerWheel : public Script
                  , public ICollisionEvents
{
public:
    gptr<Player> player;
    virtual void OnCollisionStart(const Collision& collision) override;
    virtual void OnCollisionUpdate(const Collision& collision) override;
};

} // Test
