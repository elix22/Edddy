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
#include "resourcemaster.h"
#include "block.h"
#include "blockinstance.h"
#include "blockmap.h"
#include "blockset.h"
#include "edddyevents.h"
#include "history.h"

#include "editmaster.h"

EditMaster::EditMaster(Context* context) : Object(context),
    blockMaps_{},
    currentBlockMap_{},
    currentBlockIndex_{0},
    currentBlock_{},
    currentBlockSet_{}
{
}

void EditMaster::NewMap(const IntVector3& mapSize, const Vector3& blockSize)
{
    BlockMap* newBlockMap{ MC->GetScene()->CreateChild("BlockMap")->CreateComponent<BlockMap>() };

    newBlockMap->SetMapSize(mapSize);
    newBlockMap->SetBlockSize(blockSize);
    newBlockMap->Initialize();

    blockMaps_.Push(newBlockMap);
    SetCurrentBlockMap(newBlockMap);

    if (!blockSets_.Size()) {
        LoadBlocks();
    }
}

bool EditMaster::LoadMap(String fileName)
{
    if (!FILES->FileExists(fileName))
        return false;

    File file{ MC->GetContext(), fileName, FILE_READ };
    XMLFile* mapXML{ new XMLFile(MC->GetContext()) };
    mapXML->Load(file);
    file.Close();

    XMLElement rootElem{ mapXML->GetRoot("blockmap") };

    if (rootElem) {

        BlockMap* newBlockMap{ MC->GetScene()->CreateChild("BlockMap")->CreateComponent<BlockMap>() };
        newBlockMap->LoadXML(rootElem);

        blockMaps_.Push(newBlockMap);
        SetCurrentBlockMap(newBlockMap);

        if (!blockSets_.Size())
            LoadBlocks();

        return true;
    }
    return false;
}

void EditMaster::SaveMap(BlockMap* blockMap, String fileName)
{

    XMLFile* mapXML{ new XMLFile(MC->GetContext()) };
    XMLElement rootElem{ mapXML->CreateRoot("blockmap") };

    blockMap->SaveXML(rootElem);

    File file{ MC->GetContext(), fileName, FILE_WRITE };
    mapXML->Save(file);
    file.Close();
}

void EditMaster::LoadBlocks()
{
    if (FILES->FileExists(BLOCKSET)) {

        LoadBlockSet(BLOCKSET);

    } else {

        PODVector<Model*> models{};
        CACHE->GetResources<Model>(models);
        //Remove non-block models
        for (Model* model: models){

            if (!model->GetName().Contains("Blocks/"))
                models.Remove(model);

        }

        BlockSet* newBlockSet{ new BlockSet(context_) };
        newBlockSet->name_ = "Resources/TestSet.xml";

        for (Model* model: models){

            Block* block{ new Block(context_) };
            block->SetId(newBlockSet->blocks_.Size());
            block->SetModel(model);
            block->SetMaterial(GetSubsystem<ResourceMaster>()->GetMaterial("VCol"));

            newBlockSet->blocks_.Push(block);
        }

        blockSets_.Push(newBlockSet);
        SetCurrentBlockSet(newBlockSet);
    }
}
BlockSet* EditMaster::LoadBlockSet(String fileName)
{
    if (!FILES->FileExists(fileName))
        return nullptr;

    File file{ MC->GetContext(), fileName, FILE_READ };

    XMLFile* blockSetXML{ new XMLFile(MC->GetContext()) };
    blockSetXML->Load(file);
    XMLElement rootElem{ blockSetXML->GetRoot("blockset") };

    BlockSet* newBlockSet{ new BlockSet(context_) };
    newBlockSet->name_ = rootElem.GetAttribute("name");

    XMLElement blockXML{ rootElem.GetChild("block") };
    while (blockXML) {

        Block* block{ new Block(context_) };
        block->SetId(blockXML.GetUInt("id"));
        block->SetModel(CACHE->GetResource<Model>(blockXML.GetAttribute("model")));
        block->SetMaterial(CACHE->GetResource<Material>(blockXML.GetAttribute("material")));

        newBlockSet->blocks_.Push(block);

        blockXML = blockXML.GetNext("block");
    }

    blockSets_.Push(newBlockSet);
    SetCurrentBlockSet(newBlockSet);

    return newBlockSet;
}

void EditMaster::SaveBlockSet(BlockSet* blockSet, String fileName)
{
    XMLFile* blockSetXML{ new XMLFile(MC->GetContext()) };
    XMLElement rootElem{ blockSetXML->CreateRoot("blockset") };
    rootElem.SetString("name", fileName);

    for (Block* b : blockSet->blocks_) {

        b->SaveXML(rootElem);

    }

    File file{ MC->GetContext(), fileName, FILE_WRITE };
    blockSetXML->Save(file);
    file.Close();
}

