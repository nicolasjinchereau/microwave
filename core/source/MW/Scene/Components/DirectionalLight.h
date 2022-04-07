/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Graphics/Color.h>
#include <MW/Math/Math.h>
#include <MW/Scene/Components/Component.h>
#include <MW/Scene/LayerMask.h>

namespace mw {
inline namespace scene {

class DirectionalLight : public Component
{
    REGISTER_OBJECT(DirectionalLight);

    Color color = Color::White();
    float intensity = 1.0f;
    LayerMask cullingMask = LayerMask::All;

public:
    DirectionalLight();

    void SetCullingMask(LayerMask mask);
    LayerMask GetCullingMask() const;

    void SetColor(const Color& color);
    Color GetColor() const;

    void SetIntensity(float intensity);
    float GetIntensity() const;
};

} // scene
} // mw
