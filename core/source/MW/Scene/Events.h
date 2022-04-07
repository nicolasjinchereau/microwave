/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Material.h>
#include <MW/Math/Math.h>
#include <MW/Scene/Renderable.h>
#include <MW/System/Window.h>
#include <MW/System/Pointers.h>
#include <MW/Utilities/Sink.h>
#include <vector>

namespace mw {
inline namespace scene {

class Camera;
class Node;
class Scene;
class RigidBody;

struct EventBit
{
    constexpr static uint32_t Camera      = 1 << 0;
    constexpr static uint32_t CanvasInput = 1 << 1;
    constexpr static uint32_t SceneInput  = 1 << 2;
    constexpr static uint32_t User        = 1 << 3;
    constexpr static uint32_t System      = 1 << 4;
    constexpr static uint32_t Script      = 1 << 5;
    constexpr static uint32_t Collision   = 1 << 6;
    constexpr static uint32_t Light       = 1 << 7;
    constexpr static uint32_t Rendering   = 1 << 8;
};

class EventTarget
{
protected:
    uint32_t eventBits = {};
    bool enabled = true;
    wptr<Node> node;

    void SetEventBit(uint32_t bit) { eventBits |= bit; }
    bool IsNodeBranchActive() const;
public:
    virtual ~EventTarget() = default;
    bool HasEventBit(uint32_t bit) const;

    sptr<const Node> GetNode() const;
    sptr<Node> GetNode();

    sptr<const Scene> GetScene() const;
    sptr<Scene> GetScene();

    bool IsEnabled() const;
    void SetEnabled(bool enable);

    bool IsActiveAndEnabled() const;

    virtual void OnEnable(){}
    virtual void OnDisable(){}
};

class ICanvasInputEvents : public virtual EventTarget
{
public:
    ICanvasInputEvents() { SetEventBit(EventBit::CanvasInput); }

    virtual ~ICanvasInputEvents() = default;
    virtual void OnKeyDown(Keycode key) {}
    virtual void OnKeyUp(Keycode key) {}
    virtual void OnPointerDown(Vec2 canvasPos, int id) {}
    virtual void OnPointerMove(Vec2 canvasPos, int id) {}
    virtual void OnPointerUp(Vec2 canvasPos, int id) {}
};

class ISceneInputEvents : public virtual EventTarget
{
public:
    ISceneInputEvents() { SetEventBit(EventBit::SceneInput); }

    virtual ~ISceneInputEvents() = default;
    virtual void OnKeyDown(Keycode key) {}
    virtual void OnKeyUp(Keycode key) {}
    virtual void OnPointerDown(IVec2 windowPos, int id) {}
    virtual void OnPointerMove(IVec2 windowPos, int id) {}
    virtual void OnPointerUp(IVec2 windowPos, int id) {}
};

class IUserEvents : public virtual EventTarget
{
public:
    IUserEvents() { SetEventBit(EventBit::User); }

    virtual ~IUserEvents() = default;
    virtual void Start() {}
    virtual void Update() {}
    virtual void LateUpdate() {}
};

class ISystemEvents : public virtual EventTarget
{
public:
    ISystemEvents() { SetEventBit(EventBit::System); }

    virtual ~ISystemEvents() = default;
    virtual void SystemStart() {}
    virtual void SystemUpdate1() {}
    virtual void SystemUpdate2() {}
    virtual void SystemLateUpdate() {}
};

struct ContactPoint
{
    Vec3 point;
    Vec3 normal;
    float distance{};
};

struct Collision
{
    sptr<RigidBody> body;
    std::span<ContactPoint> contacts;
};

class ICollisionEvents : public virtual EventTarget
{
public:
    ICollisionEvents() { SetEventBit(EventBit::Collision); }

    virtual ~ICollisionEvents() = default;
    virtual void OnCollisionStart(const Collision& collision) {}
    virtual void OnCollisionUpdate(const Collision& collision) {}
    virtual void OnCollisionStop(const Collision& collision) {}
};

class IRenderEvents : public virtual EventTarget
{
public:
    IRenderEvents() { SetEventBit(EventBit::Rendering); }

    virtual ~IRenderEvents() = default;
    virtual void GetRenderables(Sink<sptr<Renderable>> sink) = 0;
};

} // scene
} // mw
