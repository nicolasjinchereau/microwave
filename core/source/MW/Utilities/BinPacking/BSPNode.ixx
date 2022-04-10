/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Utilities.BinPacking.BSPNode;
import Microwave.Math;
import Microwave.System.Pointers;
import Microwave.Utilities.BinPacking.RectMapping;

export namespace mw {
inline namespace utilities {
inline namespace binpacking {

class BSPNode;
class BSPNodeAllocator;

enum class BSPNodeType
{
    Empty,   // no contents, no children
    Branch,  // has contents, has children
    Leaf     // has contents, no children
};

struct BSPNodeDeleter { void operator()(BSPNode* n); };
typedef uptr<BSPNode, BSPNodeDeleter> BSPNodePtr;

class BSPNode
{
public:
    wptr<BSPNodeAllocator> allocator;
    IntRect rect = IntRect();
    BSPNodeType type = BSPNodeType::Empty;
    RectMapping* pMapping = nullptr;
    BSPNodePtr left;
    BSPNodePtr right;

    BSPNode(const wptr<BSPNodeAllocator>& allocator);

    void Reset(const IntRect& rc);
    BSPNode* Insert(RectMapping& mapping, int padding, bool allowRotation);
    void SplitBranch(int padding);
};

} // binpacking
} // utilities
} // mw
