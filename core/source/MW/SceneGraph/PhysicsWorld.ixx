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
class D6Joint;
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
    gvector<gptr<RigidBody>> bodies;

    void PerformCollisionCallbacks(btPersistentManifold* manifold, CollisionState& state);

    friend Bullet;
    friend D6Joint;
    friend RigidBody;
public:
    PhysicsWorld();
    ~PhysicsWorld();

    Vec3 GetGravity() const;
    void SetGravity(const Vec3& gravity);

    void StepSimulation(float deltaTime);

    int GetBodyCount() const { return (int)bodies.size(); }
private:
    void AddRigidBody(const gptr<RigidBody>& body);
    void RemoveRigidBody(const gptr<RigidBody>& body);
};

} // scene
} // mw
