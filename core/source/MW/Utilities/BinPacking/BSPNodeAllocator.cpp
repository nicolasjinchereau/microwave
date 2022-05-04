/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Utilities.BinPacking.BSPNodeAllocator;
import <cassert>;

namespace mw {
inline namespace utilities {
inline namespace binpacking {

BSPNodeAllocator::BSPNodeAllocator(uint32_t initialCapacity)
{
    pool.reserve(initialCapacity);

    for (uint32_t i = 0; i < initialCapacity; ++i)
        pool.push_back( AllocateNode() );
}

BSPNodeAllocator::~BSPNodeAllocator()
{
    for (auto& mem : pool)
        DeallocateNode(mem);

    pool.clear();
}

BSPNode* BSPNodeAllocator::AllocateNode() {
    return (BSPNode*)::operator new(sizeof(BSPNode));
}

void BSPNodeAllocator::DeallocateNode(BSPNode* pNode) {
    assert(pNode);
    ::operator delete(pNode);
}

BSPNodePtr BSPNodeAllocator::GetNode()
{
    BSPNode* mem = nullptr;

    if (!pool.empty()) {
        mem = pool.back();
        pool.pop_back();
    }
    else {
        mem = AllocateNode();
    }

    auto pNode = new (mem) BSPNode(SharedFrom(this));

    return BSPNodePtr(pNode);
}

void BSPNodeAllocator::ReturnNode(BSPNode* pNode)
{
    assert(pNode);
    
    pNode->~BSPNode();

    if (auto allocator = pNode->allocator.lock()) {
        allocator->pool.push_back(pNode);
    }
    else {
        DeallocateNode(pNode);
    }
}

} // binpacking
} // utilities
} // mw
