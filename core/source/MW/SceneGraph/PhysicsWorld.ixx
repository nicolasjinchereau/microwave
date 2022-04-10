/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.PhysicsWorld;
import Microwave.Math;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <unordered_map>;
import <vector>;

export {
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
struct btDbvtBroadphase;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btPersistentManifold;
}

export namespace mw {
inline namespace scene {

class Bullet;
class RigidBody;

enum class CollisionState
{
    Start,
    Update,
    Stop,
    Cancel
};

class PhysicsWorld : public Object
{
    uptr<btDefaultCollisionConfiguration> config;
    uptr<btCollisionDispatcher> dispatcher;
    uptr<btDbvtBroadphase> broadphase;
    uptr<btSequentialImpulseConstraintSolver> solver;
    uptr<btDiscreteDynamicsWorld> world;
    std::unordered_map<btPersistentManifold*, CollisionState> collisions;
    std::vector<sptr<RigidBody>> bodies;

    void PerformCollisionCallbacks(btPersistentManifold* manifold, CollisionState& state);

    friend class Bullet;
    friend class RigidBody;
    friend class D6Joint;
public:
    PhysicsWorld();
    ~PhysicsWorld();

    Vec3 GetGravity() const;
    void SetGravity(const Vec3& gravity);

    void StepSimulation(float deltaTime);

private:
    void AddRigidBody(const sptr<RigidBody>& body);
    void RemoveRigidBody(const sptr<RigidBody>& body);
};

} // scene
} // mw
