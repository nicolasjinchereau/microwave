/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Object.h>
#include <MW/System/Pointers.h>
#include <MW/Math/Math.h>
#include <unordered_map>
#include <vector>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
struct btDbvtBroadphase;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btPersistentManifold;

namespace mw {
inline namespace scene {

enum class CollisionState
{
    Start,
    Update,
    Stop,
    Cancel
};

class Bullet;
class D6Joint;
class RigidBody;

class PhysicsWorld : public Object
{
    REGISTER_OBJECT(PhysicsWorld);

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
