/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Test.ExitDoorTrigger;
import Test.BigDoors;
import Test.Player;
import Test.Game;
import Microwave;

using namespace mw;

namespace Test {

void ExitDoorTrigger::Start() {
    auto rb = GetNode()->GetComponent<RigidBody>();
    rb->SetBodyType(BodyType::Ghost);
}

void ExitDoorTrigger::OnCollisionStart(const Collision& collision)
{
    
}

void ExitDoorTrigger::OnCollisionStop(const Collision& collision)
{
    auto node = collision.body->GetNode();
    
    if (node->GetName() == "Player")
    {
        if (auto door = exitDoor.lock())
        {
            if(node->GetPosition().x > door->GetNode()->GetPosition().x)
            {
                if (auto g = game.lock()) {
                    g->OnPlayerExit();
                }
            }
        }
    }
}

} // Test
