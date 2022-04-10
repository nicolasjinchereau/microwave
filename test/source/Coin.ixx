/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Test.Coin;
import Microwave;

using namespace mw;

export namespace Test {

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

} // Test
