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



#ifndef GUI3D_H
#define GUI3D_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class GUI3D : public Object
{
    URHO3D_OBJECT(GUI3D, Object);
public:
    GUI3D(Context* context);
    Vector2 WorldToScreenPoint(Vector3 pos) const;
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
private:
    void SetupViewport();

    Scene* guiScene_;
    Camera* guiCamera_;
};

#endif // GUI3D_H
