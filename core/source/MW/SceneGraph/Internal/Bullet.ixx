/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Internal.Bullet;
import Microwave.Math;
import Microwave.SceneGraph.PhysicsWorld;
import Microwave.SceneGraph.Components.RigidBody;
import Microwave.SceneGraph.Components.Collider;
import <MW/SceneGraph/Internal/Bullet.h>;

export namespace mw {
inline namespace scene {

class Bullet
{
public:
    static btVector3 FromVec3(const Vec3& v) {
        return btVector3(v.x, v.y, v.z);
    }

    static Vec3 ToVec3(const btVector3& v) {
        return Vec3(v.x(), v.y(), v.z());
    }

    static btQuaternion FromQuat(const Quat& q) {
        return btQuaternion(q.v.x, q.v.y, q.v.z, q.w);
    }

    static Quat ToQuat(const btQuaternion& q) {
        return Quat(q.x(), q.y(), q.z(), q.w());
    }

    static btTransform FromMat4(const Mat4& mtx)
    {
        btTransform transform;
        transform.setFromOpenGLMatrix(&mtx.m11);
        return transform;
    }

    static Mat4 ToMat4(const btTransform& transform)
    {
        Mat4 ret;
        transform.getOpenGLMatrix(&ret.m11);
        return ret;
    }
};

} // scene
} // mw
