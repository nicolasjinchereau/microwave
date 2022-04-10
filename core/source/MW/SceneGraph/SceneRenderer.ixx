/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.SceneRenderer;
import Microwave.SceneGraph.Renderable;
import Microwave.System.Pointers;
import <cstdint>;
import <vector>;

export namespace mw {
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