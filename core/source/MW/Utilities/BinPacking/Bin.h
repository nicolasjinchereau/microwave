/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <vector>
#include <list>
#include <MW/Math/Math.h>
#include <MW/Utilities/BinPacking/BSPNode.h>
#include <MW/Utilities/BinPacking/BSPNodeAllocator.h>
#include <MW/Utilities/BinPacking/RectMapping.h>

namespace mw {
inline namespace utilities {
inline namespace binpacking {

struct Bin
{
    IVec2 size;
    BSPNodePtr root;
    std::list<RectMapping> mappings;

    Bin() {}
    Bin(const IVec2& size) : size(size) {}

    Bin(Bin&& bin) noexcept
        : size(bin.size), root(std::move(bin.root)), mappings(move(bin.mappings))
    {
        bin.size = IVec2();
    }

    Bin& operator=(Bin&& bin) noexcept
    {
        size = bin.size;
        bin.size = IVec2();
        root = std::move(bin.root);
        mappings = move(bin.mappings);
        return *this;
    }

    Bin(const Bin&) = default;
    Bin& operator=(const Bin&) = default;
};

} // binpacking
} // utilities
} // mw
