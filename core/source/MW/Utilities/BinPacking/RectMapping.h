/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/Math.h>

namespace mw {
inline namespace utilities {
inline namespace binpacking {

struct RectMapping
{
    int inputIndex = 0;
    IVec2 inputSize;
    IntRect mappedRect;
    bool rotated = false;

    RectMapping() {}
    RectMapping(const IVec2& inputSize, int inputIndex)
        : inputSize(inputSize),
        mappedRect(inputSize),
        inputIndex(inputIndex) {}
};

} // binpacking
} // utilities
} // mw
