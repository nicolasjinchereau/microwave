/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.AABox;
import Microwave.Math.Vec3;
import Microwave.Math.Vec4;
import Microwave.Math.Mat4;
import Microwave.Math.Operators;
import <array>;
import <utility>;

export namespace mw {
inline namespace math {

class AABox
{
public:
    Vec3 center;
    Vec3 extents;

    AABox() { }

    AABox(const Vec3& center, const Vec3& extents)
        : center(center), extents(extents) { }

    void GetCorners(std::array<Vec3, 8>& verts) const
    {
        auto vmin = GetMin();
        auto vmax = GetMax();

        // bottom square
        verts[0] = Vec3(vmin.x, vmin.y, vmin.z);
        verts[1] = Vec3(vmin.x, vmin.y, vmax.z);
        verts[2] = Vec3(vmax.x, vmin.y, vmax.z);
        verts[3] = Vec3(vmax.x, vmin.y, vmin.z);

        // top square
        verts[4] = Vec3(vmin.x, vmax.y, vmin.z);
        verts[5] = Vec3(vmin.x, vmax.y, vmax.z);
        verts[6] = Vec3(vmax.x, vmax.y, vmax.z);
        verts[7] = Vec3(vmax.x, vmax.y, vmin.z);
    }

    void SetCenter(const Vec3& center) {
        this->center = center;
    }

    Vec3 GetCenter() const {
        return center;
    }

    void SetExtents(const Vec3& extents) {
        this->extents = extents;
    }

    Vec3 GetExtents() const {
        return extents;
    }

    void SetSize(const Vec3& size) {
        this->extents = size * 0.5f;
    }

    Vec3 GetSize() const {
        return extents * 2;
    }

    std::pair<Vec3, Vec3> GetMinMax() const {
        return { GetMin(), GetMax() };
    }

    void SetMinMax(Vec3 min, Vec3 max)
    {
        extents = (max - min) * 0.5f;
        center = min + extents;
    }

    void SetMin(const Vec3& min) {
        SetMinMax(min, GetMax());
    }

    Vec3 GetMin() const {
        return center - extents;
    }

    void SetMax(const Vec3& max) {
        SetMinMax(GetMin(), max);
    }

    Vec3 GetMax() const {
        return center + extents;
    }

    void Union(const Vec3& point)
    {
        Vec3 min = GetMin();
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        min.z = std::min(min.z, point.z);

        Vec3 max = GetMax();
        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
        max.z = std::max(max.z, point.z);

        SetMinMax(min, max);
    }

    void Union(const AABox& box)
    {
        Union(GetMin());
        Union(GetMax());
    }

    bool Intersects(const AABox& box) const
    {
        Vec3 aMin = GetMin();
        Vec3 aMax = GetMax();
        Vec3 bMin = box.GetMin();
        Vec3 bMax = box.GetMax();

        return
            (aMin.x <= bMax.x) && (aMax.x >= bMin.x) &&
            (aMin.y <= bMax.y) && (aMax.y >= bMin.y) &&
            (aMin.z <= bMax.z) && (aMax.z >= bMin.z);
    }

    AABox Transform(const Mat4& mtx) const
    {
        Vec3 worldCenter = Vec4(center, 1.0f) * mtx;
        Vec3 worldExtentsX = Vec4(extents.x, 0, 0, 0) * mtx;
        Vec3 worldExtentsY = Vec4(0, extents.y, 0, 0) * mtx;
        Vec3 worldExtentsZ = Vec4(0, 0, extents.z, 0) * mtx;

        AABox ret;
        ret.center = worldCenter;
        ret.extents.x = abs(worldExtentsX.x) + abs(worldExtentsY.x) + abs(worldExtentsZ.x);
        ret.extents.y = abs(worldExtentsX.y) + abs(worldExtentsY.y) + abs(worldExtentsZ.y);
        ret.extents.z = abs(worldExtentsX.z) + abs(worldExtentsY.z) + abs(worldExtentsZ.z);
        return ret;
    }
};

} // math
} // mw
