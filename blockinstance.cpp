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

#include "blockinstance.h"
#include "blockmap.h"


BlockInstance::BlockInstance(Context* context) : Component(context),
    blockMap_{ nullptr },
    block_{ nullptr }
{

}

void BlockInstance::OnNodeSet(Node *node)
{ if (!node) return;

    Vector<String> tag{};
    tag.Push(String("Block"));
    node_->SetTags(tag);

    blockNode_ = node_->CreateChild("BLOCK");

    blockModel_ = blockNode_->CreateComponent<StaticModel>();
}

void BlockInstance::SetBlock(Block* block, Quaternion rotation)
{
    if (block){

        block_ = block;
        blockModel_->SetModel(block->GetModel());
        blockMaterial_ = block->GetMaterial();
        blockModel_->SetMaterial(blockMaterial_);

        blockNode_->SetRotation(rotation);

    } else {

        block_ = nullptr;
        blockModel_->SetModel(nullptr);
    }
}

