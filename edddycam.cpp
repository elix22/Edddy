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
#include "editmaster.h"

EdddyCam::EdddyCam(Context *context):
    LogicComponent(context)
{
}
void EdddyCam::RegisterObject(Context* context)
{
    context->RegisterFactory<EdddyCam>();
}

void EdddyCam::OnNodeSet(Node *node)
{ if (!node) return;

    camera_ = node_->CreateChild("Camera")->CreateComponent<Camera>();
    camera_->SetFarClip(4096.0f);
    camera_->SetFov(60.0f);

   /* node_->SetPosition(Vector3(BLOCK_WIDTH  * MAP_WIDTH / 2,
                               BLOCK_HEIGHT * MAP_HEIGHT / 2,
                               -BLOCK_DEPTH));*/
    node_->SetRotation(Quaternion(23.0f, 0.0f, 0.0f));

    Zone* zone{ node_->CreateComponent<Zone>() };
    zone->SetAmbientColor(Color(0.13f, 0.23f, 0.42f));
    zone->SetFogStart(2048.0f);
    zone->SetFogEnd(4096.0f);

    SetupViewport();

    SubscribeToEvent(E_CURRENTMAPCHANGE, URHO3D_HANDLER(EdddyCam, HandleMapChange));
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
void EdddyCam::HandleMapChange(StringHash eventType, VariantMap& eventData)
{
    BlockMap* blockMap{ static_cast<BlockMap*>(eventData[CurrentMapChange::P_MAP].GetPtr()) };
    node_->SetPosition(-Max(Max(blockMap->GetMapWidth(),
                                blockMap->GetMapHeight()),
                                blockMap->GetMapDepth()) * node_->GetDirection() * Sqrt(blockMap->GetBlockSize().Length())
                       + blockMap->GetCenter());
}

void EdddyCam::ToggleOrthogaphic()
{
    camera_->SetOrthographic(!camera_->IsOrthographic());
}

void EdddyCam::Update(float timeStep)
{
    ///About to be replaced

//    if (!INPUT->GetMouseButtonDown(2)
//     && !GetSubsystem<UI>()->GetFocusElement()
//     && GetSubsystem<EditMaster>()->GetCurrentBlockMap()) {

//        float factor{ (2.0f + 3.0f * INPUT->GetKeyDown(KEY_SHIFT)) };
//        float translationSpeed{ factor * factor};
//        float rotationSpeed{ 23.0f * factor };
//        Vector3 cursorPosition{ GetSubsystem<InputMaster>()->GetCursor()->GetNode()->GetPosition() };

//        //Pan
//        node_->Translate( timeStep * translationSpeed *
//                          ((node_->GetDirection() * Vector3(1.0f, 0.0f, 1.0f)).Normalized() * (INPUT->GetKeyDown(KEY_W) - INPUT->GetKeyDown(KEY_S)) +
//                           node_->GetRight() * (INPUT->GetKeyDown(KEY_D) - INPUT->GetKeyDown(KEY_A)) +
//                           Vector3::UP * (INPUT->GetKeyDown(KEY_E) - INPUT->GetKeyDown(KEY_Q))), TS_WORLD);


//        //Zoom
//        bool ortho{ camera_->IsOrthographic() };
//        if (!ortho)
//            node_->Translate(timeStep * translationSpeed * node_->GetDirection() * (INPUT->GetKeyDown(KEY_KP_PLUS) - INPUT->GetKeyDown(KEY_KP_MINUS)), TS_WORLD);
//        else
//            camera_->SetOrthoSize(Max(1.0f, camera_->GetOrthoSize() - timeStep * translationSpeed * (INPUT->GetKeyDown(KEY_KP_PLUS) - INPUT->GetKeyDown(KEY_KP_MINUS))));

//        //Rotate
//        node_->RotateAround( cursorPosition,
//                             Quaternion(0.0f,
//                                        (INPUT->GetKeyDown(KEY_KP_4) - INPUT->GetKeyDown(KEY_KP_6)) * rotationSpeed * timeStep,
//                                        0.0f), TS_WORLD);
//        node_->RotateAround( node_->GetRotation().Inverse() * (cursorPosition - node_->GetPosition()),
//                             Quaternion((INPUT->GetKeyDown(KEY_KP_8) - INPUT->GetKeyDown(KEY_KP_2)) * rotationSpeed * timeStep,
//                                        0.0f,
//                                        0.0f), TS_LOCAL);
//    }

}

void EdddyCam::Move(Vector3 movement, MoveType type)
{
    EdddyCursor* cursor{ GetSubsystem<InputMaster>()->GetCursor() };
    Vector3 cursorPosition{ cursor->GetNode()->GetPosition() };
    float cursorDistance{ LucKey::Distance(GetPosition(), cursorPosition) };

    bool ortho{ camera_->IsOrthographic() };
    float panSpeed{ 1.5f + (ortho ? (camera_->GetOrthoSize() * 1.3f) : (cursorDistance * camera_->GetFov() * 0.018f)) };
    float rotationSpeed{ 235.0f };

    switch (type) {
    case MT_FOV: {

        if (!ortho)
            camera_->SetFov(Clamp(camera_->GetFov() + 23.0f * movement.y_, 5.0f, 120.0f));
        else
            node_->Translate(Vector3::BACK * movement.y_ * panSpeed);

    } break;
    case MT_PAN: {

        movement *= panSpeed;

        if (!ortho) {

            node_->Translate(node_->GetRight() * -movement.x_ +
                             node_->GetUp() * movement.y_ +
                             node_->GetDirection() * movement.z_, TS_WORLD);

        } else {

            camera_->SetOrthoSize(Max(1.0f, camera_->GetOrthoSize() + movement.z_ * -1.3f));
            Vector3 lockVector{ cursor->GetLockVector() };

            if (lockVector.Length() != 1.0f) { ///Needs more cases or generalisation

                if (lockVector.y_ == 0.0f)
                    movement.y_ /= Abs(node_->GetDirection().DotProduct(Vector3::UP) * M_SQRT2);
//                else
//                    movement.x_ /= Abs(node_->GetDirection().DotProduct(Vector3::FORWARD) * M_SQRT2);
            }

            node_->Translate((lockVector * node_->GetRight()).Normalized() * -movement.x_
                           + (lockVector * node_->GetUp()).Normalized()    *  movement.y_, TS_WORLD);

        }

    } break;
    default: case MT_ROTATE: {

        float pitchDelta{ movement.y_ * rotationSpeed };
        ClampPitch(pitchDelta);

        node_->RotateAround(cursorPosition,
                            Quaternion(movement.x_ * rotationSpeed, Vector3::UP) *
                            Quaternion(pitchDelta, node_->GetRight()), TS_WORLD);
    }
    }
}

void EdddyCam::ClampPitch(float& pitchDelta)
{
    float resultingPitch{ node_->GetRotation().PitchAngle() + pitchDelta };

    if (resultingPitch > PITCH_MAX) {

        pitchDelta -= resultingPitch - PITCH_MAX;

    } else if (resultingPitch < PITCH_MIN) {

        pitchDelta -= resultingPitch - PITCH_MIN;
    }
}
