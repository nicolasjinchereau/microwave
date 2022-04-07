/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/Json.h>
#include <MW/Scene/Components/D6Joint.h>
#include <MW/Scene/Components/RigidBody.h>
#include <MW/Scene/Internal/Bullet.h>
#include <MW/Scene/Node.h>
#include <MW/Scene/PhysicsWorld.h>
#include <MW/Scene/Scene.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace mw {
inline namespace scene {

D6Joint::D6Joint()
{
    btRigidBody& rbA = btTypedConstraint::getFixedBody();
    btRigidBody& rbB = btTypedConstraint::getFixedBody();
    btTransform linkOffsetTransform = btTransform::getIdentity();
    btTransform linkPivotOffsetTransform = btTransform::getIdentity();

    joint = upnew<btGeneric6DofSpring2Constraint>(
          rbA
        , rbB
        , linkOffsetTransform
        , linkPivotOffsetTransform
        , RotateOrder::RO_XYZ
        );
}

D6Joint::~D6Joint()
{
    
}

void D6Joint::ToJson(json& obj) const
{
    Component::ToJson(obj);
    
    ObjectLinker::SaveLink(obj, "linkBody", linkBody);

    obj["linkOffset"] = linkOffset;
    obj["linkPivotOffset"] = linkPivotOffset;

    obj["linearJointMotionX"] = linearJointMotion[0];
    obj["linearJointMotionY"] = linearJointMotion[1];
    obj["linearJointMotionZ"] = linearJointMotion[2];

    obj["angularJointMotionX"] = angularJointMotion[0];
    obj["angularJointMotionY"] = angularJointMotion[1];
    obj["angularJointMotionZ"] = angularJointMotion[2];

    obj["linearLowerLimit"] = linearLowerLimit;
    obj["linearUpperLimit"] = linearUpperLimit;
    obj["angularLowerLimit"] = angularLowerLimit;
    obj["angularUpperLimit"] = angularUpperLimit;

    obj["linearMotorEnabledX"] = linearMotorEnabled[0];
    obj["linearMotorEnabledY"] = linearMotorEnabled[1];
    obj["linearMotorEnabledZ"] = linearMotorEnabled[2];
    obj["angularMotorEnabledX"] = angularMotorEnabled[0];
    obj["angularMotorEnabledY"] = angularMotorEnabled[1];
    obj["angularMotorEnabledZ"] = angularMotorEnabled[2];

    obj["linearMotorTargetVelocity"] = linearMotorTargetVelocity;
    obj["angularMotorTargetVelocity"] = angularMotorTargetVelocity;

    obj["linearMotorMaxForce"] = linearMotorMaxForce;
    obj["angularMotorMaxForce"] = angularMotorMaxForce;

    obj["rotationOrder"] = rotationOrder;
}

void D6Joint::FromJson(const json& obj, ObjectLinker* linker)
{
    Component::FromJson(obj, linker);

    ObjectLinker::RestoreLink(linker, This(), linkBody, obj, "linkBody");

    linkOffset = obj.value("linkOffset", linkOffset);
    linkPivotOffset = obj.value("linkPivotOffset", linkPivotOffset);

    linearJointMotion[0] = obj.value("linearJointMotionX", linearJointMotion[0]);
    linearJointMotion[1] = obj.value("linearJointMotionY", linearJointMotion[1]);
    linearJointMotion[2] = obj.value("linearJointMotionZ", linearJointMotion[2]);
    
    angularJointMotion[0] = obj.value("angularJointMotionX", angularJointMotion[0]);
    angularJointMotion[1] = obj.value("angularJointMotionY", angularJointMotion[1]);
    angularJointMotion[2] = obj.value("angularJointMotionZ", angularJointMotion[2]);

    linearLowerLimit = obj.value("linearLowerLimit", linearLowerLimit);
    linearUpperLimit = obj.value("linearUpperLimit", linearUpperLimit);
    angularLowerLimit = obj.value("angularLowerLimit", angularLowerLimit);
    angularUpperLimit = obj.value("angularUpperLimit", angularUpperLimit);

    linearMotorEnabled[0] = obj.value("linearMotorEnabledX", linearMotorEnabled[0]);
    linearMotorEnabled[1] = obj.value("linearMotorEnabledY", linearMotorEnabled[1]);
    linearMotorEnabled[2] = obj.value("linearMotorEnabledZ", linearMotorEnabled[2]);
    angularMotorEnabled[0] = obj.value("angularMotorEnabledX", angularMotorEnabled[0]);
    angularMotorEnabled[1] = obj.value("angularMotorEnabledY", angularMotorEnabled[1]);
    angularMotorEnabled[2] = obj.value("angularMotorEnabledZ", angularMotorEnabled[2]);

    linearMotorTargetVelocity = obj.value("linearMotorTargetVelocity", linearMotorTargetVelocity);
    angularMotorTargetVelocity = obj.value("angularMotorTargetVelocity", angularMotorTargetVelocity);

    linearMotorMaxForce = obj.value("linearMotorMaxForce", linearMotorMaxForce);
    angularMotorMaxForce = obj.value("angularMotorMaxForce", angularMotorMaxForce);

    rotationOrder = obj.value("rotationOrder", rotationOrder);

    structureDirty = true;
    propertiesDirty = true;
}

void D6Joint::SetLinkBody(const sptr<RigidBody>& body) {
    linkBody = body;
    structureDirty = true;
}

void D6Joint::SetLinkOffset(const Vec3& offset) {
    linkOffset = offset;
    structureDirty = true;
}

Vec3 D6Joint::GetLinkOffset() const {
    return linkOffset;
}

void D6Joint::SetLinkPivotOffset(const Vec3& offset) {
    linkPivotOffset = offset;
    structureDirty = true;
}

Vec3 D6Joint::GetLinkPivotOffset() const {
    return linkPivotOffset;
}

void D6Joint::SetRotationOrder(RotationOrder order)
{
    if(order != rotationOrder) {
        rotationOrder = order;
        structureDirty = true;
    }
}

RotationOrder D6Joint::GetRotationOrder() const {
    return rotationOrder;
}

void D6Joint::SetLinearJointMotion(JointMotion motion) {
    SetLinearJointMotion(Axis::X, motion);
    SetLinearJointMotion(Axis::Y, motion);
    SetLinearJointMotion(Axis::Z, motion);
}

void D6Joint::SetLinearJointMotion(Axis axis, JointMotion motion)
{
    if (linearJointMotion[(int)axis] != motion) {
        linearJointMotion[(int)axis] = motion;
        propertiesDirty = true;
    }
}

JointMotion D6Joint::GetLinearJointMotion(Axis axis) const {
    return linearJointMotion[(int)axis];
}

void D6Joint::SetAngularJointMotion(JointMotion motion) {
    SetAngularJointMotion(Axis::X, motion);
    SetAngularJointMotion(Axis::Y, motion);
    SetAngularJointMotion(Axis::Z, motion);
}

void D6Joint::SetAngularJointMotion(Axis axis, JointMotion motion)
{
    if (angularJointMotion[(int)axis] != motion) {
        angularJointMotion[(int)axis] = motion;
        propertiesDirty = true;
    }
}

JointMotion D6Joint::GetAngularJointMotion(Axis axis) const {
    return angularJointMotion[(int)axis];
}

void D6Joint::SetLinearLowerLimit(const Vec3& limit)
{
    if (linearLowerLimit != limit) {
        linearLowerLimit = limit;
        propertiesDirty = true;
    }
}

void D6Joint::SetLinearLowerLimit(Axis axis, float limit)
{
    if (linearLowerLimit[(int)axis] != limit) {
        linearLowerLimit[(int)axis] = limit;
        propertiesDirty = true;
    }
}

Vec3 D6Joint::GetLinearLowerLimit() const {
    return linearLowerLimit;
}

float D6Joint::GetLinearLowerLimit(Axis axis) const {
    return linearLowerLimit[(int)axis];
}

void D6Joint::SetLinearUpperLimit(const Vec3& limit)
{
    if (linearUpperLimit != limit) {
        linearUpperLimit = limit;
        propertiesDirty = true;
    }
}

void D6Joint::SetLinearUpperLimit(Axis axis, float limit)
{
    if (linearUpperLimit[(int)axis] != limit) {
        linearUpperLimit[(int)axis] = limit;
        propertiesDirty = true;
    }
}

Vec3 D6Joint::GetLinearUpperLimit() const {
    return linearUpperLimit;
}

float D6Joint::GetLinearUpperLimit(Axis axis) const {
    return linearUpperLimit[(int)axis];
}

void D6Joint::SetAngularLowerLimit(const Vec3& limit)
{
    if (angularLowerLimit != limit) {
        angularLowerLimit = limit;
        propertiesDirty = true;
    }
}

void D6Joint::SetAngularLowerLimit(Axis axis, float limit)
{
    if (angularLowerLimit[(int)axis] != limit) {
        angularLowerLimit[(int)axis] = limit;
        propertiesDirty = true;
    }
}

void D6Joint::SetAngularUpperLimit(const Vec3& limit)
{
    if (angularUpperLimit != limit) {
        angularUpperLimit = limit;
        propertiesDirty = true;
    }
}

void D6Joint::SetAngularUpperLimit(Axis axis, float limit)
{
    if (angularUpperLimit[(int)axis] != limit) {
        angularUpperLimit[(int)axis] = limit;
        propertiesDirty = true;
    }
}

Vec3 D6Joint::GetAngularLowerLimit() const {
    return angularLowerLimit;
}

float D6Joint::GetAngularLowerLimit(Axis axis) const {
    return angularLowerLimit[(int)axis];
}

Vec3 D6Joint::GetAngularUpperLimit() const {
    return angularUpperLimit;
}

float D6Joint::GetAngularUpperLimit(Axis axis) const {
    return angularUpperLimit[(int)axis];
}

void D6Joint::SetLinearMotorEnabled(bool enabled)
{
    SetLinearMotorEnabled(Axis::X, enabled);
    SetLinearMotorEnabled(Axis::Y, enabled);
    SetLinearMotorEnabled(Axis::Z, enabled);
}

void D6Joint::SetLinearMotorEnabled(Axis axis, bool enabled)
{
    if (linearMotorEnabled[(int)axis] != enabled) {
        linearMotorEnabled[(int)axis] = enabled;
        propertiesDirty = true;
    }
}

bool D6Joint::GetLinearMotorEnabled(Axis axis) const {
    return linearMotorEnabled[(int)axis];
}

void D6Joint::SetAngularMotorEnabled(bool enabled) {
    SetAngularMotorEnabled(Axis::X, enabled);
    SetAngularMotorEnabled(Axis::Y, enabled);
    SetAngularMotorEnabled(Axis::Z, enabled);
}

void D6Joint::SetAngularMotorEnabled(Axis axis, bool enabled)
{
    if (angularMotorEnabled[(int)axis] != enabled) {
        angularMotorEnabled[(int)axis] = enabled;
        propertiesDirty = true;
    }
}

bool D6Joint::GetAngularMotorEnabled(Axis axis) const {
    return angularMotorEnabled[(int)axis];
}

void D6Joint::SetLinearTargetVelocity(const Vec3& velocity)
{
    if (linearMotorTargetVelocity != velocity) {
        linearMotorTargetVelocity = velocity;
        propertiesDirty = true;
    }
}

void D6Joint::SetLinearTargetVelocity(Axis axis, float velocity)
{
    if (linearMotorTargetVelocity[(int)axis] != velocity) {
        linearMotorTargetVelocity[(int)axis] = velocity;
        propertiesDirty = true;
    }
}

Vec3  D6Joint::GetLinearTargetVelocity() const {
    return linearMotorTargetVelocity;
}

float D6Joint::GetLinearTargetVelocity(Axis axis) const {
    return linearMotorTargetVelocity[(int)axis];
}

void D6Joint::SetAngularTargetVelocity(const Vec3& velocity)
{
    if (angularMotorTargetVelocity != velocity) {
        angularMotorTargetVelocity = velocity;
        propertiesDirty = true;
    }
}

void D6Joint::SetAngularTargetVelocity(Axis axis, float velocity)
{
    if (angularMotorTargetVelocity[(int)axis] != velocity) {
        angularMotorTargetVelocity[(int)axis] = velocity;
        propertiesDirty = true;
    }
}

Vec3 D6Joint::GetAngularTargetVelocity() const {
    return angularMotorTargetVelocity;
}

float D6Joint::GetAngularTargetVelocity(Axis axis) const {
    return angularMotorTargetVelocity[(int)axis];
}

void D6Joint::SetLinearMaxForce(const Vec3& force)
{
    if (linearMotorMaxForce != force) {
        linearMotorMaxForce = force;
        propertiesDirty = true;
    }
}

void D6Joint::SetLinearMaxForce(Axis axis, float force)
{
    if (linearMotorMaxForce[(int)axis] != force) {
        linearMotorMaxForce[(int)axis] = force;
        propertiesDirty = true;
    }
}

Vec3 D6Joint::GetLinearMaxForce() const {
    return linearMotorMaxForce;
}

float D6Joint::GetLinearMaxForce(Axis axis) const {
    return linearMotorMaxForce[(int)axis];
}

void D6Joint::SetAngularMaxForce(const Vec3& force)
{
    if (angularMotorMaxForce != force) {
        angularMotorMaxForce = force;
        propertiesDirty = true;
    }
}

void D6Joint::SetAngularMaxForce(Axis axis, float force)
{
    if (angularMotorMaxForce[(int)axis] != force) {
        angularMotorMaxForce[(int)axis] = force;
        propertiesDirty = true;
    }
}

Vec3 D6Joint::GetAngularMaxForce() const {
    return angularMotorMaxForce;
}

float D6Joint::GetAngularMaxForce(Axis axis) const {
    return angularMotorMaxForce[(int)axis];
}

void D6Joint::SystemUpdate2()
{
    auto pWorld = GetNode()->GetScene()->GetPhysics()->world.get();

    RotateOrder rotOrder = {};

    if (rotationOrder == RotationOrder::XYZ)
        rotOrder = RotateOrder::RO_XYZ;
    else if (rotationOrder == RotationOrder::XZY)
        rotOrder = RotateOrder::RO_XZY;
    else if (rotationOrder == RotationOrder::YXZ)
        rotOrder = RotateOrder::RO_YXZ;
    else if (rotationOrder == RotationOrder::YZX)
        rotOrder = RotateOrder::RO_YZX;
    else if (rotationOrder == RotationOrder::ZXY)
        rotOrder = RotateOrder::RO_ZXY;
    else if (rotationOrder == RotationOrder::ZYX)
        rotOrder = RotateOrder::RO_ZYX;

    if (structureDirty)
    {
        pWorld->removeConstraint(joint.get());

        mainBody = GetNode()->GetComponent<RigidBody>();

        btTransform linkOffsetTransform = btTransform::getIdentity();
        btTransform linkPivotOffsetTransform = btTransform::getIdentity();
        btRigidBody* rbA = &btTypedConstraint::getFixedBody();
        btRigidBody* rbB = &btTypedConstraint::getFixedBody();

        if(mainBody)
        {
            rbA = mainBody->body.get();
            Vec3 linkPos = -mainBody->localCenterOfMass + linkOffset;
            linkOffsetTransform.setOrigin(Bullet::FromVec3(linkPos));
        }
        else
        {
            Vec3 linkPos = Vec3::Zero() + linkOffset;
            linkOffsetTransform.setOrigin(Bullet::FromVec3(linkPos));
        }
        
        if(linkBody)
        {
            rbB = linkBody->body.get();
            Vec3 linkPivot = -linkBody->localCenterOfMass + linkPivotOffset;
            linkPivotOffsetTransform.setOrigin(Bullet::FromVec3(linkPivot));
        }
        
        joint->~btGeneric6DofSpring2Constraint();

        new (joint.get()) btGeneric6DofSpring2Constraint(
            *rbA, *rbB, linkOffsetTransform, linkPivotOffsetTransform, rotOrder
        );

        if (linkBody)
        {
            pWorld->addConstraint(joint.get(), true);
        }
    }

    if(structureDirty || propertiesDirty)
    {
        // Lowerlimit == Upperlimit -> axis is locked.
        // Lowerlimit > Upperlimit  -> axis is free
        // Lowerlimit < Upperlimit  -> axis it limited in that range
        Vec3 linLower;
        Vec3 linUpper;
        Vec3 angLower;
        Vec3 angUpper;

        if(linearJointMotion[(int)Axis::X] == JointMotion::Locked) {
            linLower.x = 0;
            linUpper.x = 0;
        }
        else if(linearJointMotion[(int)Axis::X] == JointMotion::Free) {
            linLower.x = 1;
            linUpper.x = 0;
        }
        else if(linearJointMotion[(int)Axis::X] == JointMotion::Limited) {
            linLower.x = linearLowerLimit.x;
            linUpper.x = linearUpperLimit.x;
        }

        if (linearJointMotion[(int)Axis::Y] == JointMotion::Locked) {
            linLower.y = 0;
            linUpper.y = 0;
        }
        else if (linearJointMotion[(int)Axis::Y] == JointMotion::Free) {
            linLower.y = 1;
            linUpper.y = 0;
        }
        else if (linearJointMotion[(int)Axis::Y] == JointMotion::Limited) {
            linLower.y = linearLowerLimit.y;
            linUpper.y = linearUpperLimit.y;
        }

        if (linearJointMotion[(int)Axis::Z] == JointMotion::Locked) {
            linLower.z = 0;
            linUpper.z = 0;
        }
        else if (linearJointMotion[(int)Axis::Z] == JointMotion::Free) {
            linLower.z = 1;
            linUpper.z = 0;
        }
        else if (linearJointMotion[(int)Axis::Z] == JointMotion::Limited) {
            linLower.z = linearLowerLimit.z;
            linUpper.z = linearUpperLimit.z;
        }
        
        if (angularJointMotion[(int)Axis::X] == JointMotion::Locked) {
            angLower.x = 0;
            angUpper.x = 0; 
        }
        else if (angularJointMotion[(int)Axis::X] == JointMotion::Free) {
            angLower.x = 1;
            angUpper.x = 0;
        }
        else if (angularJointMotion[(int)Axis::X] == JointMotion::Limited) {
            angLower.x = angularLowerLimit.x;
            angUpper.x = angularUpperLimit.x;
        }

        if (angularJointMotion[(int)Axis::Y] == JointMotion::Locked) {
            angLower.y = 0;
            angUpper.y = 0;
        }
        else if (angularJointMotion[(int)Axis::Y] == JointMotion::Free) {
            angLower.y = 1;
            angUpper.y = 0;
        }
        else if (angularJointMotion[(int)Axis::Y] == JointMotion::Limited) {
            angLower.y = angularLowerLimit.y;
            angUpper.y = angularUpperLimit.y;
        }

        if (angularJointMotion[(int)Axis::Z] == JointMotion::Locked) {
            angLower.z = 0;
            angUpper.z = 0;
        }
        else if (angularJointMotion[(int)Axis::Z] == JointMotion::Free) {
            angLower.z = 1;
            angUpper.z = 0;
        }
        else if (angularJointMotion[(int)Axis::Z] == JointMotion::Limited) {
            angLower.z = angularLowerLimit.z;
            angUpper.z = angularUpperLimit.z;
        }

        joint->setLinearLowerLimit(Bullet::FromVec3(linLower));
        joint->setLinearUpperLimit(Bullet::FromVec3(linUpper));
        joint->setAngularLowerLimit(Bullet::FromVec3(angLower));
        joint->setAngularUpperLimit(Bullet::FromVec3(angUpper));
        joint->enableMotor(0, linearMotorEnabled[(int)Axis::X]);
        joint->enableMotor(1, linearMotorEnabled[(int)Axis::Y]);
        joint->enableMotor(2, linearMotorEnabled[(int)Axis::Z]);
        joint->enableMotor(3, angularMotorEnabled[(int)Axis::X]);
        joint->enableMotor(4, angularMotorEnabled[(int)Axis::Y]);
        joint->enableMotor(5, angularMotorEnabled[(int)Axis::Z]);
        joint->setTargetVelocity(0, linearMotorTargetVelocity[(int)Axis::X]);
        joint->setTargetVelocity(1, linearMotorTargetVelocity[(int)Axis::Y]);
        joint->setTargetVelocity(2, linearMotorTargetVelocity[(int)Axis::Z]);
        joint->setTargetVelocity(3, angularMotorTargetVelocity[(int)Axis::X]);
        joint->setTargetVelocity(4, angularMotorTargetVelocity[(int)Axis::Y]);
        joint->setTargetVelocity(5, angularMotorTargetVelocity[(int)Axis::Z]);
        joint->setMaxMotorForce(0, linearMotorMaxForce[(int)Axis::X]);
        joint->setMaxMotorForce(1, linearMotorMaxForce[(int)Axis::Y]);
        joint->setMaxMotorForce(2, linearMotorMaxForce[(int)Axis::Z]);
        joint->setMaxMotorForce(3, angularMotorMaxForce[(int)Axis::X]);
        joint->setMaxMotorForce(4, angularMotorMaxForce[(int)Axis::Y]);
        joint->setMaxMotorForce(5, angularMotorMaxForce[(int)Axis::Z]);
        joint->setRotationOrder(rotOrder);
    }

    structureDirty = false;
    propertiesDirty = false;
}

void D6Joint::OnStructureChanged()
{
    structureDirty = true;
}

void D6Joint::OnAttachedToScene()
{
    
}

void D6Joint::OnDetachFromScene()
{
    auto pWorld = GetNode()->GetScene()->GetPhysics()->world.get();
    if(pWorld)
        pWorld->removeConstraint(joint.get());
}

void to_json(json& obj, const JointMotion& jointMotion)
{
    static std::unordered_map<JointMotion, std::string> names {
        { JointMotion::Locked, "Locked" },
        { JointMotion::Limited, "Limited" },
        { JointMotion::Free, "Free" }
    };

    obj = names[jointMotion];
}

void from_json(const json& obj, JointMotion& jointMotion)
{
    static std::unordered_map<std::string, JointMotion> types {
        { "Locked", JointMotion::Locked },
        { "Limited", JointMotion::Limited },
        { "Free", JointMotion::Free }
    };

    auto val = obj.get<std::string>();
    jointMotion = types[val];
}

void to_json(json& obj, const RotationOrder& order)
{
    static std::unordered_map<RotationOrder, std::string> names {
        { RotationOrder::XYZ, "XYZ" },
        { RotationOrder::XZY, "XZY" },
        { RotationOrder::YXZ, "YXZ" },
        { RotationOrder::YZX, "YZX" },
        { RotationOrder::ZXY, "ZXY" },
        { RotationOrder::ZYX, "ZYX" }
    };

    obj = names[order];
}

void from_json(const json& obj, RotationOrder& order)
{
    static std::unordered_map<std::string, RotationOrder> types {
        { "XYZ", RotationOrder::XYZ },
        { "XZY", RotationOrder::XZY },
        { "YXZ", RotationOrder::YXZ },
        { "YZX", RotationOrder::YZX },
        { "ZXY", RotationOrder::ZXY },
        { "ZYX", RotationOrder::ZYX }
    };

    auto val = obj.get<std::string>();
    order = types[val];
}

} // scene
} // mw
