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

#ifndef EDITMASTER_H
#define EDITMASTER_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"


URHO3D_EVENT(E_CURRENTBLOCKCHANGE, CurrentBlockChange){
    URHO3D_PARAM(P_BLOCK, Block*); // Block pointer
}
URHO3D_EVENT(E_CURRENTMAPCHANGE, CurrentMapChange){
    URHO3D_PARAM(P_MAP, BlockMap*); // BlockMap pointer
}
URHO3D_EVENT(E_CURRENTTOOLCHANGE, CurrentToolChange){
    URHO3D_PARAM(P_TOOL, Tool*); // Tool pointer
}

#define BLOCKSET "Resources/TestSet.xml"
#define BLOCKMAP "Resources/TestMap.xml"

class Block;
class BlockSet;
class BlockInstance;
class Tool;

struct BlockChange{
    BlockInstance* instance_;
    Pair<Block*, Block*> blockChange_;
    Pair<Quaternion, Quaternion> blockRotation_;
};
typedef Vector<BlockChange> UndoStep;

class EditMaster : public Object
{
    URHO3D_OBJECT(EditMaster, Object);
public:
    EditMaster(Context* context);
    void NewMap(const IntVector3& mapSize = IntVector3(23, 5, 23), const Vector3& blockSize = Vector3(3.0f, 1.0f, 3.0f));
    bool LoadMap(String fileName);
    void SaveMap(BlockMap *blockMap, String fileName);

    void LoadBlocks();
    BlockSet* LoadBlockSet(String fileName);
    void SaveBlockSet(BlockSet* blockSet, String fileName);

    void SetCurrentBlockMap(BlockMap* map);
    BlockMap* GetCurrentBlockMap() const { return currentBlockMap_; }

    void SetCurrentBlockSet(BlockSet* blockSet);
    const Vector<BlockSet*>& GetBlockSets() const { return blockSets_; }

    void NextBlock();
    void PreviousBlock();
    void SetCurrentBlock(unsigned index, BlockSet* blockSet);
    void SetCurrentBlock(unsigned index);
    void SetCurrentBlock(Block* block);

    Block* GetBlock(unsigned index);
    Block* GetCurrentBlock();
    void PutBlock(IntVector3 coords, Quaternion rotation, Block* block);
    void PutBlock(IntVector3 coords);
    void PutBlock();
    void PickBlock();
    void ClearBlock(IntVector3 coords);
    void ClearBlock();

    void SetTool(Tool* tool);
    Tool* GetTool() const { return currentTool_; }
    StringHash GetLastToolType() const { return lastTool_; }

    void ApplyTool(bool shiftDown, bool ctrlDown, bool altDown);
private:
    Vector<BlockMap*> blockMaps_;
    BlockMap* currentBlockMap_;

    Vector<BlockSet*> blockSets_;
    unsigned currentBlockIndex_;
    Block* currentBlock_;
    BlockSet* currentBlockSet_;
    Tool* currentTool_;

    Vector<UndoStep> history_;
    unsigned historyIndex_;
    StringHash lastTool_;
    void CreateTools();
};

#endif // EDITMASTER_H
