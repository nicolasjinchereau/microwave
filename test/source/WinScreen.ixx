/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Test.WinScreen;
import Microwave;
import <vector>;
import <string>;

using namespace mw;

export namespace Test {

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

} // Test
