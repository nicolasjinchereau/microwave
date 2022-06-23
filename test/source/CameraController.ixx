/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Test.CameraController;
import Microwave;

using namespace mw;

export namespace Test {

class CameraController : public Script
                       , public ISceneInputEvents
                       , public IUserEvents
{
public:
    bool wDown = false;
    bool aDown = false;
    bool sDown = false;
    bool dDown = false;
    bool qDown = false;
    bool eDown = false;
    int pointerID = -1;
    mw::IVec2 lastPointerPos;
    float camX = 0;
    float camY = 0;
    mw::Vec3 camVelocity = mw::Vec3::Zero();
    float camAccelleration = 20.0f;
    float camMaxSpeed = 200.0f;
    float camDampening = 4.0f;

    gptr<Camera> camera;
    Vec3 offset = Vec3(0, 9.5f, -6.662f);
    float camAngle = 55.0f;
    float defaultFOV = 68.0f;
    float minX = 5.55f;
    float maxX = 68.25f;
    float zoom = 1.0f;

    gptr<Node> player;

    virtual void Start() override
    {
        camera = GetNode()->GetComponent<Camera>();
        camera->SetFOV(defaultFOV);
    }

    virtual void Update() override
    {
        if(pointerID == -1)
        {
            if (player)
            {
                float camX = math::Clamp(player->GetPosition().x, minX, maxX);
                auto pos = Vec3::Right() * camX + offset * (1.0f / zoom);
                GetNode()->SetPosition(pos);
                GetNode()->SetRotation(Quat(camAngle, 0, 0));
            }
        }
        else if(pointerID == 0)
        {
            auto node = GetNode();

            auto app = mw::App::Get();
            auto deltaTime = GetClock()->GetDeltaTime();

            mw::Vec3 dir = mw::Vec3::Zero();
            if (wDown) dir += node->Forward();
            if (sDown) dir -= node->Forward();
            if (aDown) dir -= node->Right();
            if (dDown) dir += node->Right();
            if (qDown) dir -= node->Up();
            if (eDown) dir += node->Up();
            dir.Normalize();

            camVelocity += dir * deltaTime * camAccelleration;
            camVelocity *= mw::math::Clamp(1.0f - camDampening * deltaTime, 0.0f, 1.0f);

            if (camVelocity.Length() > camMaxSpeed)
                camVelocity = camVelocity.Normalized() * camMaxSpeed;

            node->SetPosition(node->GetPosition() + camVelocity * deltaTime);
        }
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

        case mw::Keycode::Q:
            qDown = true;
            break;

        case mw::Keycode::E:
            eDown = true;
            break;

        case mw::Keycode::UpArrow:
            zoom = std::min(zoom + 0.1f, 3.0f);
            break;

        case mw::Keycode::DownArrow:
            zoom = std::max(zoom - 0.1f, 1.0f);
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

        case mw::Keycode::Q:
            qDown = false;
            break;

        case mw::Keycode::E:
            eDown = false;
            break;
        }
    }

    virtual void OnPointerDown(mw::IVec2 pos, int id) override
    {
        if (pointerID == -1)
        {
            pointerID = id;
            lastPointerPos = pos;

            camVelocity = mw::Vec3::Zero();
            auto angles = GetNode()->GetRotation().ToEulerAngles();
            camX = angles.x;
            camY = angles.y;
        }
    }

    virtual void OnPointerMove(mw::IVec2 pos, int id) override
    {
        if (pointerID == id)
        {
            mw::IVec2 delta = pos - lastPointerPos;
            lastPointerPos = pos;

            camX -= delta.y * 0.2f;
            camY += delta.x * 0.2f;

            GetNode()->SetRotation(mw::Quat(camX, camY, 0));
        }
    }

    virtual void OnPointerUp(mw::IVec2 pos, int id) override
    {
        if (pointerID == id)
            pointerID = -1;
    }
};

} // Test
