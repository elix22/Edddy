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

#ifndef BLOCKMAP_H
#define BLOCKMAP_H

#include <Urho3D/Urho3D.h>

#include "luckey.h"
#include "intvector3.h"
#include "freeblock.h"

#define BLOCK_WIDTH  3.0f
#define BLOCK_HEIGHT 1.0f
#define BLOCK_DEPTH  3.0f
#define BLOCK_SIZE Vector3(BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_DEPTH)

class Block;
class GridBlock;

typedef HashMap<IntVector2, GridBlock*> Sheet;

class BlockMap : public LogicComponent
{
    friend class EditMaster;

    URHO3D_OBJECT(BlockMap, LogicComponent);
public:
    BlockMap(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);

    IntVector3 GetMapSize() const { return mapSize_; }
    int GetMapWidth()       const { return mapSize_.x_; }
    int GetMapHeight()      const { return mapSize_.y_; }
    int GetMapDepth()       const { return mapSize_.z_; }

    Vector3 GetBlockSize() const { return blockSize_; }
    int GetBlockWidth()    const { return blockSize_.x_; }
    int GetBlockHeight()   const { return blockSize_.y_; }
    int GetBlockDepth()    const { return blockSize_.z_; }

    Vector3 GetCenter();
    BlockInstance* GetBlockInstance(IntVector3 coords);
    void SetBlock(IntVector3 coords, Quaternion rotation, Block* block);
    void LoadXML(const XMLElement &source);
    void SaveXML(XMLElement &dest);
    Vector<Block*> GetUsedBlocks();
    Vector<BlockSet*> GetUsedBlockSets();
    Vector<GridBlock*> GetOccupiedGridBlocks();
private:
    HashMap<int, Sheet> map_;

    IntVector3 mapSize_;
    Vector3 blockSize_;

    void SetMapSize(IntVector3 size);
    void SetMapSize(int w, int h, int d);
    void CreateCorners();
    void Initialize();
    void SetBlockSize(Vector3 size);
    int GetBlockSetId(Block* block, HashMap<int, BlockSet*>& blockSetsById);
};

#endif // BLOCKMAP_H
