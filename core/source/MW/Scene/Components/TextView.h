/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/DrawBuffer.h>
#include <MW/Graphics/Color.h>
#include <MW/Graphics/Font.h>
#include <MW/Graphics/Material.h>
#include <MW/Graphics/Texture.h>
#include <MW/Scene/Components/View.h>
#include <MW/Scene/Events.h>
#include <string>

namespace mw {
inline namespace scene {

class TextView : public View, public IRenderEvents
{
    REGISTER_OBJECT(TextView);
protected:
    std::vector<IVec2> vertexRanges;
    sptr<DrawBuffer> vertexBuffer;
    sptr<DrawBuffer> indexBuffer;
    sptr<Material> mat;
    bool textDirty = true;

    sptr<Font> font;
    int fontSize = 32;
    bool wrapping = false;
    TextAlign alignment = TextAlign::MiddleCenter;
    int lineSpacing = 0;
    std::string text;

    void UpdateText();
public:

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    TextView();

    void SetFont(const sptr<Font>& font);
    const sptr<Font>& GetFont() const;

    void SetFontSize(int fontSize);
    int GetFontSize() const;

    void SetWrapping(bool enabled);
    bool GetWrapping() const;

    void SetAlignment(TextAlign align);
    TextAlign GetAlignment() const;

    void SetLineSpacing(int spacing);
    int GetLineSpacing() const;

    void SetText(const std::string& text);
    std::string_view GetText() const;

    virtual void OnSizeChanged() override;
    
    virtual void GetRenderables(Sink<sptr<Renderable>> sink) override;

private:
    std::vector<sptr<Renderable>> renderables;
};

} // scene
} // mw
