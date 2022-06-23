/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Utilities.BinPacking.BSPNode;
import Microwave.Math;
import Microwave.System.Exception;
import Microwave.System.Pointers;
import Microwave.System.Object;
import Microwave.Utilities.BinPacking.RectMapping;
import <MW/System/Debug.h>;

export namespace mw {
inline namespace utilities {
inline namespace binpacking {

class BSPNode;

enum class BSPNodeType
{
    Empty,   // no contents, no children
    Branch,  // has contents, has children
    Leaf     // has contents, no children
};

struct BSPNodeDeleter { void operator()(BSPNode* n); };
typedef uptr<BSPNode, BSPNodeDeleter> BSPNodePtr;

class IBSPNodePool : public Object
{
public:
    virtual BSPNodePtr GetNode() = 0;
    virtual void ReturnNode(BSPNode* pNode) = 0;
};

class BSPNode
{
public:
    wgptr<IBSPNodePool> nodePool;
    IntRect rect = IntRect();
    BSPNodeType type = BSPNodeType::Empty;
    RectMapping* pMapping = nullptr;
    BSPNodePtr left;
    BSPNodePtr right;

    BSPNode(const wgptr<IBSPNodePool>& nodePool);

    void Reset(const IntRect& rc);
    BSPNode* Insert(RectMapping& mapping, int padding, bool allowRotation);
    void SplitBranch(int padding);

    static BSPNode* Allocate() {
        return (BSPNode*)::operator new(sizeof(BSPNode));
    }

    static void Deallocate(BSPNode* pNode) {
        Assert(pNode);
        ::operator delete(pNode);
    }
};

void BSPNodeDeleter::operator()(BSPNode* pNode)
{
    Assert(pNode);

    if (auto pool = pNode->nodePool.lock()) {
        pool->ReturnNode(pNode);
    }
    else
    {
        pNode->~BSPNode();
        BSPNode::Deallocate(pNode);
    }
}

} // binpacking
} // utilities
} // mw
