#include "editmaster.h"
#include "inputmaster.h"
#include "edddycursor.h"
#include "resourcemaster.h"
#include "block.h"
#include "blockinstance.h"
#include "blockmap.h"


EditMaster::EditMaster(Context* context) : Object(context),
    currentBlockIndex_{0},
    currentBlock_{}
{
}

void EditMaster::LoadBlocks()
{
    PODVector<Model*> models{};
    CACHE->GetResources<Model>(models);
    //Remove non-block models
//    for (Model* model: models){

//        if (!model->GetName().Contains("Blocks", false))
//            models.Remove(model);

//    }

    for (Model* model: models){

        Block* block{ new Block(context_) };
        block->model_ = model;
        block->material_ = RM->GetMaterial("VCol");

        blocks_.Push(block);
    }

}

bool EditMaster::SetCurrentBlock(unsigned index)
{
    if (index < blocks_.Size()){

        currentBlockIndex_ = index;
        currentBlock_ = blocks_[currentBlockIndex_];

        SendEvent(E_CURRENTBLOCKCHANGE);
        return true;
    } else return false;
}
bool EditMaster::SetCurrentBlock(Block* block)
{
    if (blocks_.Contains(block)){
        for (unsigned b{0}; b < blocks_.Size(); ++b){
            if (blocks_[b] == block){
                SetCurrentBlock(b);
                return true;
            }
        }
    }
    return false;
}

void EditMaster::NextBlock()
{
    if (++currentBlockIndex_ == blocks_.Size())
        currentBlockIndex_ = 0;

    SetCurrentBlock(currentBlockIndex_);
}
void EditMaster::PreviousBlock()
{
    if (currentBlockIndex_-- == 0)
        currentBlockIndex_ = blocks_.Size() - 1;

    SetCurrentBlock(currentBlockIndex_);
}

Block* EditMaster::GetBlock(unsigned index)
{
    if (index >= blocks_.Size()) {
        return nullptr;
    } else {
        return blocks_[index];
    }
}
Block* EditMaster::GetCurrentBlock()
{
    return currentBlock_;
}

bool EditMaster::PickBlock()
{
    EdddyCursor* cursor{ GetSubsystem<InputMaster>()->GetCursor() };
    BlockInstance* blockInstance{ MC->GetMap()->GetBlockInstance(cursor->GetCoords()) };
    if (blockInstance && blockInstance->GetBlock()){
        cursor->GetNode()->SetRotation(blockInstance->GetRotation());
        return SetCurrentBlock(blockInstance->GetBlock());
    }
    return false;
}

bool EditMaster::PutBlock(IntVector3 coords, Block* block)
{
    return MC->GetMap()->SetBlock(coords, block);
}
bool EditMaster::PutBlock()
{

    return PutBlock(GetSubsystem<InputMaster>()->GetCursor()->GetCoords(), currentBlock_);
}
