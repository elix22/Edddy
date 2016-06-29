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

#ifndef GRID_H
#define GRID_H

#include <Urho3D/Urho3D.h>

#include "luckey.h"
#include "intvector3.h"
#include "freeblock.h"

#define BLOCK_WIDTH  3.0f
#define BLOCK_HEIGHT 1.0f
#define BLOCK_DEPTH  3.0f
#define BLOCK_SIZE Vector3(BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_DEPTH)

#define MAP_WIDTH  10
#define MAP_HEIGHT 10
#define MAP_DEPTH  10

class Block;
class GridBlock;

typedef HashMap<IntVector2, GridBlock*> Sheet;

class BlockMap : public LogicComponent
{
    URHO3D_OBJECT(BlockMap, LogicComponent);
public:
    BlockMap(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);

    BlockInstance* GetBlockInstance(IntVector3 coords);
    bool SetBlock(IntVector3 coords, Block* block);
private:
    HashMap<int,  Sheet> map_;
//    Vector< Pair<Vector3, FreeBlock> > freeBlocks_;
};

#endif // GRID_H
