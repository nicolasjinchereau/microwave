/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Utilities.BinPacking.BSPNode;
import Microwave.System.Exception;
import Microwave.Utilities.BinPacking.BSPNodeAllocator;
import <MW/System/Debug.h>;

namespace mw {
inline namespace utilities {
inline namespace binpacking {

BSPNode::BSPNode(const wgptr<IBSPNodePool>& nodePool)
    : nodePool(nodePool) {}

void BSPNode::Reset(const IntRect& rc)
{
    rect = rc;
    pMapping = nullptr;
    type = BSPNodeType::Empty;
}

BSPNode* BSPNode::Insert(RectMapping& mapping, int padding, bool allowRotation)
{
    if (type == BSPNodeType::Empty)
    {
        if (mapping.inputSize.x == rect.w &&
            mapping.inputSize.y == rect.h)
        {
            pMapping = &mapping;
            pMapping->mappedRect = IntRect(rect.x, rect.y, mapping.inputSize.x, mapping.inputSize.y);
            pMapping->rotated = false;
            type = BSPNodeType::Leaf;
            return this;
        }
        else if (allowRotation &&
            mapping.inputSize.x == rect.h &&
            mapping.inputSize.y == rect.w)
        {
            pMapping = &mapping;
            pMapping->mappedRect = IntRect(rect.x, rect.y, mapping.inputSize.y, mapping.inputSize.x);
            pMapping->rotated = true;
            type = BSPNodeType::Leaf;
            return this;
        }
        else if (mapping.inputSize.x <= rect.w &&
            mapping.inputSize.y <= rect.h)
        {
            pMapping = &mapping;
            pMapping->mappedRect = IntRect(rect.x, rect.y, mapping.inputSize.x, mapping.inputSize.y);
            pMapping->rotated = false;
            SplitBranch(padding);
            type = BSPNodeType::Branch;
            return this;
        }
        else if (allowRotation &&
            mapping.inputSize.x <= rect.h &&
            mapping.inputSize.y <= rect.w)
        {
            pMapping = &mapping;
            pMapping->mappedRect = IntRect(rect.x, rect.y, mapping.inputSize.y, mapping.inputSize.x);
            pMapping->rotated = true;
            SplitBranch(padding);
            type = BSPNodeType::Branch;
            return this;
        }
    }
    else if (type == BSPNodeType::Branch)
    {
        auto ret = left->Insert(mapping, padding, allowRotation);
        if (!ret) ret = right->Insert(mapping, padding, allowRotation);
        return ret;
    }

    return nullptr;
}

void BSPNode::SplitBranch(int padding)
{
    auto pool = nodePool.lock();
    Assert(pool);

    if (!left) left = pool->GetNode();
    if (!right) right = pool->GetNode();

    int remWidth = rect.w - pMapping->mappedRect.w;
    int remHeight = rect.h - pMapping->mappedRect.h;

    if (remWidth > remHeight)
    {
        // split vertically
        left->rect = IntRect(rect.x, rect.y + pMapping->mappedRect.h, pMapping->mappedRect.w, remHeight);
        left->type = BSPNodeType::Empty;
        left->pMapping = nullptr;
        right->rect = IntRect(rect.x + pMapping->mappedRect.w, rect.y, remWidth, rect.h);
        right->type = BSPNodeType::Empty;
        right->pMapping = nullptr;

        left->rect.y += padding;
        left->rect.h -= padding;
        right->rect.x += padding;
        right->rect.w -= padding;
    }
    else
    {
        // split horizontally
        left->rect = IntRect(rect.x + pMapping->mappedRect.w, rect.y, remWidth, pMapping->mappedRect.h);
        left->type = BSPNodeType::Empty;
        left->pMapping = nullptr;
        right->rect = IntRect(rect.x, rect.y + pMapping->mappedRect.h, rect.w, remHeight);
        right->type = BSPNodeType::Empty;
        right->pMapping = nullptr;

        left->rect.x += padding;
        left->rect.w -= padding;
        right->rect.y += padding;
        right->rect.h -= padding;
    }
}

} // binpacking
} // utilities
} // mw
