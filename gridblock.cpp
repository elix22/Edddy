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
#include "inputmaster.h"
#include "blockmap.h"
#include "gridblock.h"

#include "edddycam.h"
#include "edddycursor.h"

StaticModelGroup* GridBlock::activeCenterGroup_{};
StaticModelGroup* GridBlock::inactiveCenterGroup_{};
StaticModelGroup* GridBlock::sideGroup_{};

void GridBlock::RegisterObject(Context *context)
{
    context->RegisterFactory<GridBlock>();
}
GridBlock::GridBlock(Context* context) : BlockInstance(context)
{
}

void GridBlock::OnNodeSet(Node *node)
{ if (!node) return;

    if (!activeCenterGroup_) {
        activeCenterGroup_ = MC->GetScene()->CreateComponent<StaticModelGroup>();
        activeCenterGroup_->SetModel(GetSubsystem<ResourceMaster>()->GetModel("Center"));
        activeCenterGroup_->SetMaterial(GetSubsystem<ResourceMaster>()->GetMaterial("CenterActive"));
    }
    if (!inactiveCenterGroup_) {
        inactiveCenterGroup_ = MC->GetScene()->CreateComponent<StaticModelGroup>();
        inactiveCenterGroup_->SetModel(GetSubsystem<ResourceMaster>()->GetModel("Center"));
        inactiveCenterGroup_->SetMaterial(GetSubsystem<ResourceMaster>()->GetMaterial("CenterInactive"));
    }
    if (!sideGroup_) {
        sideGroup_ = MC->GetScene()->CreateComponent<StaticModelGroup>();
        sideGroup_->SetModel(GetSubsystem<ResourceMaster>()->GetModel("Plane"));
        sideGroup_->SetMaterial(GetSubsystem<ResourceMaster>()->GetMaterial("TransparentGlow"));
    }

    blockMap_ = node_->GetParent()->GetComponent<BlockMap>();

    BlockInstance::OnNodeSet(node);

    sidesNode_ = node_->CreateChild("SIDES");
    centerNode_ = sidesNode_->CreateChild("CENTER");

    CreateSideNodes();
}
void GridBlock::CreateSideNodes()
{
    for (int s{0}; s < 6; ++s) {

        Node* sideNode{ sidesNode_->CreateChild("SIDE") };

        Vector3 pos{};
        Quaternion rot{};
        Vector3 scale{ Vector3::ONE };
        switch(s) {
        case 0:
            pos = Vector3::UP * blockMap_->GetBlockHeight() * 0.5f;
            scale = Vector3(blockMap_->GetBlockWidth(), 1.0f, blockMap_->GetBlockDepth());
            break;
        case 1:
            pos = Vector3::DOWN * blockMap_->GetBlockHeight() * 0.5f;
            scale = Vector3(blockMap_->GetBlockWidth(), 1.0f, blockMap_->GetBlockDepth());
            break;
        case 2:
            pos = Vector3::LEFT * blockMap_->GetBlockWidth() * 0.5f;
            rot = Quaternion(90.0f, Vector3::FORWARD);
            scale = Vector3(blockMap_->GetBlockHeight(), 1.0f, blockMap_->GetBlockDepth());
            break;
        case 3:
            pos = Vector3::RIGHT * blockMap_->GetBlockWidth() * 0.5f;
            rot = Quaternion(90.0f, Vector3::BACK);
            scale = Vector3(blockMap_->GetBlockHeight(), 1.0f, blockMap_->GetBlockDepth());
            break;
        case 4:
            pos = Vector3::FORWARD * blockMap_->GetBlockDepth() * 0.5f;
            rot = Quaternion(90.0f, Vector3::RIGHT);
            scale = Vector3(blockMap_->GetBlockWidth(), 1.0f, blockMap_->GetBlockHeight());
            break;
        case 5:
            pos = Vector3::BACK * blockMap_->GetBlockDepth() * 0.5f;
            rot = Quaternion(90.0f, Vector3::LEFT);
            scale = Vector3(blockMap_->GetBlockWidth(), 1.0f, blockMap_->GetBlockHeight());
            break;
        default: break;
        }

        sideNode->SetPosition(pos);
        sideNode->SetRotation(rot);
        sideNode->SetScale(scale);

        sideNodes_.Push(sideNode);
    }
}

void GridBlock::DelayedStart()
{

    SubscribeToEvent(GetSubsystem<InputMaster>()->GetCursor(), E_CURSORSTEP, URHO3D_HANDLER(GridBlock, HandleCursorStep));
}

void GridBlock::Init(IntVector3 coords)
{
    SetCoords(coords);
}

void GridBlock::SetCoords(IntVector3 coords)
{
    if (coords_ == coords)
        return;

    coords_ = coords;

    UpdatePosition();
}
void GridBlock::UpdatePosition()
{
    node_->SetPosition(Vector3(
                           coords_.x_ * blockMap_->GetBlockWidth(),
                           coords_.y_ * blockMap_->GetBlockHeight(),
                           coords_.z_ * blockMap_->GetBlockDepth()));
}

void GridBlock::Update(float timeStep)
{ (void)timeStep;

}


//void GridBlock::UpdateSelectionBorders(StringHash eventType, VariantMap& eventData)
//{ (void)eventType; (void)eventData;


//}

void GridBlock::HandleCursorStep(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    EdddyCursor* cursor{ GetSubsystem<InputMaster>()->GetCursor() };
    Vector3 cursorCoords{ static_cast<float>(cursor->GetCoords().x_),
                          static_cast<float>(cursor->GetCoords().y_),
                          static_cast<float>(cursor->GetCoords().z_) };

    float distanceToCursor{ Distance(node_->GetPosition() / blockMap_->GetBlockSize(), cursorCoords) };

    float centerScale{1.0f - Clamp(distanceToCursor * 0.25f - 0.5f, 0.0f, 0.666f)};

    centerNode_->SetScale(centerScale);

    if (WithinLock() && !cursor->IsHidden()){

        if (coords_ == cursor->GetCoords()) {
            activeCenterGroup_->AddInstanceNode(centerNode_);
            inactiveCenterGroup_->RemoveInstanceNode(centerNode_);

        } else {
            inactiveCenterGroup_->AddInstanceNode(centerNode_);
            activeCenterGroup_->RemoveInstanceNode(centerNode_);
        }

    } else {

        activeCenterGroup_->RemoveInstanceNode(centerNode_);
        inactiveCenterGroup_->RemoveInstanceNode(centerNode_);
    }
}

bool GridBlock::WithinLock()
{
    EdddyCursor* cursor{ GetSubsystem<InputMaster>()->GetCursor() };
    std::bitset<3> axisLock{ cursor->GetAxisLock() };
    IntVector3 cursorCoords{ cursor->GetCoords() };

    if (axisLock.count() == 2
     && ((axisLock[1] && axisLock[2] && coords_.x_ == cursorCoords.x_)
     ||  (axisLock[0] && axisLock[2] && coords_.y_ == cursorCoords.y_)
     ||  (axisLock[0] && axisLock[1] && coords_.z_ == cursorCoords.z_)) )
            return true;

    else if
        ((axisLock[0] && coords_.y_ == cursorCoords.y_ && coords_.z_ == cursorCoords.z_)
     ||  (axisLock[1] && coords_.x_ == cursorCoords.x_ && coords_.z_ == cursorCoords.z_)
     ||  (axisLock[2] && coords_.x_ == cursorCoords.x_ && coords_.y_ == cursorCoords.y_))
            return true;

    else return false;
}
