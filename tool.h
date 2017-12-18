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

#ifndef TOOL_H
#define TOOL_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class EdddyCursor;

class Tool : public Object
{
    URHO3D_OBJECT(Tool, Object);
public:
    Tool(Context* context);

    virtual void Apply(bool shiftDown, bool ctrlDown, bool altDown) = 0;

    bool IsLastTool();
protected:
    PODVector<IntVector3> BresenhamLine(const IntVector3 start, const IntVector3 end);
    IntVector3 GetCursorCoords();
};

#endif // TOOL_H
