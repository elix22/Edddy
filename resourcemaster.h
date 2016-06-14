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

#ifndef RESOURCEMASTER_H
#define RESOURCEMASTER_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class ResourceMaster : public Object
{
    URHO3D_OBJECT(ResourceMaster, Object);
public:
    ResourceMaster(Context* context);
    Model*GetModel(String model);
    Material*GetMaterial(String material);
};

#endif // RESOURCEMASTER_H
