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

#include "mastercontrol.h"
#include "edddycam.h"
#include "resourcemaster.h"
#include "inputmaster.h"
#include "castmaster.h"
#include "effectmaster.h"

#include "blockmap.h"
#include "block.h"
#include "gridblock.h"
#include "freeblock.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl::MasterControl(Context *context):
    Application(context)
{
    EdddyCam::RegisterObject(context_);
    BlockMap::RegisterObject(context_);
    GridBlock::RegisterObject(context_);
    FreeBlock::RegisterObject(context_);
}

void MasterControl::Setup()
{
    engineParameters_["WindowTitle"] = "Edddy";
    engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"edddy.log";
    engineParameters_["ResourcePaths"] = "Data;CoreData;Resources";
    engineParameters_["WindowIcon"] = "icon.png";
}
void MasterControl::Start()
{
    ENGINE->SetMaxFps(42);
    SetRandomSeed(TIME->GetSystemTime());

    context_->RegisterSubsystem(this);
    context_->RegisterSubsystem(new ResourceMaster(context_));
    context_->RegisterSubsystem(new InputMaster(context_));
    context_->RegisterSubsystem(new CastMaster(context_));
    context_->RegisterSubsystem(new EffectMaster(context_));

    defaultStyle_ = CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml");
    CreateConsoleAndDebugHud();
    CreateScene();
    CreateUI();
    SubscribeToEvents();
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
}
void MasterControl::Exit()
{
    engine_->Exit();
}

void MasterControl::SubscribeToEvents()
{
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MasterControl, HandlePostRenderUpdate));

}

void MasterControl::CreateConsoleAndDebugHud()
{
    Console* console{engine_->CreateConsole()};
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.8f);

    DebugHud* debugHud{engine_->CreateDebugHud()};
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateUI()
{
//    cache_ = GetSubsystem<ResourceCache>();
//    UI* ui = GetSubsystem<UI>();

//    world_.cursor.uiCursor = new Cursor(context_);
//    world_.cursor.uiCursor->SetVisible(false);
//    ui->SetCursor(world_.cursor.uiCursor);
//    world_.cursor.uiCursor->SetPosition(graphics_->GetWidth()/2, graphics_->GetHeight()/2);
}

void MasterControl::CreateScene()
{
    scene_ = new Scene(context_);

    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

    scene_->CreateComponent<PhysicsWorld>();
    scene_->GetComponent<PhysicsWorld>()->SetGravity(Vector3::DOWN * 42.0f);

    //Add sky
//    Node* skyNode{scene_->CreateChild("Sky")};
//    Skybox* skybox{skyNode->CreateComponent<Skybox>()};
//    skybox->SetModel(RM->GetModel("Box"));
//    skybox->SetMaterial(RM->GetMaterial("Skybox"));
//    skybox->GetMaterial()->SetShaderParameter("MatDiffColor", Color(0.5f, 0.6f, 1.0f));

    //Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode{scene_->CreateChild("DirectionalLight")};
    lightNode->SetPosition(Vector3(-5.0f, 10.0f, -7.0f));
    lightNode->LookAt(Vector3(0.0f, 0.0f, 0.0f));
    Light* light{lightNode->CreateComponent<Light>()};
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(0.9f);
    light->SetShadowIntensity(0.3f);
    light->SetColor(Color(0.95f, 0.9f, 0.85f));
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.000023f, 0.5f));
    light->SetShadowCascade(CascadeParameters(7.0f, 23.0f, 42.0f, 500.0f, 0.8f));

    //Create camera
    Node* camNode{scene_->CreateChild("Camera")};
    camera_ = camNode->CreateComponent<EdddyCam>();

    //Create a map
    Node* mapNode{ scene_->CreateChild("Map") };
    mapNode->CreateComponent<BlockMap>();

}

void MasterControl::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;
//    if (INPUT->GetKeyPress(KEY_SPACE))
//        drawDebug_ = !drawDebug_;


    if (drawDebug_)
    {
        scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
    }
}





float MasterControl::Sine(const float freq, const float min, const float max, const float shift)
{
    float phase{SinePhase(freq, shift)};
    float add{0.5f * (min + max)};
    return LucKey::Sine(phase) * 0.5f * (max - min) + add;
}
float MasterControl::Cosine(const float freq, const float min, const float max, const float shift)
{
    return Sine(freq, min, max, shift + 0.25f);
}
float MasterControl::SinePhase(float freq, float shift)
{
    return M_PI * 2.0f * (freq * scene_->GetElapsedTime() + shift);
}
