/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Scene/Renderable.h>
#include <cstdint>
#include <vector>

namespace mw {
inline namespace scene {

class Renderable;
class Scene;

class SceneRenderer
{
    bool gizmosEnabled = false;
    std::vector<sptr<Renderable>> renderables;
    std::vector<sptr<Renderable>> temp;

public:
    SceneRenderer();

    void Render(const sptr<Scene>& scene);

    void SetGizmosEnabled(bool enable);
    bool GetGizmosEnabled() const;
};

} // scene
} // mw
