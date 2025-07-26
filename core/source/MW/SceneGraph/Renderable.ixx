/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module;
#include <MW/System/Internal/PlatformHeaders.h>

export module Microwave.SceneGraph.Renderable;
import Microwave.Graphics.Color;
import Microwave.Graphics.Buffer;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.RenderQueue;
import Microwave.Graphics.Material;
import Microwave.Graphics.MaterialPropertyBlock;
import Microwave.Graphics.Shader;
import Microwave.Math;
import Microwave.SceneGraph.LayerMask;
import Microwave.System.Pointers;
import std;

export namespace mw {
inline namespace scene {

class DirectionalLight;
class Camera;

class Renderable
{
public:
    std::uint64_t sortKey = 0; // set by SceneRenderer

    std::uint32_t queueOverride = 0; // optional
    LayerMask layerMask = {};
    gptr<Material> material;
    Mat4 mtxModel;
    AABox bounds;
    MaterialPropertyBlock extra;
    VertexMapping vertexMapping;
    gptr<Buffer> indexBuffer;
    std::size_t drawStart = {};
    std::size_t drawCount = {};
    DrawMode drawMode = DrawMode::Triangles;

    static std::uint64_t MakeSortKey(std::uint32_t renderQueue, float depth)
    {
        constexpr std::uint32_t maxDepth = std::numeric_limits<std::uint32_t>::max();

        std::uint64_t uQueue = (std::uint64_t)renderQueue;
        std::uint64_t uDepth = (std::uint64_t)((double)depth * maxDepth);

        if (uQueue >= RenderQueue::Transparent)
            uDepth = maxDepth - uDepth;

        return (uQueue << 32) | uDepth;
    }
};

} // scene
} // mw
