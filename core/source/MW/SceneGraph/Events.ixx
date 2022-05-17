/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Events;
import Microwave.Graphics.Material;
import Microwave.Math;
import Microwave.SceneGraph.Renderable;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.System.Window;
import Microwave.Utilities.Sink;
import <cstdint>;
import <span>;
import <vector>;

export namespace mw {
inline namespace scene {

class ICanvasInputEvents
{
public:
    virtual ~ICanvasInputEvents(){}
    virtual void OnKeyDown(Keycode key) {}
    virtual void OnKeyUp(Keycode key) {}
    virtual void OnPointerDown(Vec2 canvasPos, int id) {}
    virtual void OnPointerMove(Vec2 canvasPos, int id) {}
    virtual void OnPointerUp(Vec2 canvasPos, int id) {}
};

class ISceneInputEvents
{
public:
    virtual ~ISceneInputEvents(){}
    virtual void OnKeyDown(Keycode key) {}
    virtual void OnKeyUp(Keycode key) {}
    virtual void OnPointerDown(IVec2 windowPos, int id) {}
    virtual void OnPointerMove(IVec2 windowPos, int id) {}
    virtual void OnPointerUp(IVec2 windowPos, int id) {}
};

class IUserEvents
{
public:
    virtual ~IUserEvents(){}
    virtual void Start() {}
    virtual void Update() {}
    virtual void LateUpdate() {}
};

class ISystemEvents
{
public:
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

class RigidBody;

struct Collision
{
    sptr<RigidBody> body;
    std::span<ContactPoint> contacts;
};

class ICollisionEvents
{
public:
    virtual ~ICollisionEvents(){}
    virtual void OnCollisionStart(const Collision& collision) {}
    virtual void OnCollisionUpdate(const Collision& collision) {}
    virtual void OnCollisionStop(const Collision& collision) {}
};

class IRenderEvents
{
public:
    virtual ~IRenderEvents(){}
    virtual void GetRenderables(Sink<sptr<Renderable>> sink) = 0;
};

} // scene
} // mw
