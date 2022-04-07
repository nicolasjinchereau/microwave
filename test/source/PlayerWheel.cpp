/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include "PlayerWheel.h"
#include "Player.h"

void PlayerWheel::OnCollisionStart(const Collision& collision)
{
    if(auto p = player.lock())
        p->UpdateGroundedState(collision);
}

void PlayerWheel::OnCollisionUpdate(const Collision& collision)
{
    if (auto p = player.lock())
        p->UpdateGroundedState(collision);
}
