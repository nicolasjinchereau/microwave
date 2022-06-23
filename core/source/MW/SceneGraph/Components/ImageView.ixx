/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.ImageView;
import Microwave.Graphics.Color;
import Microwave.Graphics.Buffer;
import Microwave.Graphics.Material;
import Microwave.Graphics.Texture;
import Microwave.Graphics.Types;
import Microwave.Math;
import Microwave.SceneGraph.Components.View;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Renderable;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.Utilities.Sink;
import <cstdint>;

export namespace mw {
inline namespace scene {

class ImageView : public View, public IRenderEvents
{
    inline static Type::Pin<ImageView> pin;
protected:
    gptr<Buffer> vertexBuffer;
    gptr<Buffer> indexBuffer;
    gptr<Material> mat;
    size_t indexCount = 0;
    bool meshDirty = true;

    Box border;
    Color color = Color::White();
    gptr<Texture> tex;

    void UpdateMesh();

    void Construct();
public:
    
    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    ImageView(){ Construct(); }

    void SetBorder(const Box& box);
    Box GetBorder() const;

    void SetColor(const Color& color);
    Color GetColor() const;

    void SetTexture(const gptr<Texture>& tex);
    gptr<Texture> GetTexture() const;

    virtual void OnSizeChanged() override;
    
    virtual void GetRenderables(Sink<gptr<Renderable>> sink) override;

private:
    gptr<Renderable> renderable;
};

} // scene
} // mw
