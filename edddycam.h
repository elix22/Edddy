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

#ifndef EDDDYCAM_H
#define EDDDYCAM_H

#include <Urho3D/Urho3D.h>

#include "luckey.h"

using namespace Urho3D;

#define PITCH_MAX 90.0f
#define PITCH_MIN -90.0f

enum MoveType { MT_ROTATE, MT_PAN, MT_FOV };

class EdddyCam : public LogicComponent
{
    URHO3D_OBJECT(EdddyCam, LogicComponent);
public:
    EdddyCam(Context *context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node) override;
    void Update(float timeStep) override;
    void HandleMapChange(StringHash eventType, VariantMap& eventData);

    Quaternion GetRotation() const { return node_->GetRotation(); }
    float GetYaw() const { return GetRotation().EulerAngles().y_; }
    float GetPitch() const { return GetRotation().EulerAngles().x_; }
    Vector3 GetPosition() const { return node_->GetPosition(); }
    float GetFov() const { return camera_->GetFov(); }
    Ray GetScreenRay(float x, float y) const { return camera_->GetScreenRay(x, y); }
    void Move(Vector3 movement, MoveType type = MT_ROTATE);
    void Move(IntVector3 movement, MoveType type = MT_ROTATE);
    void ToggleOrthogaphic();

private:
    SharedPtr<Camera> camera_;
    SharedPtr<Viewport> viewport_;
    SharedPtr<RenderPath> effectRenderPath_;

    void SetupViewport();
    void ClampPitch(float& pitchDelta);
};

#endif // EDDDYCAM_H
