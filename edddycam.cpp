/* Edddy
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Commercial licenses are available through frode@lindeijer.nl
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "edddycam.h"
#include "blockmap.h"

EdddyCam::EdddyCam(Context *context):
    LogicComponent(context)
{
}
void EdddyCam::RegisterObject(Context* context)
{
    context->RegisterFactory<EdddyCam>();
}

void EdddyCam::OnNodeSet(Node *node)
{(void)node;

    camera_ = node_->CreateComponent<Camera>();
    camera_->SetFarClip(1024.0f);
    camera_->SetFov(60.0f);
    node_->SetPosition(Vector3(BLOCK_WIDTH  * MAP_WIDTH/2,
                               BLOCK_HEIGHT * MAP_HEIGHT/2,
                               -BLOCK_DEPTH));
    node_->SetRotation(Quaternion(23.0f, 0.0f, 0.0f));

    Zone* zone{node_->CreateComponent<Zone>()};
    zone->SetAmbientColor(Color(0.13f, 0.23f, 0.42f));

    SetupViewport();

}
void EdddyCam::SetupViewport()
{
    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    Viewport* viewport(new Viewport(context_, MC->GetScene(), camera_));

    RenderPath* effectRenderPath{viewport->GetRenderPath()};
    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath->SetEnabled("FXAA3", true);

//    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/BloomHDR.xml"));
//    effectRenderPath->SetShaderParameter("BloomHDRThreshold", 0.42f);
//    effectRenderPath->SetShaderParameter("BloomHDRMix", Vector2(0.9f, 0.23f));
//    effectRenderPath->SetEnabled("BloomHDR", true);

    viewport->SetRenderPath(effectRenderPath);
    RENDERER->SetViewport(0, viewport);
}

void EdddyCam::Update(float timeStep)
{
///Quick and temporary fly mode

    float speed{1.0f + 7.0f * INPUT->GetKeyDown(KEY_SHIFT)};
    node_->Translate( speed * timeStep *
               ((node_->GetDirection() * Vector3(1.0f, 0.0f, 1.0f)).Normalized() * (INPUT->GetKeyDown(KEY_W) - INPUT->GetKeyDown(KEY_S)) +
                node_->GetRight() * (INPUT->GetKeyDown(KEY_D) - INPUT->GetKeyDown(KEY_A)) +
                Vector3::UP * (INPUT->GetKeyDown(KEY_Q) - INPUT->GetKeyDown(KEY_E))), TS_WORLD
                );

}
