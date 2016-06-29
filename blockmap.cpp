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

#include "inputmaster.h"
#include "edddycursor.h"
#include "gridblock.h"
#include "blockmap.h"

namespace Urho3D {

    template <> unsigned MakeHash(const IntVector2& value)
    {
        return LucKey::IntVector2ToHash(value);
    }
}


void BlockMap::RegisterObject(Context *context){ context->RegisterFactory<BlockMap>(); }

BlockMap::BlockMap(Context* context) : LogicComponent(context),
    map_{}
{
}
void BlockMap::OnNodeSet(Node *node)
{ (void)node;

    for (int y{0}; y < MAP_HEIGHT; ++y){
        Sheet sheet{};
        for (int x{0}; x < MAP_WIDTH; ++x)
            for (int z{0}; z < MAP_DEPTH; ++z){
                Node* gridNode_{ node_->CreateChild("GridBlock") };
                GridBlock* gridBlock{ gridNode_->CreateComponent<GridBlock>() };
                gridBlock->Init(IntVector3(x, y, z));

                IntVector2 sheetCoords(x, z);
                sheet[sheetCoords] = gridBlock;
            }
        map_[y] = sheet;
    }
}

BlockInstance* BlockMap::GetBlockInstance(IntVector3 coords)
{
    BlockInstance* blockInstance{ nullptr };
    Sheet sheet{};

    if (map_.TryGetValue(coords.y_, sheet)){
        GridBlock* gridBlock{ nullptr };
        if (sheet.TryGetValue(IntVector2(coords.x_, coords.z_), gridBlock))
            blockInstance = static_cast<BlockInstance*>(gridBlock);
    }

    return blockInstance;
}

bool BlockMap::SetBlock(IntVector3 coords, Block* block)
{

    Sheet sheet{};
    if (map_.TryGetValue(coords.y_, sheet)){

        GridBlock* gridBlock{ nullptr };
        if (sheet.TryGetValue(IntVector2(coords.x_, coords.z_), gridBlock))
            return gridBlock->SetBlock(block, GetSubsystem<InputMaster>()->GetCursor()->GetNode()->GetRotation());
    }
    return false;
}


