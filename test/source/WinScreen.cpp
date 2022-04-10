/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Test.WinScreen;
import Test.Game;
import Microwave;

using namespace mw;

namespace Test {

void WinScreen::SetCoinCount(int count)
{
    coinCount = count;

    if (coinCountText)
        coinCountText->SetText(std::to_string(coinCount));
}

void WinScreen::SetCompletionTime(float seconds)
{
    completionTime = (int)seconds;

    if (completionTimeText)
        completionTimeText->SetText(GetTimeString(completionTime));
}

std::string WinScreen::GetTimeString(int totalSeconds)
{
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    char result[64];
    snprintf(result, 64, "%d:%02d", minutes, seconds);

    return result;
}

void WinScreen::PlayAgain()
{
    if (auto g = game.lock())
    {
        g->ResetGame();
    }
}

Task<void> WinScreen::InitAsync()
{
    auto assetLibrary = App::Get()->GetAssetLibrary();

    auto node = GetNode();

    auto font = co_await assetLibrary->GetAssetAsync<Font>("Fonts/comic_bold.ttf");
    auto blueBackgroundTex = co_await assetLibrary->GetAssetAsync<Texture>("Textures/UI/BlueBackground.png");
    auto coinTex = co_await assetLibrary->GetAssetAsync<Texture>("Textures/UI/Coin.png");
    auto clockTex = co_await assetLibrary->GetAssetAsync<Texture>("Textures/UI/Clock.png");
    auto blueButtonTex = co_await assetLibrary->GetAssetAsync<Texture>("Textures/UI/SquareButtonBlue.png");

    auto background = node->AddComponent<ImageView>();
    background->SetTexture(blueBackgroundTex);
    background->SetBorder(Box(40, 40, 40, 40));
    background->SetAnchor(Box(0.5f, 0.5f, 0.5f, 0.5f));
    background->SetAnchorOffset(Box(-440, 340, 440, -340));

    auto textView = node->AddChild()->AddComponent<TextView>();
    textView->SetFont(font);
    textView->SetText("Level Passed!");
    textView->SetFontSize(120);
    textView->SetAnchor(Box(0, 0.5f, 1, 0.5f));
    textView->SetAnchorOffset(Box(0, 350, 0, 50));
    textView->SetAlignment(TextAlign::MiddleCenter);

    auto coinImg = node->AddChild()->AddComponent<ImageView>();
    coinImg->SetTexture(coinTex);
    coinImg->SetAnchor(Box(0, 0.5f, 0, 0.5f));
    coinImg->SetAnchorOffset(Box(210, 50, 310, -50));

    coinCountText = node->AddChild()->AddComponent<TextView>();
    coinCountText->SetFont(font);
    coinCountText->SetFontSize(80);
    coinCountText->SetAnchor(Box(0, 0.5f, 1.0f, 0.5f));
    coinCountText->SetAnchorOffset(Box(330, 50, 0, -50));
    coinCountText->SetAlignment(TextAlign::MiddleLeft);
    coinCountText->SetText(std::to_string(coinCount));

    auto clockImg = node->AddChild()->AddComponent<ImageView>();
    clockImg->SetTexture(clockTex);
    clockImg->SetAnchor(Box(0, 0.5f, 0, 0.5f));
    clockImg->SetAnchorOffset(Box(210, -60, 310, -160));

    completionTimeText = node->AddChild()->AddComponent<TextView>();
    completionTimeText->SetFont(font);
    completionTimeText->SetFontSize(80);
    completionTimeText->SetAnchor(Box(0, 0.5f, 1.0f, 0.5f));
    completionTimeText->SetAnchorOffset(Box(330, -70, 0, -170));
    completionTimeText->SetAlignment(TextAlign::MiddleLeft);
    completionTimeText->SetText(GetTimeString(completionTime));

    auto button = node->AddChild()->AddComponent<Button>();
    button->SetAnchor(Box(0.5f, 0.0f, 0.5f, 0.0f));
    button->SetAnchorOffset(Box(-130, 150, 130, 50));
    button->SetTexture(blueButtonTex);
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

} // Test
