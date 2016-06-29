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
