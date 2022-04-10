/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math:AABox_p1;
import :Vec3_p1;
import :Mat4_p1;
import <array>;
import <utility>;

export namespace mw {
inline namespace math {

class AABox
{
public:
    Vec3 center;
    Vec3 extents;

    AABox();
    AABox(const Vec3& center, const Vec3& extents);

    void GetCorners(std::array<Vec3, 8>& verts) const;

    void SetCenter(const Vec3& center);
    Vec3 GetCenter() const;

    void SetExtents(const Vec3& extents);
    Vec3 GetExtents() const;
    
    void SetSize(const Vec3& size);
    Vec3 GetSize() const;

    void SetMinMax(Vec3 min, Vec3 max);
    std::pair<Vec3, Vec3> GetMinMax() const;

    void SetMin(const Vec3& min);
    Vec3 GetMin() const;

    void SetMax(const Vec3& max);
    Vec3 GetMax() const;

    void Union(const Vec3& point);
    void Union(const AABox& box);

    AABox Transform(const Mat4& mtx) const;

    bool Intersects(const AABox& box) const;
};

} // math
} // mw
