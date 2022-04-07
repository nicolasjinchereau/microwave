/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Components/Button.h>
#include <MW/Scene/Components/ImageView.h>
#include <MW/Scene/Components/Script.h>
#include <MW/Scene/Components/TextView.h>
#include <MW/Scene/Events.h>
#include <vector>

using namespace mw;

class Game;

class WinScreen : public Script
                , public IUserEvents
{
    int coinCount = 0;
    int completionTime = 0;
    sptr<TextView> coinCountText;
    sptr<TextView> completionTimeText;
public:
    
    wptr<Game> game;

    void SetCoinCount(int count);
    void SetCompletionTime(float seconds);
    std::string GetTimeString(int totalSeconds);
    void PlayAgain();

    Task<void> InitAsync();
};
