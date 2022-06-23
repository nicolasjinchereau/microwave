/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Utilities.BinPacking.BinPacker;
import Microwave.Math;
import Microwave.System.Pointers;
import Microwave.Utilities.BinPacking.Bin;
import Microwave.Utilities.BinPacking.BSPNode;
import Microwave.Utilities.BinPacking.BSPNodeAllocator;
import Microwave.Utilities.BinPacking.RectMapping;
import <algorithm>;
import <array>;
import <list>;
import <vector>;

export namespace mw {
inline namespace utilities {
inline namespace binpacking {

class BinPacker
{
    static bool IsAreaDescending(const RectMapping& a, const RectMapping& b) {
        return IntRect(a.inputSize).GetArea() > IntRect(b.inputSize).GetArea();
    }

    static bool IsPerimeterDescending(const RectMapping& a, const RectMapping& b) {
        return IntRect(a.inputSize).GetPerimeter() > IntRect(b.inputSize).GetPerimeter();
    }

    static bool IsMaxSideLengthDescending(const RectMapping& a, const RectMapping& b) {
        return std::max(a.inputSize.x, a.inputSize.y) > std::max(b.inputSize.x, b.inputSize.y);
    }

    static bool IsMaxWidthDescending(const RectMapping& a, const RectMapping& b) {
        return a.inputSize.x > b.inputSize.x;
    }

    static bool IsMaxHeightDescending(const RectMapping& a, const RectMapping& b) {
        return a.inputSize.y > b.inputSize.y;
    }

    typedef bool (*BinComparison)(const RectMapping& a, const RectMapping& b);

    constexpr static int NumBinComparison = 5;

    std::array<BinComparison, NumBinComparison> binComparisons{
        IsAreaDescending,
        IsPerimeterDescending,
        IsMaxSideLengthDescending,
        IsMaxWidthDescending,
        IsMaxHeightDescending
    };

    std::array<std::vector<RectMapping>, NumBinComparison> sortedInput;
    std::vector<RectMapping> input;
    std::vector<RectMapping> overflow;
    std::vector<IVec2> binSizes;
    gptr<BSPNodeAllocator> nodeAllocator = gpnew<BSPNodeAllocator>();
    std::vector<Bin> bins;

    bool dynamicPacking = false;
    IVec2 binSize;
    int boxPadding = 0;
    bool allowRotation = true;

    Bin PackBin(
        std::vector<RectMapping>& input,
        const std::vector<IVec2>& binSizes,
        int padding, bool allowRotation,
        std::vector<RectMapping>& overflow);

public:
    void PackBoxes(
        const std::vector<IVec2>& boxes,
        int maxSize,
        int padding,
        bool allowRotation = true);

    void StartDynamicPacking(const IVec2& binSize, int boxPadding, bool allowRotation);
    RectMapping PackBox(const IVec2& box);

    const std::vector<Bin>& GetBins() const {
        return bins;
    }
};

} // binpacking
} // utilities
} // mw
