/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.TextView;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.RenderQueue;
import Microwave.Graphics.Shader;
import Microwave.Graphics.Types;
import Microwave.SceneGraph.Node;
import Microwave.SceneGraph.Components.Canvas;
import Microwave.System.App;
import <span>;

namespace mw {
inline namespace scene {

void TextView::ToJson(json& obj) const
{
    View::ToJson(obj);

    ObjectLinker::SaveAsset(obj, "font", font);
    obj["fontSize"] = fontSize;
    obj["wrapping"] = wrapping;
    obj["alignment"] = alignment;
    obj["lineSpacing"] = lineSpacing;
    obj["text"] = text;
}

void TextView::FromJson(const json& obj, ObjectLinker* linker)
{
    View::FromJson(obj, linker);

    ObjectLinker::RestoreAsset(linker, SharedFrom(this), font, obj, "font");

    fontSize = obj.value("fontSize", fontSize);
    wrapping = obj.value("wrapping", wrapping);
    alignment = obj.value("alignment", alignment);
    lineSpacing = obj.value("lineSpacing", lineSpacing);
    text = obj.value("text", text);

    textDirty = true;
}

void TextView::Construct()
{
    vertexBuffer = spnew<Buffer>(
        BufferType::Vertex, BufferUsage::Dynamic,
        BufferCPUAccess::WriteOnly, sizeof(UIVertex) * 4);

    auto assetLib = App::Get()->GetAssetLibrary();
    mat = spnew<Material>();
    mat->shader = assetLib->GetAsset<Shader>(".internal/ui-default.cg");
    mat->depthTest = DepthTest::Always;
    mat->depthWriteEnabled = false;
    mat->renderQueue = RenderQueue::Overlay;
    mat->SetBlendFactors(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);
}

void TextView::SetFont(const sptr<Font>& font)
{
    this->font = font;
    textDirty = true;
}

const sptr<Font>& TextView::GetFont() const {
    return font;
}

void TextView::SetFontSize(int fontSize)
{
    this->fontSize = fontSize;
    textDirty = true;
}

int TextView::GetFontSize() const {
    return fontSize;
}

void TextView::SetWrapping(bool enabled)
{
    wrapping = enabled;
    textDirty = true;
}

bool TextView::GetWrapping() const {
    return wrapping;
}

void TextView::SetAlignment(TextAlign align)
{
    alignment = align;
    textDirty = true;
}

TextAlign TextView::GetAlignment() const {
    return alignment;
}

void TextView::SetLineSpacing(int spacing)
{
    lineSpacing = spacing;
    textDirty = true;
}

int TextView::GetLineSpacing() const {
    return lineSpacing;
}

void TextView::SetText(const std::string& text)
{
    this->text = text;
    textDirty = true;
}

std::string_view TextView::GetText() const {
    return text;
}

void TextView::UpdateText()
{
    if (textDirty)
    {
        font->SetPixelHeight(fontSize);
        
        for (auto c : text)
            font->AddCharacter((char8_t)c);

        std::vector<UIVertex> verts;
        auto size = GetSize();
        
        font->GetTextGeometry(
            text,
            IVec2(math::RoundToInt(size.x), math::RoundToInt(size.y)),
            alignment,
            lineSpacing,
            wrapping,
            verts,
            vertexRanges);

        vertexBuffer = spnew<Buffer>(
            BufferType::Vertex, BufferUsage::Dynamic,
            BufferCPUAccess::WriteOnly,
            std::as_writable_bytes(std::span(verts.data(), verts.size())));

        textDirty = false;
    }
}

void TextView::OnSizeChanged()
{
    textDirty = true;
}

void TextView::GetRenderables(Sink<sptr<Renderable>> sink)
{
    if (!font || text.empty())
        return;

    auto sz = GetSize();
    float hw = sz.x * 0.5f;
    float hh = sz.y * 0.5f;

    sptr<Node> node = GetNode();
    Mat4 mtxTrans = Mat4::Translation(-hw, -hh, 0);
    Mat4 mtxModel = mtxTrans * node->GetLocalToWorldMatrix();

    AABox bounds = AABox(Vec3(0, 0, 0), Vec3(hw, hh, 0));
    bounds = bounds.Transform(mtxModel);

    UpdateText();

    renderables.resize(vertexRanges.size());

    for (size_t i = 0; i != vertexRanges.size(); ++i)
    {
        if (i == font->atlases.size())
            break;

        auto& rng = vertexRanges[i];

        if (!renderables[i])
            renderables[i] = spnew<Renderable>();

        sptr<Renderable> renderable = renderables[i];

        renderable->vertexMapping = {
            { Semantic::POSITION, 0, vertexBuffer, 0, sizeof(UIVertex) },
            { Semantic::TEXCOORD, 0, vertexBuffer, 12, sizeof(UIVertex) },
            { Semantic::COLOR, 0, vertexBuffer, 20, sizeof(UIVertex) }
        };

        renderable->queueOverride = RenderQueue::Overlay + renderDepth;
        renderable->layerMask = node->GetLayerMask();
        renderable->material = mat;
        renderable->mtxModel = mtxModel;
        renderable->bounds = bounds;
        renderable->extra.SetUniform("uDiffuseTex", font->atlases[i].texture);
        renderable->indexBuffer = nullptr;
        renderable->drawStart = rng.x;
        renderable->drawCount = rng.y;
        renderable->drawMode = DrawMode::Triangles;

        sink.Add(renderable);
    }
}

} // scene
} // mw
