/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.SceneRenderer;
import Microwave.Graphics.Color;
import Microwave.Graphics.GraphicsContext;
import Microwave.Math;
import Microwave.SceneGraph.Components.Camera;
import Microwave.SceneGraph.Components.DirectionalLight;
import Microwave.SceneGraph.LayerMask;
import Microwave.SceneGraph.Node;
import Microwave.SceneGraph.Scene;
import <algorithm>;
import <cassert>;
import <cstdint>;
import <vector>;
import <utility>;

namespace mw {
inline namespace scene {

SceneRenderer::SceneRenderer()
{
    temp.reserve(16);
}

void SceneRenderer::Render(const sptr<Scene>& scene)
{
    auto graphics = GraphicsContext::GetCurrent();

    graphics->SetClearColor(Color::Clear());
    graphics->Clear(true, true);

    std::sort(
        scene->cameras.begin(), scene->cameras.end(),
        [](const sptr<Camera>& a, const sptr<Camera>& b) {
            return a->GetRenderOrder() < b->GetRenderOrder();
        }
    );

    for (auto& camera : scene->cameras)
    {
        if (!camera->IsActiveAndEnabled())
            continue;

        auto camCullingMask = camera->GetCullingMask();

        for (sptr<IRenderEvents>& r : scene->renderEvents)
        {
            if (r->IsActiveAndEnabled())
            {
                r->GetRenderables(temp);

                for(auto& rend : temp)
                {
                    if ((rend->layerMask & camCullingMask) != 0 && camera->CanSee(rend->bounds))
                    {
                        auto queue = rend->queueOverride ? rend->queueOverride : rend->material->renderQueue;
                        auto depth = camera->GetDepth(rend->mtxModel.GetTranslation());
                        rend->sortKey = Renderable::MakeSortKey(queue, depth);
                        renderables.push_back(std::move(rend));
                    }
                }

                temp.clear();
            }
        }

        std::sort(
            renderables.begin(), renderables.end(),
            [](const sptr<Renderable>& a, const sptr<Renderable>& b) {
                return a->sortKey < b->sortKey;
            }
        );

        auto camPosition = camera->GetNode()->GetPosition();
        auto mtxViewProj = camera->GetViewProjectionMatrix();
        auto ambientColor = scene->GetAmbientColor();

        for (auto& rend : renderables)
        {
            DirectionalLight* closestLight = nullptr;
            float closestLightDist = FLT_MAX;

            for(auto& light : scene->lights)
            {
                if ((rend->layerMask & light->GetCullingMask()) != 0)
                {
                    auto lightPos = light->GetNode()->GetPosition();
                    auto objectPos = rend->mtxModel.GetTranslation();

                    auto dist = lightPos.DistanceSq(objectPos);
                    if(dist < closestLightDist)
                    {
                        closestLight = light.get();
                        closestLightDist = dist;
                    }
                }
            }

            Vec4 lightPos = Vec3::Zero();
            Color lightColor = Color::Clear();

            if (closestLight)
            {
                // DirectionalLight: { -direction, 0 }
                // PointLight: { pos, 1 }
                lightPos = Vec4(-closestLight->GetNode()->Forward(), 0);
                lightColor = closestLight->GetColor() * closestLight->GetIntensity();
            }

            rend->material->SetActive(&rend->extra);

            rend->material->shader->SetUniform("uMtxModel", rend->mtxModel);
            rend->material->shader->SetUniform("uMtxMVP", rend->mtxModel * mtxViewProj);

            rend->material->shader->SetUniform("uCameraPos", Vec4(camPosition, 0));
            rend->material->shader->SetUniform("uAmbientColor", ambientColor);

            rend->material->shader->SetUniform("uLightPos", lightPos);
            rend->material->shader->SetUniform("uLightColor", lightColor);

            rend->material->shader->SetVertexBuffer(rend->vertexMapping);
            
            if (rend->indexBuffer)
            {
                rend->material->shader->SetIndexBuffer(rend->indexBuffer);
                graphics->DrawIndexed(rend->drawStart, rend->drawCount, rend->drawMode);
            }
            else
            {
                graphics->DrawArray(rend->drawStart, rend->drawCount, rend->drawMode);
            }
        }

        renderables.clear();
    }

    graphics->Flip();
}

void SceneRenderer::SetGizmosEnabled(bool enable) {
    gizmosEnabled = enable;
}

bool SceneRenderer::GetGizmosEnabled() const {
    return gizmosEnabled;
}

} // scene
} // mw
