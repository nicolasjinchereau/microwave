/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Test.LossScreen;
import Microwave;
import <vector>;

using namespace mw;

export namespace Test {

class Game;

class LossScreen : public Script
                 , public IUserEvents
{
public:
    gptr<Game> game;

    void PlayAgain();
    Task<void> InitAsync();
};

} // Test
