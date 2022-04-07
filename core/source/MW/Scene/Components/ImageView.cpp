/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Scene/Components/ImageView.h>
#include <MW/Scene/Components/Canvas.h>
#include <MW/System/App.h>
#include <MW/System/Json.h>

namespace mw {
inline namespace scene {

void ImageView::ToJson(json& obj) const
{
    View::ToJson(obj);

    obj["border"] = border;
    obj["color"] = color;
    ObjectLinker::SaveAsset(obj, "tex", tex);
}

void ImageView::FromJson(const json& obj, ObjectLinker* linker)
{
    View::FromJson(obj, linker);

    border = obj.value("border", border);
    color = obj.value("color", color);
    ObjectLinker::RestoreAsset(linker, This(), tex, obj, "tex");

    meshDirty = true;
}

ImageView::ImageView()
{
    auto app = App::Get();
    auto assetLib = app->GetAssetLibrary();
    mat = spnew<Material>();
    mat->shader = assetLib->GetAsset<Shader>(".internal/ui-default.cg");
    mat->depthTest = DepthTest::Always;
    mat->depthWriteEnabled = false;
    mat->renderQueue = RenderQueue::Overlay;
    mat->SetBlendFactors(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
}

void ImageView::SetBorder(const Box& box) {
    border = box;
    meshDirty = true;
}

Box ImageView::GetBorder() const {
    return border;
}

void ImageView::SetColor(const Color& color) {
    this->color = color;
    meshDirty = true;
}

Color ImageView::GetColor() const {
    return color;
}

void ImageView::SetTexture(const sptr<Texture>& tex) {
    this->tex = tex;
}

sptr<Texture> ImageView::GetTexture() const {
    return tex;
}

void ImageView::UpdateMesh()
{
    assert(tex);

    if (meshDirty)
    {
        auto graphics = App::Get()->GetGraphics();

        auto sz = GetSize();
        float hw = sz.x * 0.5f;
        float hh = sz.y * 0.5f;

        bool hasBorder =
            abs(border.left) > 0.001f ||
            abs(border.top) > 0.001f ||
            abs(border.right) > 0.001f ||
            abs(border.bottom) > 0.001f;

        if (hasBorder)
        {
            float x1 = -hw;
            float x2 = -hw + border.left;
            float x3 = hw - border.right;
            float x4 = hw;
            float y1 = hh;
            float y2 = hh - border.top;
            float y3 = -hh + border.bottom;
            float y4 = -hh;

            auto texSize = tex->GetSize();
            float u1 = 0.0f;
            float u2 = border.left / texSize.x;
            float u3 = 1.0f - border.right / texSize.x;
            float u4 = 1.0f;
            float v1 = 0.0f;
            float v2 = border.top / texSize.y;
            float v3 = 1.0f - border.bottom / texSize.y;
            float v4 = 1.0f;

            std::array<UIVertex, 16> vertices{
                UIVertex{ Vec3(x1, y1, 0), Vec2(u1, v1), color },
                UIVertex{ Vec3(x2, y1, 0), Vec2(u2, v1), color },
                UIVertex{ Vec3(x3, y1, 0), Vec2(u3, v1), color },
                UIVertex{ Vec3(x4, y1, 0), Vec2(u4, v1), color },
                UIVertex{ Vec3(x1, y2, 0), Vec2(u1, v2), color },
                UIVertex{ Vec3(x2, y2, 0), Vec2(u2, v2), color },
                UIVertex{ Vec3(x3, y2, 0), Vec2(u3, v2), color },
                UIVertex{ Vec3(x4, y2, 0), Vec2(u4, v2), color },
                UIVertex{ Vec3(x1, y3, 0), Vec2(u1, v3), color },
                UIVertex{ Vec3(x2, y3, 0), Vec2(u2, v3), color },
                UIVertex{ Vec3(x3, y3, 0), Vec2(u3, v3), color },
                UIVertex{ Vec3(x4, y3, 0), Vec2(u4, v3), color },
                UIVertex{ Vec3(x1, y4, 0), Vec2(u1, v4), color },
                UIVertex{ Vec3(x2, y4, 0), Vec2(u2, v4), color },
                UIVertex{ Vec3(x3, y4, 0), Vec2(u3, v4), color },
                UIVertex{ Vec3(x4, y4, 0), Vec2(u4, v4), color },
            };

            std::array<int, 54> indices{
                0, 1, 5, 0, 5, 4,
                1, 2, 6, 1, 6, 5,
                2, 3, 7, 2, 7, 6,
                4, 5, 9, 4, 9, 8,
                5, 6, 10, 5, 10, 9,
                6, 7, 11, 6, 11, 10,
                8, 9, 13, 8, 13, 12,
                9, 10, 14, 9, 14, 13,
                10, 11, 15, 10, 15, 14
            };

            size_t vertSize = vertices.size() * sizeof(UIVertex);
            std::byte* vertData = (std::byte*)vertices.data();

            if (vertexBuffer && vertexBuffer->GetCapacity() == vertSize)
            {
                vertexBuffer->UpdateSubData(0, std::span<std::byte>(vertData, vertSize));
            }
            else
            {
                vertexBuffer = graphics->CreateBuffer(
                    vertSize, DrawBufferType::Vertex, true,
                    std::span<std::byte>(vertData, vertSize));
            }

            indexCount = indices.size();
            size_t indexSize = indices.size() * sizeof(int);
            std::byte* indexData = (std::byte*)indices.data();

            if (indexBuffer && indexBuffer->GetCapacity() == indexSize)
            {
                indexBuffer->UpdateSubData(0, std::span<std::byte>(indexData, indexSize));
            }
            else
            {
                indexBuffer = graphics->CreateBuffer(
                    indexSize, DrawBufferType::Index, true,
                    std::span<std::byte>(indexData, indexSize));
            }
        }
        else
        {
            float x1 = -hw;
            float x2 = hw;
            float y1 = hh;
            float y2 = -hh;

            float u1 = 0.0f;
            float u2 = 1.0f;
            float v1 = 0.0f;
            float v2 = 1.0f;

            std::array<UIVertex, 4> vertices {
                UIVertex{ Vec3(x1, y1, 0), Vec2(u1, v1), color },
                UIVertex{ Vec3(x2, y1, 0), Vec2(u2, v1), color },
                UIVertex{ Vec3(x1, y2, 0), Vec2(u1, v2), color },
                UIVertex{ Vec3(x2, y2, 0), Vec2(u2, v2), color }
            };

            std::array<int, 6> indices {
                0, 1, 3,
                0, 3, 2
            };

            size_t vertSize = vertices.size() * sizeof(UIVertex);
            std::byte* vertData = (std::byte*)vertices.data();

            if (vertexBuffer && vertexBuffer->GetCapacity() == vertSize)
            {
                vertexBuffer->UpdateSubData(0, std::span<std::byte>(vertData, vertSize));
            }
            else
            {
                vertexBuffer = graphics->CreateBuffer(
                    vertSize, DrawBufferType::Vertex, true,
                    std::span<std::byte>(vertData, vertSize));
            }

            indexCount = indices.size();
            size_t indexSize = indexCount * sizeof(int);
            std::byte* indexData = (std::byte*)indices.data();

            if (indexBuffer && indexBuffer->GetCapacity() == indexSize)
            {
                indexBuffer->UpdateSubData(0, std::span<std::byte>(indexData, indexSize));
            }
            else
            {
                indexBuffer = graphics->CreateBuffer(
                    indexSize, DrawBufferType::Index, true,
                    std::span<std::byte>(indexData, indexSize));
            }
        }

        meshDirty = false;
    }
}

void ImageView::OnSizeChanged()
{
    meshDirty = true;
}

void ImageView::GetRenderables(Sink<sptr<Renderable>> sink)
{
    if (!tex)
        return;

    auto sz = GetSize();
    float hw = sz.x * 0.5f;
    float hh = sz.y * 0.5f;

    sptr<Node> node = GetNode();
    Mat4 mtxModel = node->GetLocalToWorldMatrix();

    AABox bounds = AABox(Vec3(0, 0, 0), Vec3(hw, hh, 0));
    bounds = bounds.Transform(mtxModel);

    UpdateMesh();

    if (!renderable)
        renderable = spnew<Renderable>();

    renderable->queueOverride = RenderQueue::Overlay + renderDepth;
    renderable->layerMask = node->GetLayerMask();
    renderable->material = mat;
    renderable->mtxModel = mtxModel;
    renderable->bounds = bounds;
    renderable->extra.SetUniform("uDiffuseTex", tex);
    renderable->vertexBuffer = vertexBuffer;
    renderable->indexBuffer = indexBuffer;
    renderable->drawStart = 0;
    renderable->drawCount = indexCount;
    renderable->drawMode = DrawMode::Triangles;

    sink.Add(renderable);
}

} // scene
} // mw
