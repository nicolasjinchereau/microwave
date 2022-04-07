/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once

namespace mw {
inline namespace math {

class IVec2;

class IntRect
{
public:
    int x;
    int y;
    int w;
    int h;

    IntRect();
    IntRect(const IVec2& size);
    IntRect(int x, int y, int w, int h);

    bool operator==(const IntRect& rc) const;
    bool operator!=(const IntRect& rc) const;

    void FitInto(const IntRect& rc);
    int GetArea() const;
    int GetPerimeter() const;
    IVec2 GetPos() const;
    IVec2 GetSize() const;
};

}
}
