/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Scene;
import Microwave.Graphics.Color;
import Microwave.SceneGraph.Events;
import Microwave.System.Clock;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.System.Window;
import <vector>;

export namespace mw {
inline namespace scene {

class Camera;
class Canvas;
class Component;
class DirectionalLight;
class Node;
class PhysicsWorld;
class RigidBody;
class SceneRenderer;
class Script;

class Scene : public Object
{
    inline static Type::Pin<Scene> pin;
public:
    gptr<Node> _rootNode;

    gvector<gptr<Camera>> cameras;
    gvector<gptr<Canvas>> canvases;
    gvector<gptr<ISceneInputEvents>> sceneInputHandlers;
    gvector<gptr<IUserEvents>> userStarts;
    gvector<gptr<ISystemEvents>> systemStarts;
    gvector<gptr<IUserEvents>> userUpdates;
    gvector<gptr<ISystemEvents>> systemUpdates;
    gvector<gptr<Script>> scripts;
    gvector<gptr<DirectionalLight>> lights;
    gvector<gptr<IRenderEvents>> renderEvents;

    gvector<gptr<void>> updateCache;
    gptr<PhysicsWorld> physicsWorld;
    gptr<Clock> clock;

    Color ambientColor = Color(0.2f, 0.2f, 0.2f, 1.0f);
    bool gizmosEnabled = false;
    bool updateEnabled = true;

    friend Node;
    friend RigidBody;
    friend SceneRenderer;

    void Construct();
    void Destruct();
public:
    Scene(){ Construct(); }
    ~Scene(){ Destruct(); }

    gptr<Node> GetRootNode();

    gptr<PhysicsWorld> GetPhysics();
    gptr<const PhysicsWorld> GetPhysics() const;

    gptr<Clock> GetClock();
    gptr<const Clock> GetClock() const;

    void SetAmbientColor(const Color& color);
    Color GetAmbientColor() const;

    void SetGizmosEnabled(bool enabled);
    bool GetGizmosEnabled() const;

    void SetUpdateEnabled(bool enabled);
    bool IsUpdateEnabled() const;

    void SendKeyDown(Window* window, Keycode key);
    void SendKeyUp(Window* window, Keycode key);
    void SendPointerDown(Window* window, IVec2 pos, int id);
    void SendPointerMove(Window* window, IVec2 pos, int id);
    void SendPointerUp(Window* window, IVec2 pos, int id);

    void Update();

private:
    void RegisterComponent(const gptr<Component>& comp);
    void UnregisterComponent(const gptr<Component>& comp);
};

} // scene
} // mw
