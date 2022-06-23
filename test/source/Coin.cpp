/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Test.Coin;
import Test.Game;
import Microwave;

using namespace mw;

namespace Test {

void Coin::Start()
{
    auto collider = GetNode()->AddComponent<SphereCollider>();
    collider->SetRadius(0.4f);
    auto rb = GetNode()->AddComponent<RigidBody>();
    rb->SetBodyType(BodyType::Ghost);
}

void Coin::Update()
{
    float time = GetClock()->GetTime();
    GetNode()->SetRotation(Quat(0, rotationSpeed * time, 0));
}

void Coin::OnCollisionStart(const Collision& collision)
{
    auto name = collision.body->GetNode()->GetName();
    if (name == "Player")
    {
        if(game)
        {
            game->OnCoinCollected(this);
        }
    }
}

}
