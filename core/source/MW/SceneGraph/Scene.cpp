/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Scene;
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
    physicsWorld = gpnew<PhysicsWorld>();
    clock = gpnew<Clock>();
}

void Scene::Destruct() {
    physicsWorld.reset();
}

gptr<Node> Scene::GetRootNode()
{
    if (!_rootNode) {
        _rootNode = gpnew<Node>();
        _rootNode->SetName("SceneRootNode");
        _rootNode->SetScene(self(this));
    }

    return _rootNode;
}

gptr<PhysicsWorld> Scene::GetPhysics() {
    return physicsWorld;
};

gptr<const PhysicsWorld> Scene::GetPhysics() const {
    return physicsWorld;
};

gptr<Clock> Scene::GetClock() {
    return clock;
}

gptr<const Clock> Scene::GetClock() const {
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
    gvector<gptr<T>>& targets,
    gvector<gptr<void>>& updateCache,
    gvector<gptr<T>>* updatedTargets,
    Fun fun, Args&&... args)
{
    gvector<gptr<T>>& updateCacheT = (gvector<gptr<T>>&)updateCache;
    updateCacheT = targets;
    if (updatedTargets) targets.clear();

    for (auto& obj : updateCacheT)
    {
        auto comp = dynamic_cast<Component*>(obj.get());
        if (comp->IsActiveAndEnabled())
        {
            ((*obj).*fun)(std::forward<Args>(args)...);

            if (updatedTargets)// && obj.use_count() > 1)
                updatedTargets->push_back(obj);
        }
        else
        {
            if (updatedTargets)// && obj.use_count() > 1)
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

void Scene::RegisterComponent(const gptr<Component>& comp)
{
    if (auto p = gpcast<Camera>(comp))
        cameras.push_back(p);

    if (auto p = gpcast<Canvas>(comp))
        canvases.push_back(p);

    if (auto p = gpcast<ISceneInputEvents>(comp))
        sceneInputHandlers.push_back(p);

    if (auto p = gpcast<IUserEvents>(comp))
        userStarts.push_back(p);

    if (auto p = gpcast<ISystemEvents>(comp))
        systemStarts.push_back(p);

    if (auto p = gpcast<Script>(comp))
        scripts.push_back(p);

    if (auto p = gpcast<DirectionalLight>(comp))
        lights.push_back(p);

    if (auto p = gpcast<IRenderEvents>(comp))
        renderEvents.push_back(p);
}

void Scene::UnregisterComponent(const gptr<Component>& comp)
{
    if (auto p = gpcast<Camera>(comp))
        std::erase(cameras, p);

    if (auto p = gpcast<Canvas>(comp))
        std::erase(canvases, p);

    if (auto p = gpcast<ISceneInputEvents>(comp))
        std::erase(sceneInputHandlers, p);

    if (auto p = gpcast<IUserEvents>(comp)) {
        std::erase(userUpdates, p);
        std::erase(userStarts, p);
    }

    if (auto p = gpcast<ISystemEvents>(comp)) {
        std::erase(systemUpdates, p);
        std::erase(systemStarts, p);
    }

    if (auto p = gpcast<Script>(comp))
        std::erase(scripts, p);

    if (auto p = gpcast<DirectionalLight>(comp))
        std::erase(lights, p);

    if (auto p = gpcast<IRenderEvents>(comp))
        std::erase(renderEvents, p);
}

}
}
