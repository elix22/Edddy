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
#include "resourcemaster.h"
#include "edddycursor.h"
#include "gridblock.h"
#include "editmaster.h"
#include "blockset.h"

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
{ if (!node) return;

    CreateCorners();
    node_->SetEnabled(false);
}
void BlockMap::Initialize()
{
    for (int y{0}; y < GetMapHeight(); ++y){
        Sheet sheet{};
        for (int x{0}; x < GetMapWidth(); ++x)
            for (int z{0}; z < GetMapDepth(); ++z){
                Node* gridNode{ node_->CreateChild("GridBlock") };
                GridBlock* gridBlock{ gridNode->CreateComponent<GridBlock>() };
                gridBlock->Init(IntVector3(x, y, z));

                IntVector2 sheetCoords(x, z);
                sheet[sheetCoords] = gridBlock;
            }
        map_[y] = sheet;
    }
    node_->SetEnabled(true);
}

void BlockMap::CreateCorners()
{
    for (int c{0}; c < 8; ++c) {

        Node* cornerNode{ node_->CreateChild("MapCorner") };

        StaticModel* cornerModel{ cornerNode->CreateComponent<StaticModel>() };
        cornerModel->SetModel(GetSubsystem<ResourceMaster>()->GetModel("Corner"));
        cornerModel->SetMaterial(GetSubsystem<ResourceMaster>()->GetMaterial("CornerInactive"));

        corners_.Push(cornerNode);
    }
}
void BlockMap::UpdateCorners()
{
    for (unsigned i{0}; i < corners_.Size(); ++i) {

        Node* c{ corners_.At(i) };
        c->SetPosition(Vector3(-0.5f + 1.0f *  (i % 2),
                               -0.5f + 1.0f * ((i / 2) % 2),
                               -0.5f + 1.0f *  (i / 4))
                       * Vector3(mapSize_) * blockSize_ + GetCenter() - 0.5f * blockSize_);

        Vector3 pos{ c->GetPosition() };
        c->SetScale(blockSize_.Length() * 5.0f * Vector3(Sign(pos.x_) * Sign(pos.y_) * Sign(pos.z_), 1.0f, 1.0f));
        c->LookAt(Vector3::FORWARD * pos.z_, Vector3::UP * pos.y_, TS_LOCAL);
    }
}

void BlockMap::LoadXML(const XMLElement &source)
{
    SetMapSize(source.GetIntVector3("map_size"));
    SetBlockSize(source.GetVector3("block_size"));
    Initialize();

    XMLElement blockSetXML{ source.GetChild("blockset") };

    while(blockSetXML) {

        BlockSet* blockSet{ GetSubsystem<EditMaster>()->LoadBlockSet(blockSetXML.GetAttribute("name")) };
        if (blockSet) {
            XMLElement gridBlockXML{ source.GetChild("gridblock") };
            while (gridBlockXML) {

                if (gridBlockXML.GetInt("set") == blockSetXML.GetInt("id")){

                    SetBlock(gridBlockXML.GetIntVector3("coords"),
                             gridBlockXML.GetQuaternion("rot"),
                             blockSet->GetBlockById(gridBlockXML.GetInt("block")));

                }

                gridBlockXML = gridBlockXML.GetNext("gridblock");
            }
        }

        blockSetXML = blockSetXML.GetNext("blockset");
    }
}

void BlockMap::SaveXML(XMLElement& dest)
{
    dest.SetIntVector3("map_size", GetMapSize());
    dest.SetVector3("block_size", blockSize_);

    HashMap<int, BlockSet*> blockSetsById{};
    int currentBlockSetId{0};

    for (BlockSet* blockSet : GetUsedBlockSets()) {

        XMLElement blockSetElem{ dest.CreateChild("blockset") };
        blockSetsById[currentBlockSetId] = blockSet;
        blockSetElem.SetInt("id", currentBlockSetId++);

        blockSetElem.SetAttribute("name", blockSet->name_);
    }
    for (GridBlock* gridBlock : GetOccupiedGridBlocks()) {

        Block* block{ gridBlock->GetBlock() };
        IntVector3 coords{ gridBlock->GetCoords() };
        XMLElement gridBlockElem{ dest.CreateChild("gridblock") };

        gridBlockElem.SetInt("set", GetBlockSetId(block, blockSetsById));
        gridBlockElem.SetInt("block", block->GetId());
        gridBlockElem.SetIntVector3("coords", coords);
        gridBlockElem.SetQuaternion("rot", gridBlock->GetRotation());
    }
}
int BlockMap::GetBlockSetId(Block* block, HashMap<int, BlockSet*>& blockSetsById)
{
    for (int id : blockSetsById.Keys()) {
        if (blockSetsById[id]->blocks_.Contains(block))
            return id;
    }
    return -1;
}

Vector<Block*> BlockMap::GetUsedBlocks()
{
    Vector<Block*> blocks{};
    for (Sheet& sheet: map_.Values()) {

        for (GridBlock* gridBlock: sheet.Values()) {

            Block* block{ gridBlock->GetBlock() };
            if (block && !blocks.Contains(block))
                blocks.Push(block);
        }
    }
    return blocks;
}
Vector<BlockSet*> BlockMap::GetUsedBlockSets()
{
    Vector<BlockSet*> blockSets{};

    for (Block* block : GetUsedBlocks()) {
        BlockSet* blockSet{ block->GetBlockSet() };

        if (blockSet && !blockSets.Contains(blockSet)) {

            blockSets.Push(blockSet);
        }
    }
    return blockSets;
}
Vector<GridBlock*> BlockMap::GetOccupiedGridBlocks()
{
    Vector<GridBlock*> gridBlocks{};

    for (Sheet& sheet: map_.Values()) {
        for (GridBlock* gridBlock: sheet.Values()) {
            if (gridBlock->GetBlock() != nullptr)

                gridBlocks.Push(gridBlock);
        }
    }
    return gridBlocks;
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

void BlockMap::SetBlock(IntVector3 coords, Quaternion rotation, Block* block)
{

    Sheet sheet{};
    if (map_.TryGetValue(coords.y_, sheet)){

        GridBlock* gridBlock{ nullptr };
        if (sheet.TryGetValue(IntVector2(coords.x_, coords.z_), gridBlock))
            gridBlock->SetBlock(block, rotation);
    }
}

void BlockMap::SetBlockSize(Vector3 size)
{
    blockSize_ = size;

    UpdateCorners();
}
void BlockMap::SetMapSize(IntVector3 size)
{
    mapSize_ = size;

    UpdateCorners();
}
void BlockMap::SetMapSize(int w, int h, int d)
{
    SetMapSize(IntVector3(w, h, d));
}

Vector3 BlockMap::GetCenter()
{
    return node_->GetPosition() + 0.5f * blockSize_ * Vector3(mapSize_.x_, mapSize_.y_, mapSize_.z_);
}


