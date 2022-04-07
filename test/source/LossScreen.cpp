/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include "LossScreen.h"
#include "Game.h"

void LossScreen::PlayAgain()
{
    if (auto g = game.lock())
    {
        g->ResetGame();
    }
}

Task<void> LossScreen::InitAsync()
{
    auto assetLibrary = App::Get()->GetAssetLibrary();

    auto node = GetNode();

    auto blueBackgroundTex = co_await assetLibrary->GetAssetAsync<Texture>("Textures/UI/BlueBackground.png");
    auto font = co_await assetLibrary->GetAssetAsync<Font>("Fonts/comic_bold.ttf");
    auto buttonTex = co_await assetLibrary->GetAssetAsync<Texture>("Textures/UI/SquareButtonBlue.png");

    auto background = node->AddComponent<ImageView>();
    background->SetTexture(blueBackgroundTex);
    background->SetBorder(Box(40, 40, 40, 40));
    background->SetAnchor(Box(0.5f, 0.5f, 0.5f, 0.5f));
    background->SetAnchorOffset(Box(-440, 340, 440, -340));

    auto textView = node->AddChild()->AddComponent<TextView>();
    textView->SetFont(font);
    textView->SetText("Level Failed!");
    textView->SetFontSize(120);
    textView->SetAnchor(Box(0, 0.5f, 1, 0.5f));
    textView->SetAnchorOffset(Box(0, 140, 0, -100));
    textView->SetAlignment(TextAlign::MiddleCenter);

    auto button = node->AddChild()->AddComponent<Button>();
    button->SetAnchor(Box(0.5f, 0.0f, 0.5f, 0.0f));
    button->SetAnchorOffset(Box(-130, 150, 130, 50));
    button->SetTexture(buttonTex);
    button->SetBorder({ 24, 24, 24, 24 });

    auto buttonText = button->GetNode()->AddChild()->AddComponent<TextView>();
    buttonText->SetFont(font);
    buttonText->SetFontSize(40);
    buttonText->SetAnchor(Box(0, 1, 1, 0));
    buttonText->SetAnchorOffset(Box(0, 0, 0, 0));
    buttonText->SetAlignment(TextAlign::MiddleCenter);
    buttonText->SetText("Play Again");
    
    button->SetAction([=]{ PlayAgain(); });

    node->SetLayerMask(LayerMask::UI, true);
}