void EditMaster::SetCurrentBlockMap(BlockMap* map)
{
    if (!map)
        return;
    else if (currentBlockMap_) {
        currentBlockMap_->GetNode()->SetEnabledRecursive(false);
    }

    currentBlockMap_ = map;

    SetCurrentBlock(nullptr);

    VariantMap eventData{};
    eventData[CurrentMapChange::P_MAP] = currentBlockMap_;

    SendEvent(E_CURRENTMAPCHANGE, eventData);
}

void EditMaster::SetCurrentBlockSet(BlockSet* blockSet)
{
    currentBlockSet_ = blockSet;
}

void EditMaster::SetCurrentBlock(unsigned index, BlockSet* blockSet)
{
    if (!blockSet)
        return;
    else if (currentBlockSet_ != blockSet)
        currentBlockSet_ = blockSet;

    currentBlockIndex_ = Clamp(index, (unsigned)0, currentBlockSet_->blocks_.Size());
    currentBlock_ = currentBlockSet_->blocks_[currentBlockIndex_];

    VariantMap eventData{};
    eventData[CurrentBlockChange::P_BLOCK] = currentBlock_;

    SendEvent(E_CURRENTBLOCKCHANGE, eventData);
}

void EditMaster::SetCurrentBlock(unsigned index)
{
    SetCurrentBlock(index, currentBlockSet_);
}
void EditMaster::SetCurrentBlock(Block* block)
{
    if (block == nullptr) {

        VariantMap eventData{};
        eventData[CurrentBlockChange::P_BLOCK] = currentBlock_ = nullptr;

        SendEvent(E_CURRENTBLOCKCHANGE, eventData);

    } else for (BlockSet* blockSet : blockSets_) {
        if (blockSet->blocks_.Contains(block)){
            for (unsigned b{0}; b < blockSet->blocks_.Size(); ++b){
                if (blockSet->blocks_[b] == block){
                    SetCurrentBlockSet(blockSet);
                    SetCurrentBlock(b);

                    return;
                }
            }
        }
    }
}

void EditMaster::NextBlock()
{
    if (!currentBlock_) {

        SetCurrentBlock(currentBlockIndex_);
        return;
    }

    if (++currentBlockIndex_ >= currentBlockSet_->blocks_.Size())
        currentBlockIndex_ = 0;

    SetCurrentBlock(currentBlockIndex_);
}
void EditMaster::PreviousBlock()
{
    if (!currentBlock_){

        SetCurrentBlock(currentBlockIndex_);
        return;
    }

    if (currentBlockIndex_-- == 0)
        currentBlockIndex_ = currentBlockSet_->blocks_.Size() - 1;

    SetCurrentBlock(currentBlockIndex_);
}

Block* EditMaster::GetBlock(unsigned index)
{
    if (index >= currentBlockSet_->blocks_.Size()) {
        return nullptr;
    } else {
        return currentBlockSet_->blocks_[index];
    }
}
Block* EditMaster::GetCurrentBlock()
{
    return currentBlock_;
}

void EditMaster::PickBlock()
{
    EdddyCursor* cursor{ GetSubsystem<InputMaster>()->GetCursor() };
    BlockInstance* blockInstance{ GetCurrentBlockMap()->GetBlockInstance(cursor->GetCoords()) };
    if (blockInstance && blockInstance->GetBlock()) {

        cursor->SetRotation(blockInstance->GetRotation());
        SetCurrentBlock(blockInstance->GetBlock());
    } else {

        SetCurrentBlock(nullptr);
    }
}

void EditMaster::PutBlock(IntVector3 coords, Quaternion rotation, Block* block)
{
    Change change{};
    change.position_.first_ = change.position_.second_ = Vector3(coords);

    change.instance_ = GetCurrentBlockMap()->GetBlockInstance(coords);
    change.block_.first_ = change.instance_->GetBlock();
    change.rotation_.first_ = change.instance_->GetRotation();

    GetCurrentBlockMap()->SetBlock(coords, rotation, block);

    change.block_.second_ = change.instance_->GetBlock();
    change.rotation_.second_ = change.instance_->GetRotation();

    if (change.Any()) {

        GetSubsystem<History>()->AddChange(change);
        GetSubsystem<History>()->EndStep();
    }
}
void EditMaster::PutBlock()
{
    EdddyCursor* cursor{ GetSubsystem<InputMaster>()->GetCursor() };

    if (!currentBlockMap_ || cursor->IsHidden())
        return;

    PutBlock(cursor->GetCoords(), cursor->GetTargetRotation(), currentBlock_);
}
