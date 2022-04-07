/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Events.h>
#include <MW/Scene/Components/Button.h>
#include <MW/Scene/Components/ImageView.h>
#include <MW/Scene/Components/Script.h>
#include <MW/Scene/Components/TextView.h>
#include <vector>

using namespace mw;

class Game;

class LossScreen : public Script
                 , public IUserEvents
{
public:
    wptr<Game> game;

    void PlayAgain();
    Task<void> InitAsync();
};
