/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Renderable;
import Microwave.Graphics.Color;
import Microwave.Graphics.DrawBuffer;
import Microwave.Graphics.RenderQueue;
import Microwave.Graphics.Material;
import Microwave.Graphics.MaterialPropertyBlock;
import Microwave.Math;
import Microwave.SceneGraph.LayerMask;
import Microwave.System.Pointers;
import <cstdint>;
import <limits>;

export namespace mw {
inline namespace scene {

class DirectionalLight;
class Camera;

class Renderable
{
public:
    uint64_t sortKey = 0; // set by SceneRenderer

    uint32_t queueOverride = 0; // optional
    LayerMask layerMask = {};
    sptr<Material> material;
    Mat4 mtxModel;
    AABox bounds;
    MaterialPropertyBlock extra;
    sptr<DrawBuffer> vertexBuffer;
    sptr<DrawBuffer> normalBuffer;
    sptr<DrawBuffer> texcoordBuffer;
    sptr<DrawBuffer> colorBuffer;
    sptr<DrawBuffer> indexBuffer;
    size_t drawStart = {};
    size_t drawCount = {};
    DrawMode drawMode = DrawMode::Triangles;

    static uint64_t MakeSortKey(uint32_t renderQueue, float depth)
    {
        constexpr uint32_t maxDepth = std::numeric_limits<uint32_t>::max();

        uint64_t uQueue = (uint64_t)renderQueue;
        uint64_t uDepth = (uint64_t)((double)depth * maxDepth);

        if (uQueue >= RenderQueue::Transparent)
            uDepth = maxDepth - uDepth;

        return (uQueue << 32) | uDepth;
    }
};

} // scene
} // mw
