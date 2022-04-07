/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <vector>
#include <MW/Scene/Components/ImageView.h>
#include <MW/Scene/Components/Script.h>
#include <MW/Scene/Events.h>
#include "Player.h"

using namespace mw;

class Player;

class BatteryMeter : public Script
                   , public IUserEvents
{
    std::vector<sptr<ImageView>> bars;
    Color barColor = Color32(175, 248, 255, 210);

public:
    wptr<Player> player;

    Task<void> InitAsync()
    {
        auto assetLibrary = App::Get()->GetAssetLibrary();

        auto backgroundTex = co_await assetLibrary->GetAssetAsync<Texture>("Textures/UI/BatteryMeter.png");
        auto meterBarTex = co_await assetLibrary->GetAssetAsync<Texture>("Textures/UI/BatteryMeterBar.png");

        auto batteryMeterNode = GetNode();
        auto batteryMeterView = batteryMeterNode->AddComponent<ImageView>();
        batteryMeterView->SetTexture(backgroundTex);
        batteryMeterView->SetAnchor(Box(0.5f, 0.0f, 0.5f, 0.0f)); // bottom center
        batteryMeterView->SetAnchorOffset(Box(-286, 146, 286, 34));

        Vec2 barPos{ 67, -37.5 };

        for (int i = 0; i != 20; ++i)
        {
            auto meterBarNode = batteryMeterNode->AddChild(spnew<Node>());
            auto meterBarView = meterBarNode->AddComponent<ImageView>();
            meterBarView->SetTexture(meterBarTex);
            meterBarView->SetAnchor(Box(0.0f, 1.0f, 0.0f, 1.0f)); // top left
            meterBarView->SetAnchorOffset(Box(barPos.x, barPos.y, barPos.x + 21, barPos.y - 35));
            meterBarView->SetColor(barColor);
            barPos.x += 22.9f;
            bars.push_back(meterBarView);
        }

        batteryMeterNode->SetLayerMask(LayerMask::UI, true);
    }

    virtual void LateUpdate() override
    {
        UpdateBatteryBars();
    }

    void UpdateBatteryBars()
    {
        if(auto p = player.lock())
        {
            auto count = bars.size();
            auto barWidth = 1.0f / count;

            for (auto i = 0u; i != count; ++i)
            {
                float alpha = math::Clamp01((p->batteryLevel - i * barWidth) / barWidth);

                auto col = barColor;
                col.a *= alpha;
                bars[i]->SetColor(col);
            }
        }
    }
};
