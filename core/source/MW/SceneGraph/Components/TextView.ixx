/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.TextView;
import Microwave.Graphics.Color;
import Microwave.Graphics.Buffer;
import Microwave.Graphics.Font;
import Microwave.Graphics.Material;
import Microwave.Graphics.Texture;
import Microwave.Math;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Renderable;
import Microwave.SceneGraph.Components.View;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.Utilities.Sink;
import Microwave.Utilities.Util;
import <string>;
import <vector>;

export namespace mw {
inline namespace scene {

class TextView : public View, public IRenderEvents
{
    inline static Type::Pin<TextView> pin;
protected:
    std::vector<IVec2> vertexRanges;
    gptr<Buffer> vertexBuffer;
    gptr<Material> mat;
    bool textDirty = true;

    gptr<Font> font;
    int fontSize = 32;
    bool wrapping = false;
    TextAlign alignment = TextAlign::MiddleCenter;
    int lineSpacing = 0;
    std::string text;

    void UpdateText();
    void Construct();
public:

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    TextView(){ Construct(); }

    void SetFont(const gptr<Font>& font);
    const gptr<Font>& GetFont() const;

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
    
    virtual void GetRenderables(Sink<gptr<Renderable>> sink) override;

private:
    gvector<gptr<Renderable>> renderables;
};

} // scene
} // mw
