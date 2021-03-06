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

#include "editmaster.h"
#include "history.h"
#include "edddycursor.h"
#include "blockmap.h"

#include "brush.h"

Brush::Brush(Context* context) : Tool(context)
{
    Init();
}

void Brush::Apply(bool shiftDown, bool ctrlDown, bool altDown)
{
    EditMaster* editMaster{ GetSubsystem<EditMaster>() };
    IntVector3 cursorCoords{ GetCursor()->GetCoords() };

    if (shiftDown && IsLastTool()) {

        for (const IntVector3& coords: BresenhamLine(lastCoords_, cursorCoords)) {
            editMaster->PutBlock(coords);
        }
    } else {

        editMaster->PutBlock(cursorCoords);
    }
    GetSubsystem<History>()->EndStep();

    lastCoords_ = cursorCoords;
}

void Brush::UpdatePreview(bool shiftDown, bool ctrlDown, bool altDown)
{
    Tool::UpdatePreview(shiftDown, ctrlDown, altDown);


    if (shiftDown && IsLastTool()) {
        EditMaster* editMaster{ GetSubsystem<EditMaster>() };
        IntVector3 cursorCoords{ GetCursor()->GetCoords() };

        for (const IntVector3& coords: BresenhamLine(lastCoords_, cursorCoords)) {

            if (coords != cursorCoords)
                GetCursor()->AddInstanceNode(editMaster->GetCurrentBlockMap()->GetBlockInstance(coords)->GetNode(), GetCursor()->GetTargetRotation());
        }
    }
}
