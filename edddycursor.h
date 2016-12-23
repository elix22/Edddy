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

#ifndef EDDDYCURSOR_H
#define EDDDYCURSOR_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

URHO3D_EVENT(E_CURSORSTEP, CursorStep){}

class EdddyCursor : public LogicComponent
{
    URHO3D_OBJECT(EdddyCursor, LogicComponent);
public:
    EdddyCursor(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void DelayedStart();

    void UpdateSizeAndOffset();
    void SetAxisLock(std::bitset<3> lock);
    std::bitset<3> GetAxisLock() { return axisLock_; }

    void Step(IntVector3 step);
    void SetCoords(IntVector3 coords);
    IntVector3 GetCoords() const { return coords_; }
    void MoveTo(Vector3 position);

    void Rotate(bool clockWise);
    void SetRotation(Quaternion rot);
    Quaternion GetTargetRotation() const { return targetRotation_; }

    void HandleMouseMove();
private:
    IntVector3 coords_;
    Quaternion targetRotation_;

    Node* boxNode_;
    StaticModel* boxModel_;
    Node* blockNode_;
    StaticModel* blockModel_;

    std::bitset<3> axisLock_;
    std::bitset<3> previousAxisLock_;
    AnimatedModel* hitPlanes_;

    void UpdateHitPlanes();
    void UpdateModel(StringHash eventType, VariantMap& eventData);
};


#endif // EDDDYCURSOR_H
