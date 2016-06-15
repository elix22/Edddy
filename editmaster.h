#ifndef EDITMASTER_H
#define EDITMASTER_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

URHO3D_EVENT(E_CURRENTBLOCKCHANGE, CurrentBlockChange){
    URHO3D_PARAM(P_BLOCK, Block); // Block pointer
}

class Block;

class EditMaster : public Object
{
    URHO3D_OBJECT(EditMaster, Object);
public:
    EditMaster(Context* context);
    void LoadBlocks();

    void NextBlock();
    void PreviousBlock();
    bool SetCurrentBlock(unsigned index);
    bool SetCurrentBlock(Block* block);

    Block* GetBlock(unsigned index);
    Block* GetCurrentBlock();
    bool PutBlock(IntVector3 coords, Block* block);
    bool PutBlock();
    bool PickBlock();
private:
    Vector<Block*> blocks_;
    unsigned currentBlockIndex_;
    Block* currentBlock_;
};

#endif // EDITMASTER_H
