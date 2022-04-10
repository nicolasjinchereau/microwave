/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.DirectionalLight;
import Microwave.SceneGraph.Node;

namespace mw {
inline namespace scene {

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
