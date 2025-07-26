/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Utilities.BinPacking.BSPNodeAllocator;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.Utilities.BinPacking.BSPNode;
import std;

export namespace mw {
inline namespace utilities {
inline namespace binpacking {

class BSPNodeAllocator : public IBSPNodePool
{
    std::vector<BSPNode*> pool;
public:
    BSPNodeAllocator(std::uint32_t initialCapacity = 0);
    ~BSPNodeAllocator();

    virtual BSPNodePtr GetNode() override;
    virtual void ReturnNode(BSPNode* pNode) override;
};

} // binpacking
} // utilities
} // mw
