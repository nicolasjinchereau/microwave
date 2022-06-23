/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Utilities.BinPacking.BSPNodeAllocator;
import Microwave.System.Exception;
import <MW/System/Debug.h>;

namespace mw {
inline namespace utilities {
inline namespace binpacking {

BSPNodeAllocator::BSPNodeAllocator(uint32_t initialCapacity)
{
    pool.reserve(initialCapacity);

    for (uint32_t i = 0; i < initialCapacity; ++i)
        pool.push_back(BSPNode::Allocate() );
}

BSPNodeAllocator::~BSPNodeAllocator()
{
    for (auto& mem : pool)
        BSPNode::Deallocate(mem);

    pool.clear();
}

BSPNodePtr BSPNodeAllocator::GetNode()
{
    BSPNode* mem = nullptr;

    if (!pool.empty()) {
        mem = pool.back();
        pool.pop_back();
    }
    else {
        mem = BSPNode::Allocate();
    }

    auto pNode = new (mem) BSPNode(self(this));

    return BSPNodePtr(pNode);
}

void BSPNodeAllocator::ReturnNode(BSPNode* pNode)
{
    Assert(pNode);

    pNode->~BSPNode();
    pool.push_back(pNode);
}

} // binpacking
} // utilities
} // mw
