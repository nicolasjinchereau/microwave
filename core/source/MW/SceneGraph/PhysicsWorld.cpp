/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.PhysicsWorld;
import Microwave.SceneGraph.Components.RigidBody;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Node;
import Microwave.System.App;
import Microwave.System.Exception;
import Microwave.Utilities.Util;
import <MW/SceneGraph/Internal/Bullet.h>;
import <MW/System/Debug.h>;
import <array>;
import <span>;
import <utility>;

namespace mw {
inline namespace scene {

PhysicsWorld::PhysicsWorld()
{
    config = upnew<btDefaultCollisionConfiguration>();
    dispatcher = upnew<btCollisionDispatcher>(config.get());
    broadphase = upnew<btDbvtBroadphase>();
    solver = upnew<btSequentialImpulseConstraintSolver>();
    world = upnew<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), config.get());
    world->setSynchronizeAllMotionStates(true);
    world->setGravity(btVector3(0, 0, 0));
}

PhysicsWorld::~PhysicsWorld()
{
    int numConstraints;
    while (numConstraints = world->getNumConstraints())
    {
        auto pConstraint = world->getConstraint(numConstraints - 1);
        world->removeConstraint(pConstraint);
    }
    
    int numObjects;
    while (numObjects = world->getNumCollisionObjects())
    {
        btCollisionObjectArray& arr = world->getCollisionObjectArray();
        btCollisionObject* pObject = arr[numObjects - 1];
        world->removeCollisionObject(pObject);
    }

    world.reset();
    solver.reset();
    broadphase.reset();
    dispatcher.reset();
    config.reset();
}

Vec3 PhysicsWorld::GetGravity() const {
    btVector3 gravity = world->getGravity();
    return Vec3(gravity.getX(), gravity.getY(), gravity.getZ());
}

void PhysicsWorld::SetGravity(const Vec3& gravity) {
    world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

void PhysicsWorld::AddRigidBody(const gptr<RigidBody>& body)
{
    auto bodyWorld = body->world.lock();
    if (!bodyWorld)
    {
        bodies.push_back(body);
        world->addRigidBody(body->body.get());
        body->world = self(this);
    }
}

void PhysicsWorld::RemoveRigidBody(const gptr<RigidBody>& body)
{
    auto bodyWorld = body->world.lock();
    if (bodyWorld.get() == this)
    {
        body->world.reset();
        std::erase(bodies, body);
        
        for (auto& [manifold, state] : collisions)
        {
            if (manifold->getBody0() == body->body.get() ||
                manifold->getBody1() == body->body.get())
            {
                state = CollisionState::Cancel;
            }
        }

        world->removeRigidBody(body->body.get());
    }
}

void PhysicsWorld::StepSimulation(float deltaTime)
{
    world->stepSimulation(deltaTime, 10);

    btDispatcher* disp = world->getDispatcher();

    auto numManifolds = disp->getNumManifolds();
    for (int i = 0; i != numManifolds; ++i)
    {
        btPersistentManifold* manifold = disp->getManifoldByIndexInternal(i);
        
        auto [it, didInsert] = collisions.insert(
            std::make_pair(manifold, CollisionState::Start));

        if (!didInsert && it->second != CollisionState::Cancel)
            it->second = CollisionState::Update;
    }

    for (auto it = collisions.begin(); it != collisions.end(); )
    {
        if (it->second == CollisionState::Start)
        {
            PerformCollisionCallbacks(it->first, it->second);

            if (it->second == CollisionState::Cancel) {
                it = collisions.erase(it);
            }
            else {
                it->second = CollisionState::Stop;
                ++it;
            }
        }
        else if (it->second == CollisionState::Update)
        {
            PerformCollisionCallbacks(it->first, it->second);

            if (it->second == CollisionState::Cancel) {
                it = collisions.erase(it);
            }
            else {
                it->second = CollisionState::Stop;
                ++it;
            }
        }
        else if (it->second == CollisionState::Stop)
        {
            PerformCollisionCallbacks(it->first, it->second);
            it = collisions.erase(it);
        }
        else if (it->second == CollisionState::Cancel)
        {
            it = collisions.erase(it);
        }
    }
}

void PhysicsWorld::PerformCollisionCallbacks(btPersistentManifold* manifold, CollisionState& state)
{
    auto body0 = (RigidBody*)manifold->getBody0()->getUserPointer();
    auto body1 = (RigidBody*)manifold->getBody1()->getUserPointer();
    Assert(body0 && body1);
    
    int contacts = 0;
    std::array<ContactPoint, 8> contactPoints;

    int numContacts = manifold->getNumContacts();
    for (int c = 0; c != numContacts; ++c)
    {
        btManifoldPoint& contactPoint = manifold->getContactPoint(c);
        auto worldPosA = contactPoint.m_positionWorldOnA;
        auto worldPosB = contactPoint.m_positionWorldOnB;
        btVector3 point = worldPosA + ((worldPosB - worldPosA) *= 0.5f);
        auto normal = contactPoint.m_normalWorldOnB;
        auto distance = contactPoint.m_distance1;

        auto index0 = contactPoint.m_index0;
        auto index1 = contactPoint.m_index1;
        auto partId0 = contactPoint.m_partId0;
        auto partId1 = contactPoint.m_partId1;

        contactPoints[contacts++] = ContactPoint{
            { point.x(), point.y(), point.z() },
            { normal.x(), normal.y(), normal.z() },
            distance
        };
    }

    Collision collision = {
        body1->self(body1),
        std::span<ContactPoint>(contactPoints.data(), contacts)
    };

    for (auto& c : body0->GetNode()->GetComponents())
    {
        if (auto h = dynamic_cast<ICollisionEvents*>(c.get()))
        {
            switch (state)
            {
            case CollisionState::Start:
                h->OnCollisionStart(collision);
                break;

            case CollisionState::Update:
                h->OnCollisionUpdate(collision);
                break;

            case CollisionState::Stop:
                h->OnCollisionStop(collision);
                break;
            }

            if (state == CollisionState::Cancel)
                return;
        }
    }

    collision.body = body0->self(body0);
    
    for (auto& contact : collision.contacts)
        contact.normal = -contact.normal;

    for (auto& c : body1->GetNode()->GetComponents())
    {
        if (auto h = dynamic_cast<ICollisionEvents*>(c.get()))
        {
            switch (state)
            {
            case CollisionState::Start:
                h->OnCollisionStart(collision);
                break;

            case CollisionState::Update:
                h->OnCollisionUpdate(collision);
                break;

            case CollisionState::Stop:
                h->OnCollisionStop(collision);
                break;
            }

            if (state == CollisionState::Cancel)
                return;
        }
    }
}

} // scene
} // mw
