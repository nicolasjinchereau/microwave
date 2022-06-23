/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Test.ExitDoorTrigger;
import Microwave;
import <vector>;

using namespace mw;

export namespace Test {

class BigDoors;
class Game;

export class ExitDoorTrigger : public Script
                             , public IUserEvents
                             , public ICollisionEvents
{
public:
    gptr<BigDoors> exitDoor;
    gptr<Game> game;

    virtual void Start() override;
    virtual void OnCollisionStart(const Collision& collision) override;
    virtual void OnCollisionStop(const Collision& collision) override;
};

} // Test
