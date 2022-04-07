/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Components/Component.h>
#include <MW/Scene/Scene.h>
#include <MW/Scene/Node.h>
#include <MW/Scene/Coroutine.h>
#include <MW/System/Pointers.h>
#include <MW/Utilities/Util.h>
#include <iostream>

namespace mw {
inline namespace scene {

class Script : public Component
{
    REGISTER_OBJECT(Script);

    std::vector<Coroutine> coroutines;
public:

    Script()
    {
        SetEventBit(EventBit::Script);
    }

    Coroutine StartCoroutine(Coroutine coroutine)
    {
        if (coroutine.Step(GetClock()))
            coroutines.emplace_back(coroutine);

        return coroutine;
    }

    void UpdateCoroutines()
    {
        auto clock = GetClock();

        coroutines.erase(std::remove_if(
            coroutines.begin(),
            coroutines.end(),
            [this, clock](Coroutine& coroutine)
            {
                bool done = false;

                try {
                    done = !coroutine.Step(clock);
                }
                catch (std::exception& ex) {
                    std::cout << "Coroutine execution failed:\n" << ex.what() << std::endl;
                    done = true;
                }

                return done;
            }
        ), coroutines.end());
    }
};

} // scene
} // mw
