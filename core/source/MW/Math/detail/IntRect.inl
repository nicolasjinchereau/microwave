/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/detail/IntRect.h>
#include <MW/Math/detail/IVec2.h>
#include <cmath>

namespace mw {
inline namespace math {

inline IntRect::IntRect()
    : IntRect(0, 0, 0, 0) {}

inline IntRect::IntRect(const IVec2& size)
    : IntRect(0, 0, size.x, size.y)
{

}

inline IntRect::IntRect(int x, int y, int w, int h)
    : x(x), y(y), w(w), h(h) {}

inline bool IntRect::operator==(const IntRect& rc) const
{
    return
        x == rc.x &&
        y == rc.y &&
        w == rc.w &&
        h == rc.h;
}

inline bool IntRect::operator!=(const IntRect& rc) const {
    return !operator==(rc);
}

inline void IntRect::FitInto(const IntRect& rc)
{
    float wx = (float)rc.x;
    float wy = (float)rc.y;
    float ww = (float)rc.w;
    float wh = (float)rc.h;

    float s = 1.0f;

    if (w > h)
    {
        s = wh / (float)h;

        if ((float)w * s > ww)
            s = ww / (float)w;
    }
    else
    {
        s = ww / (float)w;

        if ((float)h * s > wh)
            s = wh / (float)h;
    }

    float sw = (float)w * s;
    float sh = (float)h * s;

    x = (int)(wx + ((sw < ww - 0.5f) ? (ww - sw) * 0.5f : 0));
    y = (int)(ww + ((sh < wh - 0.5f) ? (wh - sh) * 0.5f : 0));
    w = (int)sw;
    h = (int)sh;
}

inline int IntRect::GetArea() const {
    return w * h;
}

inline int IntRect::GetPerimeter() const {
    return (w + h) * 2;
}

inline IVec2 IntRect::GetPos() const {
    return IVec2(x, y);
}

inline IVec2 IntRect::GetSize() const {
    return IVec2(w, h);
}

}
}
