/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Utilities/BinPacking/BSPNodeAllocator.h>
#include <MW/Utilities/BinPacking/BSPNode.h>

namespace mw {
inline namespace utilities {
inline namespace binpacking {

void BSPNodeDeleter::operator()(BSPNode* n)
{
    auto alloc = n->allocator.lock();
    if (alloc) {
        alloc->ReturnNode(n);
    }
    else {
        BSPNodeAllocator::DeleteNode(n);
    }
}

BSPNodeAllocator::BSPNodeAllocator(uint32_t initialCapacity)
{
    nodes.reserve(initialCapacity);

    for (uint32_t i = 0; i < initialCapacity; ++i)
        nodes.push_back((BSPNode*)::operator new(sizeof(BSPNode)));
}

BSPNodeAllocator::~BSPNodeAllocator()
{
    for (auto& node : nodes)
        DeleteNode((BSPNode*)node);
}

BSPNodePtr BSPNodeAllocator::GetNode()
{
    BSPNode* node = nullptr;

    if (!nodes.empty()) {
        node = nodes.back();
        nodes.pop_back();
    }
    else {
        node = (BSPNode*)::operator new(sizeof(BSPNode));
    }

    return BSPNodePtr(new (node) BSPNode(This<BSPNodeAllocator>()));
}

void BSPNodeAllocator::ReturnNode(BSPNode* node) {
    nodes.push_back(node);
}

void BSPNodeAllocator::DeleteNode(BSPNode* node) {
    ::operator delete(node);
}

} // binpacking
} // utilities
} // mw
