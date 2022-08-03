/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Editor.Toolbar;
import Microwave;

export namespace mw {
inline namespace editor {

class Toolbar : public View
              , public IUserEvents
{
    gvector<gptr<Button>> buttons;
public:
    virtual void Start() override
    {
        // pin to top and width
        SetAnchor(Box(0, 1, 1, 1)); // left, top, right, bottom
        SetAnchorOffset(Box(0, 0, 0, -100));

        auto node = GetNode();

        auto assetLibrary = App::Get()->GetAssetLibrary();

        auto bgTex = assetLibrary->GetAsset<Texture>("Textures/UI/ThinBorder.png");
        auto background = node->AddChild()->AddComponent<ImageView>();
        background->SetTexture(bgTex);
        background->SetColor(Color(1, 1, 1, 1));
        background->SetAnchor(Box(0, 1, 1, 0));
        background->SetAnchorOffset(Box(0, 0, 0, 0));
        background->SetBorder(Box(2, 2, 2, 2));

        node->SetLayerMask(LayerMask::UI, true);
    }

    float imagePadding = 8;

    void AddButton(const path& iconPath, const std::function<void()>& func)
    {
        auto assetLibrary = App::Get()->GetAssetLibrary();

        auto btnTex = assetLibrary->GetAsset<Texture>("Textures/UI/SquareButtonBlue.png");
        
        auto node = GetNode();

        auto btn = node->AddChild()->AddComponent<Button>();
        btn->SetAnchor(Box(0, 1, 0, 0));
        btn->SetAnchorOffset(Box(2, -2, 98, 2));
        btn->SetTexture(btnTex);
        btn->SetBorder({ 24, 24, 24, 24 });
        btn->SetAction(func);

        auto iconTex = assetLibrary->GetAsset<Texture>(iconPath);
        auto img = btn->GetNode()->AddChild()->AddComponent<ImageView>();
        img->SetTexture(iconTex);
        img->SetAnchor(Box(0, 1, 1, 0));
        img->SetAnchorOffset(Box(imagePadding, -imagePadding, -imagePadding, imagePadding));

        node->SetLayerMask(LayerMask::UI, true);

        buttons.push_back(btn);
        RecalcLayout();
    }

    void RecalcLayout()
    {
        float left = 0;

        for(auto& btn : buttons)
        {
            btn->SetAnchorOffset(Box(left + 2, -2, left + 98, 2));
            left += 100;
        }
    }
};

} // editor
} // mw
