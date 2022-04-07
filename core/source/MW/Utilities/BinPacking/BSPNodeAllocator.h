/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <vector>
#include <memory>
#include <MW/System/Object.h>

namespace mw {
inline namespace utilities {
inline namespace binpacking {

class BSPNode;

struct BSPNodeDeleter {
    void operator()(BSPNode* n);
};

typedef std::unique_ptr<BSPNode, BSPNodeDeleter> BSPNodePtr;

class BSPNodeAllocator : public Object
{
    std::vector<BSPNode*> nodes;

    void ReturnNode(BSPNode* node);
    static void DeleteNode(BSPNode* node);

    friend struct BSPNodeDeleter;
public:
    BSPNodeAllocator(uint32_t initialCapacity = 0);
    ~BSPNodeAllocator();

    BSPNodePtr GetNode();
};

} // binpacking
} // utilities
} // mw
