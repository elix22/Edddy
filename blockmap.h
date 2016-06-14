#ifndef GRID_H
#define GRID_H

#include <Urho3D/Urho3D.h>

#include "luckey.h"
#include "intvector3.h"
#include "freeblock.h"

#define BLOCK_WIDTH  3.0f
#define BLOCK_HEIGHT 1.0f
#define BLOCK_DEPTH  3.0f

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

    Block* GetBlock(IntVector3 coords);
    void SetBlock(IntVector3 coords, Block* block);
private:
    HashMap<int,  Sheet> map_;
//    Vector< Pair<Vector3, FreeBlock> > freeBlocks_;
};

#endif // GRID_H
