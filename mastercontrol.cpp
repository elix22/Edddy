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

#include "resourcemaster.h"
#include "inputmaster.h"
#include "castmaster.h"
#include "effectmaster.h"
#include "editmaster.h"
#include "history.h"
#include "guimaster.h"

#include "edddycam.h"
#include "edddycursor.h"

#include "blockmap.h"
#include "block.h"
#include "gridblock.h"
#include "freeblock.h"

#include "gui3d.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl::MasterControl(Context *context):
    Application(context)
{
    EdddyCam::RegisterObject(context_);
    EdddyCursor::RegisterObject(context_);
    BlockMap::RegisterObject(context_);
    GridBlock::RegisterObject(context_);
    FreeBlock::RegisterObject(context_);
}

void MasterControl::Setup()
{
    engineParameters_[EP_WINDOW_TITLE] = "Edddy";
    engineParameters_[EP_WINDOW_ICON] = "icon.png";
    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"edddy.log";
    engineParameters_[EP_RESOURCE_PATHS] = "Resources;";

    engineParameters_[EP_FULL_SCREEN] = false;
    engineParameters_[EP_WINDOW_RESIZABLE] = true;
}

void MasterControl::Start()
{
    ENGINE->SetMaxFps(50);
    ENGINE->SetPauseMinimized(true);
//    ENGINE->SetAutoExit(false);
    SetRandomSeed(TIME->GetSystemTime());
    CACHE->SetAutoReloadResources(true);

    context_->RegisterSubsystem(this);
    context_->RegisterSubsystem(new ResourceMaster(context_));
    context_->RegisterSubsystem(new InputMaster(context_));
    context_->RegisterSubsystem(new CastMaster(context_));
    context_->RegisterSubsystem(new EffectMaster(context_));
    context_->RegisterSubsystem(new EditMaster(context_));
    context_->RegisterSubsystem(new History(context_));

//    LoadBlocks();

    CreateConsoleAndDebugHud();
    CreateScene();
    SubscribeToEvents();

    context_->RegisterSubsystem(new GUIMaster(context_));
}

void MasterControl::LoadBlocks()
{
//    GetSubsystem<ResourceMaster>()->SetResourcePath("Resources");
    String blockPath{ /*GetSubsystem<ResourceMaster>()->GetResourcePath() + */"Resources/Blocks" }; ///Should be relative to map path

    Vector<String> blocksNames{};
    FILES->ScanDir(blocksNames, blockPath, "mdl", SCAN_FILES, true);

    for (String blockName : blocksNames) {

        CACHE->GetResource<Model>(blockPath + blockName);
    }
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
}
void MasterControl::Exit()
{
    //Save settings
    engine_->Exit();
}

void MasterControl::SubscribeToEvents()
{
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MasterControl, HandlePostRenderUpdate));

}

void MasterControl::CreateConsoleAndDebugHud()
{
    Console* console{ engine_->CreateConsole() };
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.8f);

    DebugHud* debugHud{ engine_->CreateDebugHud() };
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateScene()
{
    scene_ = new Scene(context_);

    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

//    scene_->CreateComponent<PhysicsWorld>();
//    scene_->GetComponent<PhysicsWorld>()->SetGravity(Vector3::DOWN * 42.0f);

    //Add sky
//    Node* skyNode{scene_->CreateChild("Sky")};
//    Skybox* skybox{skyNode->CreateComponent<Skybox>()};
//    skybox->SetModel(RM->GetModel("Box"));
//    skybox->SetMaterial(RM->GetMaterial("Skybox"));
//    skybox->GetMaterial()->SetShaderParameter("MatDiffColor", Color(0.5f, 0.6f, 1.0f));

    //Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode{ scene_->CreateChild("DirectionalLight") };
    lightNode->SetPosition(Vector3(-5.0f, 10.0f, -7.0f));
    lightNode->LookAt(Vector3(0.0f, 0.0f, 0.0f));
    Light* light{ lightNode->CreateComponent<Light>() };
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(0.9f);
    light->SetShadowIntensity(0.3f);
    light->SetColor(Color(0.95f, 0.9f, 0.85f));
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.000023f, 0.5f));
    light->SetShadowCascade(CascadeParameters(7.0f, 23.0f, 42.0f, 500.0f, 0.8f));

    RENDERER->SetNumViewports(2);
    //Create camera
    Node* camNode{ scene_->CreateChild("Camera") };
    camera_ = camNode->CreateComponent<EdddyCam>();
    //Create 3D GUI
//    context_->RegisterSubsystem(new GUI3D(context_));

    //Create the cursor
    Node* cursorNode{ scene_->CreateChild("Cursor") };
    EdddyCursor* cursor{ cursorNode->CreateComponent<EdddyCursor>() };
    GetSubsystem<InputMaster>()->SetCursor(cursor);

    //Create a map
//    Node* mapNode{ scene_->CreateChild("Map") };
//    blockMap_ = mapNode->CreateComponent<BlockMap>();
    if (!GetSubsystem<EditMaster>()->LoadMap(BLOCKMAP))
        GetSubsystem<EditMaster>()->NewMap();

//    GetSubsystem<EditMaster>()->SaveBlockSet();
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
    float phase{ SinePhase(freq, shift) };
    float add{ 0.5f * (min + max) };
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
