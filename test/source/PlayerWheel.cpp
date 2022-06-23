/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Test.PlayerWheel;
import Microwave;
import Test.Player;

using namespace mw;

namespace Test {

void PlayerWheel::OnCollisionStart(const Collision& collision)
{
    if(player)
        player->UpdateGroundedState(collision);
}

void PlayerWheel::OnCollisionUpdate(const Collision& collision)
{
    if (player)
        player->UpdateGroundedState(collision);
}

} // Test
