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

#include "block.h"

void Block::RegisterAttributes(Context* context)
{
    context->RegisterAttribute<Block>(AttributeInfo(VAR_INT, "id", (size_t)8, Variant((int)0), (unsigned)0));
    context->RegisterAttribute<Block>(AttributeInfo(VAR_STRING, "model", (size_t)32, Variant(""), (unsigned)0));
    context->RegisterAttribute<Block>(AttributeInfo(VAR_STRING, "material", (size_t)32, Variant(""), (unsigned)0));
}

Block::Block(Context* context) : Object(context)
{
}

void Block::SetId(unsigned id)
{
    id_ = id;
}

void Block::SetModel(Model *model)
{
    model_ = model;
}

void Block::SetMaterial(Material *material)
{
    material_ = material;
}

BlockSet* Block::GetBlockSet()
{
    for (BlockSet* bS : GetSubsystem<EditMaster>()->GetBlockSets()) {

        if (bS->blocks_.Contains(this))
            return bS;
    }
    return nullptr;
}

void Block::SaveXML(XMLElement &dest)
{
    XMLElement blockElem{ dest.CreateChild("block") };

    blockElem.SetUInt("id", id_);
    blockElem.SetString("model", model_->GetName());
    blockElem.SetString("material", material_->GetName());
}





