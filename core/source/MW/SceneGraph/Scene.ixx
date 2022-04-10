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
class Script;

class Scene : public Object
{
    inline static Type::Pin<Scene> pin;

    sptr<Node> _rootNode;

    std::vector<sptr<Camera>> cameras;
    std::vector<sptr<Canvas>> canvases;
    std::vector<sptr<ISceneInputEvents>> sceneInputHandlers;
    std::vector<sptr<IUserEvents>> userStarts;
    std::vector<sptr<ISystemEvents>> systemStarts;
    std::vector<sptr<IUserEvents>> userUpdates;
    std::vector<sptr<ISystemEvents>> systemUpdates;
    std::vector<sptr<Script>> scripts;
    std::vector<sptr<DirectionalLight>> lights;
    std::vector<sptr<IRenderEvents>> renderEvents;

    std::vector<sptr<void>> updateCache;
    sptr<PhysicsWorld> physicsWorld;
    sptr<Clock> clock;

    Color ambientColor = Color(0.2f, 0.2f, 0.2f, 1.0f);
    bool gizmosEnabled = false;
    bool updateEnabled = true;

    friend class Node;
    friend class RigidBody;
    friend class SceneRenderer;

    void Construct();
    void Destruct();
public:
    Scene(){ Construct(); }
    ~Scene(){ Destruct(); }

    sptr<Node> GetRootNode();

    sptr<PhysicsWorld> GetPhysics();
    sptr<const PhysicsWorld> GetPhysics() const;

    sptr<Clock> GetClock();
    sptr<const Clock> GetClock() const;

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
    void RegisterComponent(const sptr<Component>& comp);
    void UnregisterComponent(const sptr<Component>& comp);
};

}
}
