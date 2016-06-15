#ifndef BLOCKINSTANCE_H
#define BLOCKINSTANCE_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"
#include "block.h"

class BlockInstance : public LogicComponent
{
    URHO3D_OBJECT(BlockInstance, LogicComponent);
public:
    BlockInstance(Context* context);
    virtual void OnNodeSet(Node* node);
    bool SetBlock(Block* block, Quaternion rotation);
    Block* GetBlock() const { return block_; }
    Quaternion GetRotation() const { return rotation_; }
protected:
    Block* block_;

    Node* blockNode_;
    Quaternion rotation_;
    StaticModel* blockModel_;
    Material* blockMaterial_;
};

#endif // BLOCKINSTANCE_H
