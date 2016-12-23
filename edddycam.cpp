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
#include "edddycursor.h"
#include "inputmaster.h"
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
{ (void)node;

    camera_ = node_->CreateComponent<Camera>();
    camera_->SetFarClip(1024.0f);
    camera_->SetFov(60.0f);

    node_->SetPosition(Vector3(BLOCK_WIDTH  * MAP_WIDTH/2,
                               BLOCK_HEIGHT * MAP_HEIGHT/2,
                               -BLOCK_DEPTH));
    node_->SetRotation(Quaternion(23.0f, 0.0f, 0.0f));

    Zone* zone{ node_->CreateComponent<Zone>() };
    zone->SetAmbientColor(Color(0.13f, 0.23f, 0.42f));

    SetupViewport();

}
void EdddyCam::SetupViewport()
{
    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    Viewport* viewport( new Viewport(context_, MC->GetScene(), camera_) );

    RenderPath* effectRenderPath{ viewport->GetRenderPath() };
    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath->SetEnabled("FXAA3", true);

    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/BloomHDR.xml"));
    effectRenderPath->SetShaderParameter("BloomHDRThreshold", 0.42f);
    effectRenderPath->SetShaderParameter("BloomHDRMix", Vector2(0.9f, 0.23f));
    effectRenderPath->SetEnabled("BloomHDR", true);

    viewport->SetRenderPath(effectRenderPath);
    RENDERER->SetViewport(0, viewport);
}

void EdddyCam::Update(float timeStep)
{
    if (!INPUT->GetMouseButtonDown(2)){

        float translationSpeed{ 5.0f * (2.0f + 3.0f * INPUT->GetKeyDown(KEY_SHIFT)) };
        float zoomSpeed{ 0.1f * (2.0f + 3.0f * INPUT->GetKeyDown(KEY_SHIFT)) };
        float rotationSpeed{ 23.0f * (2.0f + 3.0f * INPUT->GetKeyDown(KEY_SHIFT)) };
        Vector3 cursorPosition{ GetSubsystem<InputMaster>()->GetCursor()->GetNode()->GetPosition() };

        node_->Translate( translationSpeed * timeStep *
                          ((node_->GetDirection() * Vector3(1.0f, 0.0f, 1.0f)).Normalized() * (INPUT->GetKeyDown(KEY_W) - INPUT->GetKeyDown(KEY_S)) +
                           node_->GetRight() * (INPUT->GetKeyDown(KEY_D) - INPUT->GetKeyDown(KEY_A)) +
                           Vector3::UP * (INPUT->GetKeyDown(KEY_Q) - INPUT->GetKeyDown(KEY_E))) +
                          zoomSpeed * node_->GetDirection() * (INPUT->GetKeyDown(KEY_KP_PLUS) - INPUT->GetKeyDown(KEY_KP_MINUS)), TS_WORLD);

        node_->RotateAround( cursorPosition,
                             Quaternion(0.0f,
                                        (INPUT->GetKeyDown(KEY_KP_4) - INPUT->GetKeyDown(KEY_KP_6)) * rotationSpeed * timeStep,
                                        0.0f), TS_WORLD);
        node_->RotateAround( node_->GetRotation().Inverse() * (cursorPosition - node_->GetPosition()),
                             Quaternion((INPUT->GetKeyDown(KEY_KP_8) - INPUT->GetKeyDown(KEY_KP_2)) * rotationSpeed * timeStep,
                                        0.0f,
                                        0.0f), TS_LOCAL);
    }

    ClampPitch();
}

void EdddyCam::Pan(Vector2 pan)
{
    Vector3 cursorPosition{ GetSubsystem<InputMaster>()->GetCursor()->GetNode()->GetPosition() };
    float cursorDistance{ LucKey::Distance(GetPosition(), cursorPosition) };

    float panSpeed{ 1.5f + 1.8f * cursorDistance };
    float zoomSpeed{ 13.0f + 5.0f * cursorDistance };
    float rotationSpeed{ 235.0f };

    if (INPUT->GetKeyDown(KEY_SHIFT)){

        pan *= panSpeed;

        node_->Translate(node_->GetRight() * -pan.x_ + node_->GetUp() * pan.y_, TS_WORLD);

    } else if (INPUT->GetKeyDown(KEY_CTRL)) {

        node_->Translate(zoomSpeed * -node_->GetDirection() * pan.y_, TS_WORLD);

    } else {
        node_->RotateAround( cursorPosition,
                             Quaternion(0.0f,
                                        pan.x_ * rotationSpeed,
                                        0.0f), TS_WORLD);
        node_->RotateAround( node_->GetRotation().Inverse() * (cursorPosition - node_->GetPosition()),
                             Quaternion(pan.y_ * rotationSpeed,
                                        0.0f,
                                        0.0f), TS_LOCAL);
    }
}

void EdddyCam::ClampPitch()
{

    Vector3 cursorPosition{ GetSubsystem<InputMaster>()->GetCursor()->GetNode()->GetPosition() };

    if (GetPitch() > PITCH_MAX)
        node_->RotateAround(cursorPosition,
                                Quaternion(PITCH_MAX - GetPitch(), node_->GetRight()),
                                TS_WORLD);
    else if (GetPitch() < PITCH_MIN)
        node_->RotateAround(cursorPosition,
                                Quaternion(PITCH_MIN - GetPitch(), node_->GetRight()),
                                TS_WORLD);
}
