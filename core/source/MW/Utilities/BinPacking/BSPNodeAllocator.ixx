/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Utilities.BinPacking.BSPNodeAllocator;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.Utilities.BinPacking.BSPNode;
import <vector>;
import <cstdint>;

export namespace mw {
inline namespace utilities {
inline namespace binpacking {

class BSPNodeAllocator : public Object
{
    std::vector<BSPNode*> pool;

    static BSPNode* AllocateNode();
    static void DeallocateNode(BSPNode* pNode);

    friend struct BSPNodeDeleter;
public:
    BSPNodeAllocator(uint32_t initialCapacity = 0);
    ~BSPNodeAllocator();

    BSPNodePtr GetNode();
    static void ReturnNode(BSPNode* pNode);
};

} // binpacking
} // utilities
} // mw
