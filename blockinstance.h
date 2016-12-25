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
    void SetBlock(Block* block, Quaternion rotation);
    Block* GetBlock() const { return block_; }
    Quaternion GetRotation() const { return rotation_; }
protected:
    BlockMap* blockMap_;

    Block* block_;
    Node* blockNode_;
    Quaternion rotation_;
    StaticModel* blockModel_;
    Material* blockMaterial_;
};

#endif // BLOCKINSTANCE_H
