/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Events;
import Microwave.Graphics.Material;
import Microwave.Math;
import Microwave.SceneGraph.Renderable;
import Microwave.System.Window;
import Microwave.System.Pointers;
import Microwave.Utilities.Sink;
import <cstdint>;
import <span>;
import <vector>;

export namespace mw {
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
    uint32_t eventBits = 0;
    bool enabled = true;
    wptr<Node> node;

    void SetEventBit(uint32_t bit) { eventBits |= bit; }
    bool IsNodeBranchActive() const;
public:
    EventTarget(){}
    virtual ~EventTarget(){}

    bool HasEventBit(uint32_t bit) const;

    // do not call this if you are not a Node.
    void SetNode(const wptr<Node>& node);

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

    virtual ~ICanvasInputEvents(){}
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

    virtual ~ISceneInputEvents(){}
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

    virtual ~IUserEvents(){}
    virtual void Start() {}
    virtual void Update() {}
    virtual void LateUpdate() {}
};

class ISystemEvents : public virtual EventTarget
{
public:
    ISystemEvents() { SetEventBit(EventBit::System); }

    virtual ~ISystemEvents(){}
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

    virtual ~ICollisionEvents(){}
    virtual void OnCollisionStart(const Collision& collision) {}
    virtual void OnCollisionUpdate(const Collision& collision) {}
    virtual void OnCollisionStop(const Collision& collision) {}
};

class IRenderEvents : public virtual EventTarget
{
public:
    IRenderEvents() { SetEventBit(EventBit::Rendering); }

    virtual ~IRenderEvents(){}
    virtual void GetRenderables(Sink<sptr<Renderable>> sink) = 0;
};

} // scene
} // mw
