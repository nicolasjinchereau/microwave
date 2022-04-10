/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Internal.CapsuleShape;
import Microwave.SceneGraph.Axis;
import <array>;
import <MW/SceneGraph/Internal/Bullet.h>;

export namespace mw {
inline namespace scene {

class CapsuleShape : public btCapsuleShape
{
public:
    CapsuleShape(Axis upAxis, btScalar radius, btScalar height)
        : btCapsuleShape()
    {
        m_upAxis = (int)upAxis;
        m_collisionMargin = radius;

        if (upAxis == Axis::X)
            m_implicitShapeDimensions.setValue(0.5f * height, radius, radius);
        else if (upAxis == Axis::Y)
            m_implicitShapeDimensions.setValue(radius, 0.5f * height, radius);
        else if (upAxis == Axis::Z)
            m_implicitShapeDimensions.setValue(radius, radius, 0.5f * height);
    }

    virtual const char* getName() const
    {
        static std::array<const char*, 3> names{
            "CapsuleX",
            "CapsuleShape",
            "CapsuleZ"
        };
        return names[m_upAxis];
    }
};

} // scene
} // mw
