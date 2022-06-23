/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Test.CoinCounter;
import Microwave;
import <vector>;

export namespace Test {

using namespace mw;

class CoinCounter : public Script
                  , public IUserEvents
{
public:
    int coinsCollected = 0;
    gptr<ImageView> coinImage;
    gptr<TextView> coinText;
    gptr<AudioSource> coinSound;

    Task<void> InitAsync()
    {
        auto assetLibrary = App::Get()->GetAssetLibrary();
        auto node = GetNode();

        auto backgroundTex = co_await assetLibrary->GetAssetAsync<Texture>("Textures/UI/blueOval.png");
        auto coinTex = co_await assetLibrary->GetAssetAsync<Texture>("Textures/UI/Coin.png");
        auto coinFont = co_await assetLibrary->GetAssetAsync<Font>("Fonts/comic_bold.ttf");
        auto coinClip = co_await assetLibrary->GetAssetAsync<AudioClip>("Audio/Sounds/Coin.wav");

        auto background = node->AddComponent<ImageView>();
        background->SetTexture(backgroundTex);
        background->SetColor(Color(1, 1, 1, 0.6f));
        background->SetAnchor(Box(0.5f, 0.5f, 0.5f, 0.5f)); // middle center
        background->SetAnchorOffset(Box(-185, 730, 185, 628));
        background->SetBorder(Box(56, 0, 56, 0));
        
        coinImage = node->AddChild()->AddComponent<ImageView>();
        coinImage->SetTexture(coinTex);
        coinImage->SetAnchor(Box(0, 0.5f, 0, 0.5f));
        coinImage->SetAnchorOffset(Box(0, 51, 102, -51));

        coinText = node->AddChild()->AddComponent<TextView>();
        coinText->SetFont(coinFont);
        coinText->SetText("0");
        coinText->SetFontSize(80);
        coinText->SetAnchor(Box(0, 1, 1, 0)); // middle left
        coinText->SetAnchorOffset(Box(112, 0, 0, 4));
        coinText->SetAlignment(TextAlign::MiddleLeft);

        node->SetLayerMask(LayerMask::UI, true);

        coinSound = node->AddChild()->AddComponent<AudioSource>();
        coinSound->SetClip(coinClip);
    }

    int GetCoinsCollected() const {
        return coinsCollected;
    }

    void AnimateCollectedCoin(const Vec2& canvasPos) {
        StartCoroutine(CoinAnimation(canvasPos));
    }

    void Reset() {
        coinsCollected = 0;
        coinText->SetText(std::to_string(coinsCollected));
    }
private:
    Coroutine CoinAnimation(const Vec2& canvasPos)
    {
        gptr<Node> coinCopy = Instantiate<Node>(coinImage->GetNode());
        GetNode()->AddChild(coinCopy);
        auto view = coinCopy->GetComponent<ImageView>();
        view->SetAnchorEnabled(false);

        Vec3 startPos = Vec3(canvasPos.x, canvasPos.y, 0);
        Vec3 endPos = coinImage->GetNode()->GetPosition();

        float duration = 0.15f;
        float startTime = GetClock()->GetTime();
        float endTime = startTime + duration;

        float now = startTime;

        while (now < endTime)
        {
            now = GetClock()->GetTime();
            float t = math::Clamp01((now - startTime) / (endTime - startTime));
            Vec3 pos = Vec3::Lerp(startPos, endPos, t);
            coinCopy->SetPosition(pos);

            co_yield Wait::Next();
        }

        coinSound->Play();

        ++coinsCollected;
        coinText->SetText(std::to_string(coinsCollected));

        coinCopy->SetParent(nullptr);
    }
};

} // Test
