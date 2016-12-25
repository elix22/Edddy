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

#ifndef MASTERCONTROL_H
#define MASTERCONTROL_H


#include <Urho3D/Urho3D.h>
#include "luckey.h"
#include "edddyevents.h"

using namespace Urho3D;

class ResourceMaster;
class EffectMaster;
class InputMaster;
class EdddyCam;
class BlockMap;

class MasterControl : public Application
{
    URHO3D_OBJECT(MasterControl, Application);
public:
    MasterControl(Context* context);

    virtual void Setup();
    virtual void Start();
    virtual void Stop();
    void Exit();


    Scene* GetScene() { return scene_; }
    EdddyCam* GetCamera() { return camera_; }

    float Sine(const float freq, const float min, const float max, const float shift = 0.0f);
    float Cosine(const float freq, const float min, const float max, const float shift = 0.0f);
    SharedPtr<EdddyCam> camera_;
private:
    SharedPtr<UI> ui_;
    SharedPtr<Renderer> renderer_;
    SharedPtr<XMLFile> defaultStyle_;

    SharedPtr<Scene> scene_;

    void CreateConsoleAndDebugHud();
    void CreateScene();
    void CreateUI();
    void SubscribeToEvents();

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    void CreatePlatform(const Vector3 pos);
    void UpdateCursor(float timeStep);

    bool drawDebug_{false};
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
    float SinePhase(float freq, float shift);
    void LoadBlocks();
};

#endif // MASTERCONTROL_H
