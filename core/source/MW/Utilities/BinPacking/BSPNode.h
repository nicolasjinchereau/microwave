/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/Math.h>
#include <MW/Utilities/BinPacking/RectMapping.h>
#include <MW/Utilities/BinPacking/BSPNodeAllocator.h>
#include <memory>

namespace mw {
inline namespace utilities {
inline namespace binpacking {

enum class BSPNodeType
{
    Empty,   // no contents, no children
    Branch,  // has contents, has children
    Leaf     // has contents, no children
};

class BSPNode
{
public:
    std::weak_ptr<BSPNodeAllocator> allocator;
    IntRect rect = IntRect();
    BSPNodeType type = BSPNodeType::Empty;
    RectMapping* pMapping = nullptr;
    BSPNodePtr left;
    BSPNodePtr right;

    BSPNode(const std::weak_ptr<BSPNodeAllocator>& allocator);

    void Reset(const IntRect& rc);
    BSPNode* Insert(RectMapping& mapping, int padding, bool allowRotation);
    void SplitBranch(int padding);
};

} // binpacking
} // utilities
} // mw
