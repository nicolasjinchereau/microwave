/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Color.h>
#include <MW/Graphics/DrawBuffer.h>
#include <MW/Graphics/Material.h>
#include <MW/Graphics/Texture.h>
#include <MW/Graphics/Types.h>
#include <MW/Scene/Components/View.h>
#include <MW/Scene/Events.h>

namespace mw {
inline namespace scene {

class ImageView : public View, public IRenderEvents
{
    REGISTER_OBJECT(ImageView);
protected:
    sptr<DrawBuffer> vertexBuffer;
    sptr<DrawBuffer> indexBuffer;
    sptr<Material> mat;
    size_t indexCount = 0;
    bool meshDirty = true;

    Box border;
    Color color = Color::White();
    sptr<Texture> tex;

    void UpdateMesh();

public:
    
    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    ImageView();

    void SetBorder(const Box& box);
    Box GetBorder() const;

    void SetColor(const Color& color);
    Color GetColor() const;

    void SetTexture(const sptr<Texture>& tex);
    sptr<Texture> GetTexture() const;

    virtual void OnSizeChanged() override;
    
    virtual void GetRenderables(Sink<sptr<Renderable>> sink) override;

private:
    sptr<Renderable> renderable;
};

} // scene
} // mw
