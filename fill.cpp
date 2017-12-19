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

#include "fill.h"

Fill::Fill(Context* context) : Tool(context),
    targetBlock_{},
    replacementBlock_{}
{
    Init();
}


void Fill::Apply(bool shiftDown, bool ctrlDown, bool altDown)
{
    EditMaster* editMaster{ GetSubsystem<EditMaster>() };
    IntVector3 cursorCoords{ GetCursor()->GetCoords() };

    targetBlock_ = editMaster->GetCurrentBlockMap()->GetBlockInstance(cursorCoords)->GetBlock();
    replacementBlock_ = editMaster->GetCurrentBlock();

    HashSet<IntVector3> area{};

    Flood(cursorCoords, area, !shiftDown);

    for (IntVector3 coords: area) {
        editMaster->PutBlock(coords);
    }
    GetSubsystem<History>()->EndStep();
}

void Fill::Flood(IntVector3 coords, HashSet<IntVector3>& area, bool obeyLock)
{
    BlockMap* currentBlockMap{ GetSubsystem<EditMaster>()->GetCurrentBlockMap() };
    Block* oldBlock{ nullptr };
    BlockInstance* instance{ currentBlockMap->GetBlockInstance(coords) };
    if (instance)
        oldBlock = instance->GetBlock();

    if (currentBlockMap->Contains(coords)
     && oldBlock != replacementBlock_ && !area.Contains(coords)
     && oldBlock == targetBlock_)
    {
        area.Insert(coords);

        for (int i{0}; i < 6; ++i) {

            IntVector3 direction{};

            switch (i) {
            case 0:
                direction = IntVector3::RIGHT;
                break;
            case 1:
                direction = IntVector3::LEFT;
                break;
            case 2:
                direction = IntVector3::UP;
                break;
            case 3:
                direction = IntVector3::DOWN;
                break;
            case 4:
                direction = IntVector3::FORWARD;
                break;
            case 5:
                direction = IntVector3::BACK;
                break;
            default:
                break;
            }
            if (obeyLock) {
                std::bitset<3> lock{ GetCursor()->GetAxisLock() };

                direction.x_ *= lock[0];
                direction.y_ *= lock[1];
                direction.z_ *= lock[2];
            }
            if (!direction.Length())
                continue;

            Flood(coords + direction, area, obeyLock);
        }
    }
}

