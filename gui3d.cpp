/* Edddy
// Copyright (C) 2017 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
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



#include "gui3d.h"
#include "resourcemaster.h"


GUI3D::GUI3D(Context* context) : Object(context)
{
    //Scene
    guiScene_ = new Scene(context_);
    guiScene_->CreateComponent<Octree>();
    //Camera
    Node* cameraNode{ guiScene_->CreateChild("Camera") };
    cameraNode->SetPosition(Vector3::BACK * 10.0f);
    cameraNode->LookAt(Vector3::ZERO);
    guiCamera_ = cameraNode->CreateComponent<Camera>();

    //Viewport
    SetupViewport();

    Zone* zone{ guiScene_->CreateComponent<Zone>() };
    zone->SetAmbientColor(Color(0.23f, 0.23f, 0.23f));

    Node* sunNode{ guiScene_->CreateChild("Sun") };
    sunNode->SetPosition(Vector3(1.0f, 5.0f, -3.0f));
    sunNode->LookAt(Vector3::ZERO);
    Light* sun{ sunNode->CreateComponent<Light>() };
    sun->SetLightType(LIGHT_DIRECTIONAL);

    for (int i : {0, 1, 2}) {
        String toolName{};
        switch (i) {
        case 0: toolName = String("Stamp");
            break;
        case 1: toolName = String("Fill");
            break;
        case 2: toolName = String("Random");
            break;
        }

        Node* toolNode{ guiScene_->CreateChild(toolName) };
        toolNode->AddTag("Tool");
        toolNode->SetPosition(Vector3(-5.5f + i * 0.75f, -4.0f, 0.0f));
        StaticModel* toolModel{ toolNode->CreateComponent<StaticModel>() };
        toolModel->SetModel(GetSubsystem<ResourceMaster>()->GetModel(toolName));
        toolModel->SetMaterial(GetSubsystem<ResourceMaster>()->GetMaterial("VCol"));
    }

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(GUI3D, HandleUpdate));
}

void GUI3D::SetupViewport()
{
    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    Viewport* viewport( new Viewport(context_, guiScene_, guiCamera_) );

    SharedPtr<RenderPath> effectRenderPath{ new RenderPath() };
    effectRenderPath->Load(CACHE->GetResource<XMLFile>("RenderPaths/ForwardNoClear.xml"));

    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath->SetEnabled("FXAA3", true);

    effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/BloomHDR.xml"));
    effectRenderPath->SetShaderParameter("BloomHDRThreshold", 0.42f);
    effectRenderPath->SetShaderParameter("BloomHDRMix", Vector2(0.9f, 0.23f));
    effectRenderPath->SetEnabled("BloomHDR", true);

    viewport->SetRenderPath(effectRenderPath);
    RENDERER->SetViewport(1, viewport);
}

Vector2 GUI3D::WorldToScreenPoint(Vector3 pos) const
{
    return guiCamera_->WorldToScreenPoint(pos) * Vector2(GRAPHICS->GetWidth(), GRAPHICS->GetHeight());
}

void GUI3D::HandleUpdate(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    for (Node* n : guiScene_->GetChildrenWithTag("Tool")) {

        float distanceToCursor{ LucKey::Distance(WorldToScreenPoint(n->GetWorldPosition()),
                                                 Vector2(INPUT->GetMousePosition().x_,
                                                         INPUT->GetMousePosition().y_)) };
        if (distanceToCursor < 64.0f) {

            n->SetScale(1.32f - distanceToCursor * distanceToCursor * 0.000078125f);

        } else {

            n->SetScale(1.0f);
        }
    }
}
