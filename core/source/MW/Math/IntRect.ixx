/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Math.IntRect;
import Microwave.Math.IVec2;
import <cmath>;

export namespace mw {
inline namespace math {

class IntRect
{
public:
    int x;
    int y;
    int w;
    int h;

    IntRect()
        : IntRect(0, 0, 0, 0) {}

    IntRect(const IVec2& size)
        : IntRect(0, 0, size.x, size.y)
    {

    }

    IntRect(int x, int y, int w, int h)
        : x(x), y(y), w(w), h(h) {}

    bool operator==(const IntRect& rc) const
    {
        return
            x == rc.x &&
            y == rc.y &&
            w == rc.w &&
            h == rc.h;
    }

    bool operator!=(const IntRect& rc) const {
        return !operator==(rc);
    }

    void FitInto(const IntRect& rc)
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

    int GetArea() const {
        return w * h;
    }

    int GetPerimeter() const {
        return (w + h) * 2;
    }

    IVec2 GetPos() const {
        return IVec2(x, y);
    }

    IVec2 GetSize() const {
        return IVec2(w, h);
    }
};

} // math
} // mw
