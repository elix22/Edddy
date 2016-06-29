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
    for (Model* model: models){

        if (!model->GetName().Contains("Blocks", false))
            models.Remove(model);

    }

    for (Model* model: models){

        Block* block{ new Block(context_) };
        block->model_ = model;
        block->material_ = GetSubsystem<ResourceMaster>()->GetMaterial("VCol");

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
