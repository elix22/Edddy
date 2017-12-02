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

#include "resourcemaster.h"
#include "editmaster.h"
#include "blockset.h"

#include "block.h"

void Block::RegisterAttributes(Context* context)
{
    URHO3D_ATTRIBUTE("id", unsigned, id_, 0, AM_FILE);
    URHO3D_ATTRIBUTE("model", String, model_, "", AM_FILE);
    URHO3D_ATTRIBUTE("material", String, material_, "", AM_FILE);
}

Block::Block(Context* context) : Serializable(context)
{
}

void Block::SetId(unsigned id)
{
    id_ = id;
}

void Block::SetModel(Model *model)
{
    model_ = model->GetName();
}

void Block::SetMaterial(Material *material)
{
    material_ = material->GetName();
}

Model* Block::GetModel() const
{
    return CACHE->GetResource<Model>(model_);
}

Material* Block::GetMaterial() const
{
    return CACHE->GetResource<Material>(material_);
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
    blockElem.SetString("model", model_);
    blockElem.SetString("material", material_);
}





