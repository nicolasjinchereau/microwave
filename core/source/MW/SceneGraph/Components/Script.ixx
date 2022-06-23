/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.Script;
import Microwave.IO.Terminal;
import Microwave.Math;
import Microwave.SceneGraph.Components.Component;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.Node;
import Microwave.SceneGraph.Scene;
import Microwave.SceneGraph.Coroutine;
import Microwave.System.Clock;
import Microwave.System.Exception;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.Utilities.Util;
import <algorithm>;
import <vector>;

export namespace mw {
inline namespace scene {

class Script : public Component
{
    inline static Type::Pin<Script> pin;

    std::vector<Coroutine> coroutines;
public:

    Script() {}

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
                catch (const Exception& ex) {
                    writeln("Coroutine execution failed: ", ex.what());
                    done = true;
                }

                return done;
            }
        ), coroutines.end());
    }
};

} // scene
} // mw
