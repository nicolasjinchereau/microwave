/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/Scene/Components/DirectionalLight.h>
#include <MW/Scene/Node.h>

namespace mw {
inline namespace scene {

DirectionalLight::DirectionalLight()
{
    SetEventBit(EventBit::Light);
}

void DirectionalLight::SetCullingMask(LayerMask mask) {
    cullingMask = mask;
}

LayerMask DirectionalLight::GetCullingMask() const {
    return cullingMask;
}

void DirectionalLight::SetColor(const Color& color) {
    this->color = color;
}

Color DirectionalLight::GetColor() const {
    return color;
}

void DirectionalLight::SetIntensity(float intensity) {
    this->intensity = intensity;
}

float DirectionalLight::GetIntensity() const {
    return intensity;
}

} // scene
} // mw
