/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Scene/Components/RigidBody.h>
#include <MW/Scene/Components/BoxCollider.h>
#include <MW/Scene/Components/CapsuleCollider.h>
#include <MW/Scene/Components/MeshCollider.h>
#include <MW/Scene/Components/SphereCollider.h>
#include <MW/Scene/Internal/Bullet.h>
#include <MW/Scene/Internal/CapsuleShape.h>
#include <MW/Scene/Node.h>
#include <MW/Scene/PhysicsWorld.h>
#include <MW/Scene/Scene.h>
#include <MW/Math/Math.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace mw {
inline namespace scene {

class RigidBodyMotionState : public btMotionState
{
    RigidBody* pBody;
public:
    RigidBodyMotionState(RigidBody* pBody)
        : pBody(pBody)
    {
        assert(pBody);
    }

    virtual void getWorldTransform(btTransform& worldTrans) const override
    {
        auto node = pBody->GetNode();
        assert(node);

        auto pos = node->GetPosition();
        auto rot = node->GetRotation();
        pos += (pBody->localCenterOfMass * rot);

        worldTrans.setOrigin(Bullet::FromVec3(pos));
        worldTrans.setRotation(Bullet::FromQuat(rot));
    }

    virtual void setWorldTransform(const btTransform& worldTrans) override
    {
        auto node = pBody->GetNode();
        assert(node);

        Vec3 pos = Bullet::ToVec3(worldTrans.getOrigin());
        Quat rot = Bullet::ToQuat(worldTrans.getRotation());
        pos -= (pBody->localCenterOfMass * rot);

        node->SetPosition(pos);
        node->SetRotation(rot);
        pBody->transformDirty = false;
    }
};

RigidBody::RigidBody()
{
    btRigidBody::btRigidBodyConstructionInfo info(mass, nullptr, nullptr);
    info.m_collisionShape = shape.get();
    info.m_friction = friction;
    info.m_rollingFriction = rollingFriction;
    info.m_spinningFriction = spinningFriction;
    info.m_restitution = restitution;
    info.m_linearDamping = linearDamping;
    info.m_angularDamping = angularDamping;
    info.m_linearSleepingThreshold = 0.000001f;
    info.m_angularSleepingThreshold = 0.000001f;
    
    body = upnew<btRigidBody>(info);
    body->setUserPointer((void*)this);
    
    motionState = upnew<RigidBodyMotionState>(this);
}

RigidBody::~RigidBody()
{
}

void RigidBody::ToJson(json& obj) const
{
    Component::ToJson(obj);

    obj["bodyType"] = bodyType;
    obj["mass"] = mass;
    obj["friction"] = friction;
    obj["rollingFriction"] = rollingFriction;
    obj["spinningFriction"] = spinningFriction;
    obj["restitution"] = restitution;
    obj["linearDamping"] = linearDamping;
    obj["angularDamping"] = angularDamping;
}

void RigidBody::FromJson(const json& obj, ObjectLinker* linker)
{
    Component::FromJson(obj, linker);

    bodyType = obj.value("bodyType", bodyType);
    mass = obj.value("mass", mass);
    friction = obj.value("friction", friction);
    rollingFriction = obj.value("rollingFriction", rollingFriction);
    spinningFriction = obj.value("spinningFriction", spinningFriction);
    restitution = obj.value("restitution", restitution);
    linearDamping = obj.value("linearDamping", linearDamping);
    angularDamping = obj.value("angularDamping", angularDamping);
    
    structureDirty = true;
    transformDirty = true;
}

void RigidBody::ClearCollisionShape()
{
    body->setCollisionShape(nullptr);
    shape.reset();
    childShapes.clear();
    colliders.clear();
}

void RigidBody::RebuildCollisionShape()
{
    ClearCollisionShape();

    struct R
    {
        static void FindColliders(
            const sptr<Node>& node,
            bool isRoot,
            std::vector<sptr<Collider>>& out)
        {
            for (auto& c : node->GetComponents())
            {
                if (!isRoot && std::dynamic_pointer_cast<RigidBody>(c))
                    return;

                if (auto t = std::dynamic_pointer_cast<Collider>(c))
                    out.push_back(std::move(t));
            }

            for (auto& n : node->GetChildren())
                FindColliders(n, false, out);
        }
    };

    R::FindColliders(GetNode(), true, colliders);

    shape = upnew<btCompoundShape>();

    for (auto& collider : colliders)
    {
        uptr<btCollisionShape> childShape;

        auto& type = collider->GetObjectType();
        if (type == BoxCollider::GetClassType())
        {
            BoxCollider* c = (BoxCollider*)collider.get();
            childShape = upnew<btBoxShape>(Bullet::FromVec3(c->GetExtents()));
        }
        else if (type == CapsuleCollider::GetClassType())
        {
            CapsuleCollider* c = (CapsuleCollider*)collider.get();
            auto upAxis = c->GetUpAxis();
            auto radius = c->GetRadius();
            auto height = c->GetHeight();
            childShape = upnew<CapsuleShape>(upAxis, radius, height);
        }
        else if (type == MeshCollider::GetClassType())
        {
            MeshCollider* c = (MeshCollider*)collider.get();
            auto meshShape = upnew<btConvexHullShape>();
            
            if (auto mesh = c->GetMesh())
            {
                for (auto& v : mesh->vertices)
                    meshShape->addPoint(Bullet::FromVec3(v), false);
                
                meshShape->recalcLocalAabb();
            }

            childShape = std::move(meshShape);
        }
        else if (type == SphereCollider::GetClassType())
        {
            SphereCollider* c = (SphereCollider*)collider.get();
            childShape = upnew<btSphereShape>(c->GetRadius());
        }

        shape->addChildShape(btTransform::getIdentity(), childShape.get());
        childShapes.emplace_back(std::move(childShape));
        collider->SetDirty(false);
    }

    body->setCollisionShape(shape.get());
}

void RigidBody::UpdateStructure()
{
    bool colliderDirty = std::any_of(
        colliders.begin(), colliders.end(),
        [](auto& c) { return c->IsDirty(); });

    if (structureDirty || colliderDirty)
    {
        if (auto scene = GetNode()->GetScene())
        {
            if(auto w = world.lock()) {
                w->RemoveRigidBody(This<RigidBody>());
                world.reset();
            }

            RebuildCollisionShape();
            UpdateChildTransforms();

            if(!body->getMotionState())
                body->setMotionState(motionState.get());

            UpdateTransform(true);

            using btCollisionObject::CF_KINEMATIC_OBJECT;
            using btCollisionObject::CF_NO_CONTACT_RESPONSE;

            if (bodyType == BodyType::Dynamic)
            {
                btVector3 localInertia(0, 0, 0);
                shape->calculateLocalInertia(mass, localInertia);
                body->setMassProps(mass, localInertia);
                body->forceActivationState(ACTIVE_TAG);
                body->setLinearVelocity({ 0, 0, 0 });
                body->setAngularVelocity({ 0, 0, 0 });

                auto flags = body->getCollisionFlags();
                flags &= ~CF_KINEMATIC_OBJECT;
                flags &= ~CF_NO_CONTACT_RESPONSE;
                body->setCollisionFlags(flags);
            }
            else if(bodyType == BodyType::Kinematic)
            {
                body->setMassProps(0, {0, 0, 0});
                body->forceActivationState(DISABLE_DEACTIVATION);
                body->setLinearVelocity({ 0, 0, 0 });
                body->setAngularVelocity({ 0, 0, 0 });

                auto flags = body->getCollisionFlags();
                flags |= CF_KINEMATIC_OBJECT;
                flags &= ~CF_NO_CONTACT_RESPONSE;
                body->setCollisionFlags(flags);
            }
            else if (bodyType == BodyType::Static)
            {
                body->setMassProps(0, { 0, 0, 0 });
                body->forceActivationState(ACTIVE_TAG);
                body->setLinearVelocity({ 0, 0, 0 });
                body->setAngularVelocity({ 0, 0, 0 });

                auto flags = body->getCollisionFlags();
                flags &= ~CF_KINEMATIC_OBJECT;
                flags &= ~CF_NO_CONTACT_RESPONSE;
                body->setCollisionFlags(flags);
            }
            else if (bodyType == BodyType::Ghost)
            {
                body->setMassProps(0, { 0, 0, 0 });
                body->forceActivationState(DISABLE_DEACTIVATION);
                body->setLinearVelocity({ 0, 0, 0 });
                body->setAngularVelocity({ 0, 0, 0 });

                auto flags = body->getCollisionFlags();
                flags &= ~CF_KINEMATIC_OBJECT;
                flags |= CF_NO_CONTACT_RESPONSE;
                body->setCollisionFlags(flags);
            }

            body->setFriction(friction);
            body->setRollingFriction(rollingFriction);
            body->setSpinningFriction(spinningFriction);
            body->setRestitution(restitution);
            body->setDamping(linearDamping, angularDamping);

            if (IsActiveAndEnabled()) {
                scene->GetPhysics()->AddRigidBody(This<RigidBody>());
                world = scene->GetPhysics();
            }
            
            structureDirty = false;
        }
    }
}

Vec3 RigidBody::CalcShapeCenter()
{
    Vec3 worldCenter = {};

    if (!colliders.empty())
    {
        for (auto& c : colliders)
            worldCenter += c->GetPivot()->GetPosition();

        worldCenter = worldCenter.Divide(Vec3::One() * (float)colliders.size());
    }

    return worldCenter;
}

void RigidBody::UpdateChildTransforms()
{
    auto node = GetNode();
    auto bodyPos = node->GetPosition();
    auto bodyRot = node->GetRotation();

    localCenterOfMass = (CalcShapeCenter() - bodyPos) * bodyRot.Inverse();

    for (int i = 0; i != colliders.size(); ++i)
    {
        auto pivot = colliders[i]->GetPivot();
        auto pivotPos = pivot->GetPosition();
        auto pivotRot = pivot->GetRotation();
        auto pivotScale = pivot->GetScale();
        
        auto pivotLocalPos = (pivotPos - bodyPos) * bodyRot.Inverse() - localCenterOfMass;
        auto pivotLocalRot = pivotRot * bodyRot.Inverse();

        btTransform pivotLocalTrans(
            Bullet::FromQuat(pivotLocalRot),
            Bullet::FromVec3(pivotLocalPos)
        );

        shape->updateChildTransform(i, pivotLocalTrans, false);
        childShapes[i]->setLocalScaling(Bullet::FromVec3(pivotScale));
    }

    shape->recalculateLocalAabb();
}

void RigidBody::UpdateTransform(bool force)
{
    if (transformDirty || force)
    {
        btTransform worldTransform;
        motionState->getWorldTransform(worldTransform);
        body->setCenterOfMassTransform(worldTransform);
        transformDirty = false;
    }
}

void RigidBody::SystemUpdate1()
{
    UpdateStructure();
    UpdateChildTransforms();
    UpdateTransform();
}

void RigidBody::OnAttachedToScene() {
    structureDirty = true;
}

void RigidBody::OnDetachFromScene()
{
    if(auto w = world.lock()) {
        w->RemoveRigidBody(This<RigidBody>());
        world.reset();
    }

    ClearCollisionShape();
}

void RigidBody::OnEnable()
{
    if (IsActiveAndEnabled())
        structureDirty = true;
}

void RigidBody::OnDisable()
{
    if (auto w = world.lock())
    {
        w->RemoveRigidBody(This<RigidBody>());
        world.reset();
    }
}

void RigidBody::OnNodeActivated() {
    OnEnable();
}

void RigidBody::OnNodeDeactivated() {
    OnDisable();
}

void RigidBody::OnTransformChanged() {
    transformDirty = true;
}

void RigidBody::OnStructureChanged() {
    structureDirty = true;
}

float RigidBody::GetMass() const {
    return mass;
}

void RigidBody::SetMass(float mass)
{
    this->mass = mass;
    structureDirty = true;
}

BodyType RigidBody::GetBodyType() const {
    return bodyType;
}

void RigidBody::SetBodyType(BodyType type) {
    bodyType = type;
    structureDirty = true;
}

Vec3 RigidBody::GetLinearFactor() const {
    auto factor = body->getLinearFactor();
    return { factor.x(), factor.y(), factor.z() };
}

void RigidBody::SetLinearFactor(const Vec3& factor) {
    body->setLinearFactor({ factor.x, factor.y, factor.z });
}

Vec3 RigidBody::GetAngularFactor() const {
    auto factor = body->getAngularFactor();
    return { factor.x(), factor.y(), factor.z() };
}

void RigidBody::SetAngularFactor(const Vec3& factor) {
    body->setAngularFactor({ factor.x, factor.y, factor.z });
}

Vec3 RigidBody::GetVelocity() const
{
    btVector3 vel = body->getLinearVelocity();
    return Vec3(vel.x(), vel.y(), vel.z());
}

void RigidBody::SetVelocity(const Vec3& vel)
{
    body->activate();
    body->setLinearVelocity(btVector3(vel.x, vel.y, vel.z));
}

Vec3 RigidBody::GetAngularVelocity() const
{
    btVector3 vel = body->getAngularVelocity();
    return Vec3(vel.x(), vel.y(), vel.z());
}

void RigidBody::SetAngularVelocity(const Vec3& vel)
{
    body->activate();
    body->setAngularVelocity(btVector3(vel.x, vel.y, vel.z));
}

float RigidBody::GetFriction() const {
    return body->getFriction();
}

void RigidBody::SetFriction(float fric) {
    friction = fric;
    body->setFriction(fric);
}

float RigidBody::GetRollingFriction() const {
    return body->getRollingFriction();
}

void RigidBody::SetRollingFriction(float fric) {
    rollingFriction = fric;
    body->setRollingFriction(fric);
}

float RigidBody::GetSpinningFriction() const {
    return body->getSpinningFriction();
}

void RigidBody::SetSpinningFriction(float fric) {
    spinningFriction = fric;
    body->setSpinningFriction(fric);
}

float RigidBody::GetRestitution() const {
    return body->getSpinningFriction();
}

void RigidBody::SetRestitution(float rest) {
    restitution = rest;
    body->setRestitution(rest);
}

float RigidBody::GetLinearDamping() const {
    return linearDamping;
}

void RigidBody::SetLinearDamping(float damping) {
    linearDamping = damping;
    body->setDamping(linearDamping, angularDamping);
}

float RigidBody::GetAngularDamping() const {
    return angularDamping;
}

void RigidBody::SetAngularDamping(float damping) {
    angularDamping = damping;
    body->setDamping(linearDamping, angularDamping);
}

void RigidBody::AddForce(const Vec3& force)
{
    body->activate();
    body->applyCentralForce(
        { force.x, force.y, force.z }
    );
}

void RigidBody::AddForce(const Vec3& force, const Vec3& point)
{
    Vec3 relPos = point - GetNode()->GetPosition();
    
    body->activate();
    body->applyForce(
        { force.x, force.y, force.z },
        { relPos.x, relPos.y, relPos.z }
    );
}

void RigidBody::AddImpulse(const Vec3& impulse)
{
    body->activate();
    body->applyCentralImpulse(
        { impulse.x, impulse.y, impulse.z }
    );
}

void RigidBody::AddImpulse(const Vec3& impulse, const Vec3& point)
{
    Vec3 relPos = point - GetNode()->GetPosition();

    body->activate();
    body->applyImpulse(
        { impulse.x, impulse.y, impulse.z },
        { relPos.x, relPos.y, relPos.z }
    );
}

void RigidBody::AddTorque(const Vec3& torque)
{
    body->activate();
    body->applyTorque(btVector3(torque.x, torque.y, torque.z));
}

void to_json(json& obj, const BodyType& bodyType)
{
    static const char* names[]{
        "Dynamic",
        "Kinematic",
        "Static",
        "Ghost"
    };
    obj = names[(int)bodyType];
}

void from_json(const json& obj, BodyType& bodyType)
{
    static std::unordered_map<std::string, BodyType> types{
        { "Dynamic", BodyType::Dynamic },
        { "Kinematic", BodyType::Kinematic },
        { "Static", BodyType::Static },
        { "Ghost", BodyType::Ghost }
    };
    bodyType = types[obj.get<std::string>("Invalid")];
}

} // scene
} // mw
