/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.DirectionalLight;
import Microwave.Graphics.Color;
import Microwave.Math;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.LayerMask;
import Microwave.SceneGraph.Components.Component;

export namespace mw {
inline namespace scene {

class DirectionalLight : public Component
{
    inline static Type::Pin<DirectionalLight> pin;

    Color color = Color::White();
    float intensity = 1.0f;
    LayerMask cullingMask = LayerMask::All;

public:
    DirectionalLight() {
        SetEventBit(EventBit::Light);
    }

    void SetCullingMask(LayerMask mask);
    LayerMask GetCullingMask() const;

    void SetColor(const Color& color);
    Color GetColor() const;

    void SetIntensity(float intensity);
    float GetIntensity() const;
};

} // scene
} // mw
