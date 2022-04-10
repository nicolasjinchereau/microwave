/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.RigidBody;
import Microwave.Math;
import Microwave.SceneGraph.Components.Component;
import Microwave.SceneGraph.Components.Collider;
import Microwave.SceneGraph.Events;
import Microwave.System.Console;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <vector>;
import <atomic>;
import <MW/SceneGraph/Internal/Bullet.h>;

//export {
//class btRigidBody;
//struct btDefaultMotionState;
//class btCollisionShape;
//class btCompoundShape;
//}

export namespace mw {
inline namespace scene {

class Bullet;
class D6Joint;
class PhysicsWorld;
class RigidBodyMotionState;

enum class BodyType
{
    Dynamic,
    Kinematic,
    Static,
    Ghost
};

class RigidBody : public Component
                , public ISystemEvents
{
    inline static Type::Pin<RigidBody> pin;

    wptr<PhysicsWorld> world;
    uptr<RigidBodyMotionState> motionState;
    uptr<btRigidBody> body;
    uptr<btCompoundShape> shape;
    std::vector<uptr<btCollisionShape>> childShapes;
    std::vector<sptr<Collider>> colliders;
    bool structureDirty = true;
    bool transformDirty = true;

    BodyType bodyType = BodyType::Dynamic;
    float mass = 1.0f;
    float friction = 0.5f;
    float rollingFriction = 0.0f;
    float spinningFriction = 0.0f;
    float restitution = 0.0f;
    float linearDamping = 0.0f;
    float angularDamping = 0.05f;

    Vec3 localCenterOfMass = {};

    friend class PhysicsWorld;
    friend class RigidBodyMotionState;
    friend class Bullet;
    friend class D6Joint;

    void Construct();
    void Destruct();
public:

    RigidBody(){ Construct(); }
    virtual ~RigidBody() { Destruct(); }

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    float GetMass() const;
    void SetMass(float mass);

    BodyType GetBodyType() const;
    void SetBodyType(BodyType type);

    Vec3 GetLinearFactor() const;
    void SetLinearFactor(const Vec3& factors);

    Vec3 GetAngularFactor() const;
    void SetAngularFactor(const Vec3& factors);

    Vec3 GetVelocity() const;
    void SetVelocity(const Vec3& vel);

    Vec3 GetAngularVelocity() const;
    void SetAngularVelocity(const Vec3& vel);

    float GetFriction() const;
    void SetFriction(float fric);

    float GetRollingFriction() const;
    void SetRollingFriction(float fric);

    float GetSpinningFriction() const;
    void SetSpinningFriction(float fric);

    float GetRestitution() const;
    void SetRestitution(float rest);

    float GetLinearDamping() const;
    void SetLinearDamping(float damping);

    float GetAngularDamping() const;
    void SetAngularDamping(float damping);

    void AddForce(const Vec3& force);
    void AddForce(const Vec3& force, const Vec3& point);
    void AddImpulse(const Vec3& impulse);
    void AddImpulse(const Vec3& impulse, const Vec3& point);
    void AddTorque(const Vec3& torque);

    virtual void SystemUpdate1() override;
    virtual void OnTransformChanged() override;
    virtual void OnStructureChanged() override;
    virtual void OnAttachedToScene() override;
    virtual void OnDetachFromScene() override;

    //virtual void OnEnable() override;
    //virtual void OnDisable() override;
    virtual void OnNodeActivated() override;
    virtual void OnNodeDeactivated() override;
    
    void UpdateTransform(bool force = false);
private:
    void UpdateStructure();
    void UpdateChildTransforms();

    void ClearCollisionShape();
    void RebuildCollisionShape();

    // average of all collider pivots in world space
    Vec3 CalcShapeCenter();
};

class RigidBodyMotionState : public btMotionState
{
    RigidBody* pBody;
public:
    RigidBodyMotionState(RigidBody* pBody);
    virtual void getWorldTransform(btTransform& worldTrans) const override;
    virtual void setWorldTransform(const btTransform& worldTrans) override;
};

void to_json(json& obj, const BodyType& bodyType);
void from_json(const json& obj, BodyType& bodyType);

} // scene
} // mw
