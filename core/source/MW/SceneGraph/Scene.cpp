/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Scene;
import Microwave.SceneGraph.Scene;
import Microwave.SceneGraph.Components.Camera;
import Microwave.SceneGraph.Components.Canvas;
import Microwave.SceneGraph.Components.DirectionalLight;
import Microwave.SceneGraph.Components.Script;
import Microwave.SceneGraph.Node;
import Microwave.SceneGraph.PhysicsWorld;
import Microwave.System.App;
import Microwave.Utilities.Util;
import <type_traits>;
import <cstdint>;

namespace mw {
inline namespace scene {

void Scene::Construct() {
    physicsWorld = spnew<PhysicsWorld>();
    clock = spnew<Clock>();
}

void Scene::Destruct() {
    physicsWorld.reset();
}

sptr<Node> Scene::GetRootNode()
{
    if (!_rootNode) {
        _rootNode = spnew<Node>();
        _rootNode->SetScene(SharedFrom(this));
    }

    return _rootNode;
}

sptr<PhysicsWorld> Scene::GetPhysics() {
    return physicsWorld;
};

sptr<const PhysicsWorld> Scene::GetPhysics() const {
    return physicsWorld;
};

sptr<Clock> Scene::GetClock() {
    return clock;
}

sptr<const Clock> Scene::GetClock() const {
    return clock;
}

void Scene::SetAmbientColor(const Color& color) {
    ambientColor = color;
}

Color Scene::GetAmbientColor() const {
    return ambientColor;
}

void Scene::SetGizmosEnabled(bool enabled) {
    gizmosEnabled = enabled;
}

bool Scene::GetGizmosEnabled() const {
    return gizmosEnabled;
}

void Scene::SetUpdateEnabled(bool enabled)
{
    if (updateEnabled != enabled)
    {
        updateEnabled = enabled;
        if(updateEnabled)
            clock->Reset();
    }
}

bool Scene::IsUpdateEnabled() const {
    return updateEnabled;
}

template<class T, class Fun, class... Args>
inline void RunUpdates(
    std::vector<sptr<T>>& targets,
    std::vector<sptr<void>>& updateCache,
    std::vector<sptr<T>>* updatedTargets,
    Fun fun, Args&&... args)
{
    std::vector<sptr<T>>& updateCacheT = (std::vector<sptr<T>>&)updateCache;
    updateCacheT = targets;
    if (updatedTargets) targets.clear();

    for (auto& obj : updateCacheT)
    {
        if (obj->IsActiveAndEnabled())
        {
            ((*obj).*fun)(std::forward<Args>(args)...);

            if (updatedTargets && obj.use_count() > 1)
                updatedTargets->push_back(obj);
        }
        else
        {
            if (updatedTargets && obj.use_count() > 1)
                targets.push_back(obj);
        }
    }

    updateCacheT.clear();
}

void Scene::SendKeyDown(Window* window, Keycode key)
{
    if (!updateEnabled) return;
    RunUpdates(canvases, updateCache, {}, &Canvas::SendKeyDown, window, key);
    RunUpdates(sceneInputHandlers, updateCache, {}, &ISceneInputEvents::OnKeyDown, key);
}

void Scene::SendKeyUp(Window* window, Keycode key)
{
    if (!updateEnabled) return;
    RunUpdates(canvases, updateCache, {}, &Canvas::SendKeyUp, window, key);
    RunUpdates(sceneInputHandlers, updateCache, {}, &ISceneInputEvents::OnKeyUp, key);
}

void Scene::SendPointerDown(Window* window, IVec2 pos, int id)
{
    if (!updateEnabled) return;
    RunUpdates(canvases, updateCache, {}, &Canvas::SendPointerDown, window, pos, id);
    RunUpdates(sceneInputHandlers, updateCache, {}, &ISceneInputEvents::OnPointerDown, pos, id);
}

void Scene::SendPointerMove(Window* window, IVec2 pos, int id)
{
    if (!updateEnabled) return;
    RunUpdates(canvases, updateCache, {}, &Canvas::SendPointerMove, window, pos, id);
    RunUpdates(sceneInputHandlers, updateCache, {}, &ISceneInputEvents::OnPointerMove, pos, id);
}

void Scene::SendPointerUp(Window* window, IVec2 pos, int id)
{
    if (!updateEnabled) return;
    RunUpdates(canvases, updateCache, {}, &Canvas::SendPointerUp, window, pos, id);
    RunUpdates(sceneInputHandlers, updateCache, {}, &ISceneInputEvents::OnPointerUp, pos, id);
}

void Scene::Update()
{
    if (!updateEnabled) return;

    RunUpdates(userStarts, updateCache, &userUpdates, &IUserEvents::Start);
    RunUpdates(systemStarts, updateCache, &systemUpdates, &ISystemEvents::SystemStart);
    
    RunUpdates(userUpdates, updateCache, {}, &IUserEvents::Update);
    RunUpdates(scripts, updateCache, {}, &Script::UpdateCoroutines);
    RunUpdates(systemUpdates, updateCache, {}, &ISystemEvents::SystemUpdate1);
    RunUpdates(systemUpdates, updateCache, {}, &ISystemEvents::SystemUpdate2);

    physicsWorld->StepSimulation(clock->GetDeltaTime());

    RunUpdates(userUpdates, updateCache, {}, &IUserEvents::LateUpdate);
    RunUpdates(systemUpdates, updateCache, {}, &ISystemEvents::SystemLateUpdate);

    clock->Tick();
}

template<class T>
sptr<T> GetEventHandler(const sptr<Component>& comp, uint32_t eventBit)
{
    sptr<T> p;

    if (comp->HasEventBit(eventBit))
        p = spcast<T>(comp);

    return p;
}

void Scene::RegisterComponent(const sptr<Component>& comp)
{
    if (auto p = GetEventHandler<Camera>(comp, EventBit::Camera))
        cameras.push_back(p);

    if (auto p = GetEventHandler<Canvas>(comp, EventBit::CanvasInput))
        canvases.push_back(p);

    if (auto p = GetEventHandler<ISceneInputEvents>(comp, EventBit::SceneInput))
        sceneInputHandlers.push_back(p);

    if (auto p = GetEventHandler<IUserEvents>(comp, EventBit::User))
        userStarts.push_back(p);

    if (auto p = GetEventHandler<ISystemEvents>(comp, EventBit::System))
        systemStarts.push_back(p);

    if (auto p = GetEventHandler<Script>(comp, EventBit::Script))
        scripts.push_back(p);

    if (auto p = GetEventHandler<DirectionalLight>(comp, EventBit::Light))
        lights.push_back(p);

    if (auto p = GetEventHandler<IRenderEvents>(comp, EventBit::Rendering))
        renderEvents.push_back(p);
}

void Scene::UnregisterComponent(const sptr<Component>& comp)
{
    if (auto p = GetEventHandler<Camera>(comp, EventBit::Camera))
        Erase(cameras, p);

    if (auto p = GetEventHandler<Canvas>(comp, EventBit::CanvasInput))
        Erase(canvases, p);

    if (auto p = GetEventHandler<ISceneInputEvents>(comp, EventBit::SceneInput))
        Erase(sceneInputHandlers, p);

    if (auto p = GetEventHandler<IUserEvents>(comp, EventBit::User)) {
        Erase(userUpdates, p);
        Erase(userStarts, p);
    }

    if (auto p = GetEventHandler<ISystemEvents>(comp, EventBit::System)) {
        Erase(systemUpdates, p);
        Erase(systemStarts, p);
    }

    if (auto p = GetEventHandler<Script>(comp, EventBit::Script))
        Erase(scripts, p);

    if (auto p = GetEventHandler<DirectionalLight>(comp, EventBit::Light))
        Erase(lights, p);

    if (auto p = GetEventHandler<IRenderEvents>(comp, EventBit::Rendering))
        Erase(renderEvents, p);
}

}
}
