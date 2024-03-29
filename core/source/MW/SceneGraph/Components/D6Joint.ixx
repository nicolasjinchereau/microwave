/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.D6Joint;
import Microwave.Math;
import Microwave.SceneGraph.Axis;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Components.Component;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <climits>;
import <array>;
import <MW/SceneGraph/Internal/Bullet.h>;

//export {
//class btGeneric6DofSpring2Constraint;
//}

export namespace mw {
inline namespace scene {

class RigidBody;
class PhysicsWorld;

enum class JointMotion
{
    Locked,
    Limited,
    Free
};

enum class RotationOrder
{
    XYZ,
    XZY,
    YXZ,
    YZX,
    ZXY,
    ZYX
};

class D6Joint :
    public Component,
    public ISystemEvents
{
    inline static Type::Pin<D6Joint> pin;

    friend PhysicsWorld;

    uptr<btGeneric6DofSpring2Constraint> joint;
    bool structureDirty = true;
    bool propertiesDirty = true;

    gptr<RigidBody> mainBody;
    gptr<RigidBody> linkBody;
    Vec3 linkOffset; // in space of this node
    Vec3 linkPivotOffset; // in space of the link-body's node

    std::array<JointMotion, 3> linearJointMotion { JointMotion::Locked, JointMotion::Locked, JointMotion::Locked };
    std::array<JointMotion, 3> angularJointMotion { JointMotion::Locked, JointMotion::Locked, JointMotion::Locked };

    Vec3 linearLowerLimit;
    Vec3 linearUpperLimit;
    Vec3 angularLowerLimit;
    Vec3 angularUpperLimit;

    std::array<bool, 3> linearMotorEnabled { false, false, false };
    std::array<bool, 3> angularMotorEnabled { false, false, false };

    Vec3 linearMotorTargetVelocity;
    Vec3 angularMotorTargetVelocity;

    Vec3 linearMotorMaxForce = { FLT_MAX, FLT_MAX, FLT_MAX };
    Vec3 angularMotorMaxForce = { FLT_MAX, FLT_MAX, FLT_MAX };

    RotationOrder rotationOrder = RotationOrder::XYZ;

    void Construct();
public:
    D6Joint(){ Construct(); }
    ~D6Joint(){}

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    void SetLinkBody(const gptr<RigidBody>& body);

    void SetLinkOffset(const Vec3& offset);
    Vec3 GetLinkOffset() const;

    void SetLinkPivotOffset(const Vec3& offset);
    Vec3 GetLinkPivotOffset() const;
    
    void SetRotationOrder(RotationOrder order);
    RotationOrder GetRotationOrder() const;

    // default is JointMotion::Locked
    void SetLinearJointMotion(JointMotion motion);
    void SetLinearJointMotion(Axis axis, JointMotion motion);
    JointMotion GetLinearJointMotion(Axis axis) const;

    // default is JointMotion::Locked
    void SetAngularJointMotion(JointMotion motion);
    void SetAngularJointMotion(Axis axis, JointMotion motion);
    JointMotion GetAngularJointMotion(Axis axis) const;

    void SetLinearLowerLimit(const Vec3& limit);
    void SetLinearLowerLimit(Axis axis, float limit);
    Vec3  GetLinearLowerLimit() const;
    float GetLinearLowerLimit(Axis axis) const;

    void SetLinearUpperLimit(const Vec3& limit);
    void SetLinearUpperLimit(Axis axis, float limit);
    Vec3  GetLinearUpperLimit() const;
    float GetLinearUpperLimit(Axis axis) const;

    void SetAngularLowerLimit(const Vec3& limit);
    void SetAngularLowerLimit(Axis axis, float limit);
    void SetAngularUpperLimit(const Vec3& limit);
    void SetAngularUpperLimit(Axis axis, float limit);

    Vec3  GetAngularLowerLimit() const;
    float GetAngularLowerLimit(Axis axis) const;
    Vec3  GetAngularUpperLimit() const;
    float GetAngularUpperLimit(Axis axis) const;

    // default is 'false'
    void SetLinearMotorEnabled(bool enabled);
    void SetLinearMotorEnabled(Axis axis, bool enabled);
    bool GetLinearMotorEnabled(Axis axis) const;

    // default is 'false'
    void SetAngularMotorEnabled(bool enabled);
    void SetAngularMotorEnabled(Axis axis, bool enabled);
    bool GetAngularMotorEnabled(Axis axis) const;

    void SetLinearTargetVelocity(const Vec3& velocity);
    void SetLinearTargetVelocity(Axis axis, float velocity);
    Vec3  GetLinearTargetVelocity() const;
    float GetLinearTargetVelocity(Axis axis) const;

    void SetAngularTargetVelocity(const Vec3& velocity);
    void SetAngularTargetVelocity(Axis axis, float velocity);
    Vec3  GetAngularTargetVelocity() const;
    float GetAngularTargetVelocity(Axis axis) const;

    void SetLinearMaxForce(const Vec3& force);
    void SetLinearMaxForce(Axis axis, float force);
    Vec3  GetLinearMaxForce() const;
    float GetLinearMaxForce(Axis axis) const;

    void SetAngularMaxForce(const Vec3& force);
    void SetAngularMaxForce(Axis axis, float force);
    Vec3  GetAngularMaxForce() const;
    float GetAngularMaxForce(Axis axis) const;

    virtual void SystemUpdate2() override;
    virtual void OnStructureChanged() override;
    virtual void OnAttachedToScene() override;
    virtual void OnDetachFromScene() override;
};

void to_json(json& obj, const JointMotion& jointMotion);
void from_json(const json& obj, JointMotion& jointMotion);

void to_json(json& obj, const RotationOrder& order);
void from_json(const json& obj, RotationOrder& order);

} // scene
} // mw
