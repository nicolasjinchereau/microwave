/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Test.Player;
import Microwave;
import Test.PlayerWheel;

using namespace mw;

export namespace Test {

class Player : public Script
             , public IUserEvents
             , public ISceneInputEvents
             , public ICollisionEvents
{
public:
    bool aDown = false;
    bool sDown = false;
    bool dDown = false;
    bool wDown = false;

    int pointerID = -1;
    IVec2 lastPointerPos;

    sptr<AudioSource> jumpSound;
    sptr<AudioSource> landSound;
    sptr<RigidBody> body;
    sptr<Node> wheelAxle;
    sptr<Node> wheel;
    sptr<D6Joint> wheelJoint;
    sptr<RigidBody> wheelBody;
    sptr<SphereCollider> wheelCollider;
    sptr<Animator> anim;
    sptr<AudioSource> shutdownSound;

    const float BatteryLife = 60.0f * 2;

    bool alive = true;
    float batteryLevel = 1.0f;
    bool drainBattery = true;

    bool grounded = true;
    bool groundedLastUpdate = true;
    bool jumpPressed = false;
    float lastGroundedTime = std::numeric_limits<float>::lowest();

    constexpr static float MaxMoveSpeed = 4.0f;
    constexpr static float MaxAirSpeed = 4.0f;
    constexpr static float MaxJumpSlope = 45.0f;
    constexpr static float AngularVelocityFactor = 18.15f;
    constexpr static float JumpImpulse = 15.9f;
    constexpr static float JumpTimeAllowance = 0.1f;
    constexpr static float MinAirtimeBeforeLandingSound = 0.2f;

    Task<void> InitAsync()
    {
        auto assetLibrary = App::Get()->GetAssetLibrary();

        auto playerModelAsset = co_await assetLibrary->GetAssetAsync<Node>("Models/Player.fbx");
        auto jumpClip = co_await assetLibrary->GetAssetAsync<AudioClip>("Audio/Sounds/Jump.wav");
        auto landClip = co_await assetLibrary->GetAssetAsync<AudioClip>("Audio/Sounds/Land.wav");
        auto shutdownClip = co_await assetLibrary->GetAssetAsync<AudioClip>("Audio/Sounds/Shutdown.wav");

        auto node = GetNode();
        node->SetName("Player");
        
        auto playerModel = Instantiate<Node>(playerModelAsset);
        node->AddChild(playerModel);
        playerModel->SetLocalPosition(Vec3::Zero());
        playerModel->SetLocalRotation(Quat(0, 180.0f, 0));

        auto colliderPivot = spnew<Node>();
        node->AddChild(colliderPivot);
        colliderPivot->SetLocalPosition({ 0, 2.94f / 2 + 0.82f + 0.05f, 0 });
        node->SetLocalScale(Vec3(0.27f, 0.27f, 0.27f));
        
        auto collider = colliderPivot->AddComponent<CapsuleCollider>();
        collider->SetPivot(colliderPivot);
        collider->SetRadius(0.82f);
        collider->SetHeight(2.94f - 0.1f);

        body = node->AddComponent<RigidBody>();
        body->SetAngularFactor({ 0, 0, 0 });
        body->SetMass(1.0f);
        
        wheelAxle = node->GetChild("RootNode/WheelAxle");

        wheel = spnew<Node>();
        wheel->SetName("Wheel");
        wheel->SetPosition(wheelAxle->GetPosition());
        wheel->SetRotation(wheelAxle->GetRotation());
        wheel->SetScale({ 0.27f, 0.27f, 0.27f });
        node->GetScene()->GetRootNode()->AddChild(wheel);
        
        wheelCollider = wheel->AddComponent<SphereCollider>();
        wheelCollider->SetRadius(0.82f);
        wheelBody = wheel->AddComponent<RigidBody>();
        wheelBody->SetMass(0.5f);
        wheelBody->SetFriction(3.0f);
        wheelJoint = wheel->AddComponent<D6Joint>();
        wheelJoint->SetLinkBody(body);
        wheelJoint->SetLinkPivotOffset({ 0, 0.8f * 0.27f, 0 });
        wheelJoint->SetAngularJointMotion(JointMotion::Free);
        wheelJoint->SetAngularTargetVelocity({ AngularVelocityFactor, 0, 0 });

        auto wheelScript = wheel->AddComponent<PlayerWheel>();
        wheelScript->player = This<Player>();

        anim = GetNode()->FindComponentDownward<Animator>();

        jumpSound = node->AddComponent<AudioSource>();
        jumpSound->SetClip(jumpClip);

        landSound = node->AddComponent<AudioSource>();
        landSound->SetClip(landClip);

        shutdownSound = GetNode()->AddComponent<AudioSource>();
        shutdownSound->SetClip(shutdownClip);
    }

    virtual void OnDisable() override
    {
        wheelJoint->SetAngularMotorEnabled(false);
        body->SetVelocity(Vec3::Zero());
        body->SetAngularVelocity(Vec3::Zero());
        wheelBody->SetVelocity(Vec3::Zero());
        wheelBody->SetAngularVelocity(Vec3::Zero());
        CancelInput();
    }

    void Reset()
    {
        wheelJoint->SetAngularMotorEnabled(false);
        body->SetVelocity(Vec3::Zero());
        body->SetAngularVelocity(Vec3::Zero());
        body->UpdateTransform(true);
        wheel->SetPosition(wheelAxle->GetPosition());
        wheel->SetRotation(wheelAxle->GetRotation());
        wheelBody->SetVelocity(Vec3::Zero());
        wheelBody->SetAngularVelocity(Vec3::Zero());
        wheelBody->UpdateTransform(true);
        anim->CrossFade("Idle");
        batteryLevel = 1.0f;
        CancelInput();
        SetEnabled(true);
        alive = true;
    }

    void Shutdown()
    {
        if (alive)
        {
            shutdownSound->Play();
            SetEnabled(false);
            alive = false;
            CancelInput();
            wheelJoint->SetAngularMotorEnabled(false);
            body->SetVelocity(Vec3::Zero());
            body->SetAngularVelocity(Vec3::Zero());
            wheelBody->SetVelocity(Vec3::Zero());
            wheelBody->SetAngularVelocity(Vec3::Zero());
            anim->CrossFade("Shutdown");
        }
    }

    void CancelInput()
    {
        aDown = false;
        sDown = false;
        dDown = false;
        wDown = false;
    }

    virtual void Update() override
    {
        if(!alive)
            return;

        UpdateMovement();
        UpdateBattery();
    }

    void UpdateBattery()
    {
        if (drainBattery)
        {
            float deltaTime = GetClock()->GetDeltaTime();
            float drainAmount = (1.0f / BatteryLife) * deltaTime;
            batteryLevel = math::Clamp01(batteryLevel - drainAmount);
        }

        if (alive && batteryLevel < 10e-7f)
        {
            Shutdown();
        }
    }

    void UpdateMovement()
    {
        auto node = GetNode();
        
        Vec3 dir = mw::Vec3::Zero();
        if(pointerID == -1)
        {
            if (aDown) dir += Vec3::Left();
            if (dDown) dir += Vec3::Right();
            if (wDown) dir += Vec3::Forward();
            if (sDown) dir += Vec3::Backward();
            dir.Normalize();
        }

        bool hasDirectionInput = (dir.Length() > 10e-6f);

        if (grounded)
        {
            if (hasDirectionInput)
                anim->CrossFade("Moving");
            else
                anim->CrossFade("Idle");
        }

        // update player body direction
        if(hasDirectionInput)
        {
            float angle = Vec3::Angle(dir, body->GetNode()->Forward());
            if (angle > 1.0f)
            {
                auto rot = Quat::LookRotation(dir, Vec3::Up());
                body->GetNode()->SetRotation(rot);
            }

            Quat wheelRot = wheelBody->GetNode()->GetRotation();
            Vec3 axleTarget = Vec3::Up().Cross(dir);
            Vec3 axleActual = Vec3::Right() * wheelRot;
            float axleOffset = Vec3::Angle(axleTarget, axleActual);

            if(axleOffset > 3.0f) {
                wheelRot *= Quat::FromTo(axleActual, axleTarget);
                wheelBody->GetNode()->SetRotation(wheelRot);
            }
        }

        // update wheel angular motor
        if (grounded && hasDirectionInput)
        {
            wheelJoint->SetAngularMotorEnabled(true);
            wheelBody->SetAngularFactor(Vec3::One());
        }
        else
        {
            wheelJoint->SetAngularMotorEnabled(false);
            wheelBody->SetAngularVelocity(Vec3::Zero());
            wheelBody->SetAngularFactor(Vec3::Zero());
        }

        // add airborn propulsion
        if (!grounded)
        {
            Vec3 vel = body->GetVelocity();

            if (hasDirectionInput)
            {
                auto inputSpeed = dir.Length() * MaxAirSpeed;
                auto projectedSpeed = std::max(0.0f, Vec3(vel.x, 0, vel.z).Dot(dir.Normalized()));
                auto targetSpeed = std::max(inputSpeed, projectedSpeed);
                auto targetVelocity = dir.Normalized() * targetSpeed;
                vel = Vec3(targetVelocity.x, vel.y, targetVelocity.z);
            }

            float linearDampening = 0.5f;
            float damp = math::Clamp01(1.0f - linearDampening * GetClock()->GetDeltaTime());
            vel = Vec3(vel.x * damp, vel.y, vel.z * damp);

            body->SetVelocity(vel);
        }

        auto time = GetClock()->GetTime();

        // do landing sound
        if (grounded && !groundedLastUpdate &&
            (time - lastGroundedTime) >= MinAirtimeBeforeLandingSound)
        {
            landSound->Play();
        }

        // do jump
        if (grounded)
            lastGroundedTime = GetClock()->GetTime();

        if (jumpPressed && (time - lastGroundedTime) <= JumpTimeAllowance)
        {
            jumpSound->Play();
            (*anim)["Jump"].time = 0.3f;
            anim->CrossFade("Jump");
            lastGroundedTime = std::numeric_limits<float>::lowest();
            body->AddImpulse(Vec3::Up() * JumpImpulse);
        }

        groundedLastUpdate = grounded;
        grounded = false;
        jumpPressed = false;
    }

    void UpdateGroundedState(const Collision& collision)
    {
        if(!alive)
            return;

        if (!grounded)
        {
            auto vel = body->GetVelocity();
            if(vel.y < 2.0f)
            {
                for(auto& contact : collision.contacts)
                {
                    auto floorSlope = Vec3::Angle(contact.normal, Vec3::Up());
                    if (floorSlope < MaxJumpSlope)
                    {
                        grounded = true;
                        break;
                    }
                }
            }
        }
    }

    virtual void LateUpdate() override
    {
    }

    virtual void OnKeyDown(mw::Keycode key) override
    {
        switch (key)
        {
        case mw::Keycode::A:
            aDown = true;
            break;

        case mw::Keycode::S:
            sDown = true;
            break;

        case mw::Keycode::D:
            dDown = true;
            break;

        case mw::Keycode::W:
            wDown = true;
            break;

        case mw::Keycode::Space:
            jumpPressed = true;
            break;

        case mw::Keycode::X:
            Console::WriteLine("Speed: %", body->GetVelocity().Length());
            break;
        }
    }

    virtual void OnKeyUp(mw::Keycode key) override
    {
        switch (key)
        {
        case mw::Keycode::A:
            aDown = false;
            break;

        case mw::Keycode::S:
            sDown = false;
            break;

        case mw::Keycode::D:
            dDown = false;
            break;

        case mw::Keycode::W:
            wDown = false;
            break;
        }
    }

    virtual void OnPointerDown(mw::IVec2 pos, int id) override
    {
        if (pointerID == -1)
            pointerID = id;
    }

    virtual void OnPointerUp(mw::IVec2 pos, int id) override
    {
        if (pointerID == id)
            pointerID = -1;
    }
};

} // Test
