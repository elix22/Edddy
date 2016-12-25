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

#include "edddycam.h"
#include "edddycursor.h"

#include "blockmap.h"
#include "block.h"
#include "gridblock.h"
#include "freeblock.h"

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
    engineParameters_[EP_RESOURCE_PATHS] = "Data;CoreData;Resources;";

//    engineParameters_[EP_FULL_SCREEN] = false;
//    engineParameters_[EP_WINDOW_HEIGHT] = 1280;
//    engineParameters_[EP_WINDOW_WIDTH] = 1024;
}


void MasterControl::Start()
{
    ENGINE->SetMaxFps(80);
    SetRandomSeed(TIME->GetSystemTime());
    CACHE->SetAutoReloadResources(true);
    LoadBlocks();

    context_->RegisterSubsystem(this);
    context_->RegisterSubsystem(new ResourceMaster(context_));
    context_->RegisterSubsystem(new InputMaster(context_));
    context_->RegisterSubsystem(new CastMaster(context_));
    context_->RegisterSubsystem(new EffectMaster(context_));
    context_->RegisterSubsystem(new EditMaster(context_));

    defaultStyle_ = CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml");
    CreateConsoleAndDebugHud();
    CreateScene();
    CreateUI();
    SubscribeToEvents();
}
void MasterControl::LoadBlocks()
{
    CACHE->GetResource<Model>("Blocks/CliffLowBend.mdl");
    CACHE->GetResource<Model>("Blocks/CliffBottomBend.mdl");
    CACHE->GetResource<Model>("Blocks/CliffBottomCorner.mdl");
    CACHE->GetResource<Model>("Blocks/CliffBottomEdge.mdl");
    CACHE->GetResource<Model>("Blocks/CliffBottomHillBendLeft.mdl");
    CACHE->GetResource<Model>("Blocks/CliffBottomHillBendRight.mdl");
    CACHE->GetResource<Model>("Blocks/CliffBottomHillLeft.mdl");
    CACHE->GetResource<Model>("Blocks/CliffBottomHillRight.mdl");
    CACHE->GetResource<Model>("Blocks/CliffLowBend.mdl");
    CACHE->GetResource<Model>("Blocks/CliffLowCorner.mdl");
    CACHE->GetResource<Model>("Blocks/CliffLowEdge.mdl");
    CACHE->GetResource<Model>("Blocks/CliffMiddleBend.mdl");
    CACHE->GetResource<Model>("Blocks/CliffMiddleCorner.mdl");
    CACHE->GetResource<Model>("Blocks/CliffMiddleEdge.mdl");
    CACHE->GetResource<Model>("Blocks/CliffMiddleTopBendLeft.mdl");
    CACHE->GetResource<Model>("Blocks/CliffMiddleTopBendRight.mdl");
    CACHE->GetResource<Model>("Blocks/CliffMiddleTopLeft.mdl");
    CACHE->GetResource<Model>("Blocks/CliffMiddleTopRight.mdl");
    CACHE->GetResource<Model>("Blocks/CliffToHillLeft.mdl");
    CACHE->GetResource<Model>("Blocks/CliffToHillRight.mdl");
    CACHE->GetResource<Model>("Blocks/CliffTopBend.mdl");
    CACHE->GetResource<Model>("Blocks/CliffTopCorner.mdl");
    CACHE->GetResource<Model>("Blocks/CliffTopEdge.mdl");
    CACHE->GetResource<Model>("Blocks/Flat.mdl");
    CACHE->GetResource<Model>("Blocks/HillBend.mdl");
    CACHE->GetResource<Model>("Blocks/HillCorner.mdl");
    CACHE->GetResource<Model>("Blocks/HillEdge.mdl");
    CACHE->GetResource<Model>("Blocks/RampBottomCenter.mdl");
    CACHE->GetResource<Model>("Blocks/RampBottomLeft.mdl");
    CACHE->GetResource<Model>("Blocks/RampBottomRight.mdl");
    CACHE->GetResource<Model>("Blocks/RampMiddleCenter.mdl");
    CACHE->GetResource<Model>("Blocks/RampMiddleLeft.mdl");
    CACHE->GetResource<Model>("Blocks/RampMiddleRight.mdl");
    CACHE->GetResource<Model>("Blocks/RampTopCenter.mdl");
    CACHE->GetResource<Model>("Blocks/RampTopLeft.mdl");
    CACHE->GetResource<Model>("Blocks/RampTopRight.mdl");
    CACHE->GetResource<Model>("Blocks/ToRampMiddleLeft.mdl");
    CACHE->GetResource<Model>("Blocks/ToRampMiddleRight.mdl");
    CACHE->GetResource<Model>("Blocks/ToRampTopLeft.mdl");
    CACHE->GetResource<Model>("Blocks/ToRampTopRight.mdl");
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
}
void MasterControl::Exit()
{
    GetSubsystem<EditMaster>()->SaveMap(GetSubsystem<EditMaster>()->GetCurrentBlockMap(), BLOCKMAP);

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

    //Create camera
    Node* camNode{ scene_->CreateChild("Camera") };
    camera_ = camNode->CreateComponent<EdddyCam>();

    //Create a map
//    Node* mapNode{ scene_->CreateChild("Map") };
//    blockMap_ = mapNode->CreateComponent<BlockMap>();
    if (!GetSubsystem<EditMaster>()->LoadMap(BLOCKMAP))
        GetSubsystem<EditMaster>()->NewMap();

    //Create the cursor
    Node* cursorNode{ scene_->CreateChild("Cursor") };
    EdddyCursor* cursor{ cursorNode->CreateComponent<EdddyCursor>() };
    GetSubsystem<InputMaster>()->SetCursor(cursor);

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
