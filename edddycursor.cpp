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

#include "edddycam.h"
#include "resourcemaster.h"
#include "inputmaster.h"
#include "effectmaster.h"
#include "castmaster.h"
#include "editmaster.h"
#include "blockmap.h"

#include "edddycursor.h"

void EdddyCursor::RegisterObject(Context *context)
{
    context->RegisterFactory<EdddyCursor>();
}

EdddyCursor::EdddyCursor(Context* context) : LogicComponent(context),
    coords_{},
    targetRotation_{},
    blockModelGroups_{},
    previewNodes_{},
    axisLock_{},
    previousAxisLock_{}
{
    axisLock_.flip();
    axisLock_[1] = false;

    previousAxisLock_ = axisLock_;
    previousAxisLock_.flip();
}

void EdddyCursor::OnNodeSet(Node *node)
{ if (!node) return;

    blockNode_ = node_->CreateChild("PREVIEW");

    RigidBody* rigidBody_{ node_->CreateComponent<RigidBody>() };
    rigidBody_->SetKinematic(true);

    for (int c{0}; c < 3; ++c){

        CollisionShape* hitPlane{ node_->CreateComponent<CollisionShape>() };
        hitPlane->SetBox(Vector3(c == 0 ? 0.0f : 1000.0f,
                                 c == 1 ? 0.0f : 1000.0f,
                                 c == 2 ? 0.0f : 1000.0f));
    }

    boxNode_ = node_->CreateChild("BOX");
    boxModel_ = boxNode_->CreateComponent<StaticModel>();
    boxModel_->SetModel(GetSubsystem<ResourceMaster>()->GetModel("Cursor"));
    boxModel_->SetMaterial(MAT_GLOWWIRE);

    for (bool wire : {true, false}) {

        StaticModelGroup* blockModelGroup{ blockNode_->CreateComponent<StaticModelGroup>() };

        if (wire) {
            blockModelGroups_.first_ = blockModelGroup;
        } else {
            blockModelGroups_.second_ = blockModelGroup;
        }
    }
    AddInstanceNode(blockNode_);

    SubscribeToEvent(E_CURRENTBLOCKCHANGE, URHO3D_HANDLER(EdddyCursor, UpdateModel));
    SubscribeToEvent(E_CURRENTMAPCHANGE, URHO3D_HANDLER(EdddyCursor, HandleMapChange));
}
void EdddyCursor::DelayedStart()
{
//    MoveTo(GetSubsystem<EditMaster>()->GetCurrentBlockMap()->GetCenter());
//    SendEvent(E_CURSORSTEP);
}
void EdddyCursor::HandleMapChange(StringHash, VariantMap& eventData)
{

    BlockMap* blockMap{ static_cast<BlockMap*>(eventData[CurrentMapChange::P_MAP].GetPtr()) };

    if (blockMap) {

        UpdateSize();
        MoveTo(blockMap->GetCenter());
        node_->SetEnabled(true);

    } else {

        node_->SetEnabled(false);
    }
}
void EdddyCursor::UpdateModel(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    Block* currentBlock{ static_cast<Block*>(eventData[CurrentBlockChange::P_BLOCK].GetPtr()) };

    if (currentBlock) {

        Model* model{ currentBlock->GetModel() };
        if (model) {

            blockModelGroups_.first_->SetModel(model);
            blockModelGroups_.first_->SetMaterial(MAT_GLOWWIRE);
            blockModelGroups_.second_->SetModel(model);
            blockModelGroups_.second_->SetMaterial(MAT_TRANSPARENTGLOW);
            if (boxNode_->IsEnabled())
                boxNode_->SetEnabled(false);
        }

    } else {

        blockModelGroups_.first_->SetModel(nullptr);
        blockModelGroups_.second_->SetModel(nullptr);

        if (!boxNode_->IsEnabled())
            boxNode_->SetEnabled(true);
    }
}

void EdddyCursor::Hide()
{
    hidden_ = true;
    boxNode_->SetEnabled(false);
    blockNode_->SetEnabled(false);

    SendEvent(E_CURSORSTEP);
}
void EdddyCursor::Show()
{
    hidden_ = false;
    blockNode_->SetEnabled(true);
    if (blockModelGroups_.first_->GetModel()) {
        boxNode_->SetEnabled(false);
    } else {
        boxNode_->SetEnabled(true);
    }
}
void EdddyCursor::ToggleVisibility()
{
    hidden_ ? Show()
            : Hide();
}

void EdddyCursor::AddInstanceNode(Node* node, Quaternion rotation)
{ if (!node) return;

    if (node == blockNode_) {

        blockModelGroups_.first_->AddInstanceNode(node);
        blockModelGroups_.second_->AddInstanceNode(node);

    } else {

        Node* previewNode{ node->GetChild("PREVIEW") };

        if (!previewNode)
            previewNode = node->CreateChild("PREVIEW");

        previewNode->SetWorldRotation(rotation);
        blockModelGroups_.first_->AddInstanceNode(previewNode);
        blockModelGroups_.second_->AddInstanceNode(previewNode);

        previewNodes_.Push(previewNode);
    }
}
void EdddyCursor::RemoveInstanceNode(Node* node)
{ if (!node) return;

    if (node == blockNode_) {

        blockModelGroups_.first_->RemoveInstanceNode(node);
        blockModelGroups_.second_->RemoveInstanceNode(node);

    } else {

        Node* previewNode{ node->GetChild("PREVIEW") };

        if (!previewNode)
            return;

        blockModelGroups_.first_->RemoveInstanceNode(previewNode);
        blockModelGroups_.second_->RemoveInstanceNode(previewNode);

        if (previewNodes_.Contains(previewNode)) {
            previewNodes_.Remove(previewNode);
            previewNode->Remove();
        }
    }
}
void EdddyCursor::RemoveAllInstanceNodes()
{
    blockModelGroups_.first_->RemoveAllInstanceNodes();
    blockModelGroups_.second_->RemoveAllInstanceNodes();
    
    AddInstanceNode(blockNode_);

    for (Node* node: previewNodes_)
        node->Remove();

    previewNodes_.Clear();
}

void EdddyCursor::UpdateSize()
{
    BlockMap* currentMap{ GetSubsystem<EditMaster>()->GetCurrentBlockMap() };
    Vector3 blockSize{ Vector3::ONE };

    if (currentMap)
        blockSize = currentMap->GetBlockSize();

    boxNode_->SetScale(Vector3(blockSize.x_, blockSize.y_, blockSize.z_));
}

void EdddyCursor::SetAxisLock(std::bitset<3> lock)
{
    if (lock.all() || lock.none())
        return;

    if (lock != axisLock_){
        previousAxisLock_ = axisLock_;
        axisLock_ = lock;
        SendEvent(E_CURSORSTEP);
    }
}

void EdddyCursor::Step(IntVector3 step)
{
    BlockMap* currentMap{ GetSubsystem<EditMaster>()->GetCurrentBlockMap() };
    IntVector3 mapSize{ IntVector3::ONE };

    if (currentMap)
        mapSize = currentMap->GetMapSize();

    switch (axisLock_.to_ulong()) {
    case 5: step = IntVector3(step.x_, step.z_, step.y_);
        break;
    case 3: step = IntVector3(step.x_, step.y_, step.z_);
        break;
    case 6: step = IntVector3(step.z_, step.y_, step.x_);
        break;

    case 1: step = IntVector3(step.x_ + step.y_ + step.z_, 0, 0);
        break;
    case 2: step = IntVector3(0, step.x_ + step.y_ + step.z_, 0);
        break;
    case 4: step = IntVector3(0, 0, step.x_ + step.y_ + step.z_);
        break;
    default:
        break;
    }


    IntVector3 resultingCoords{ coords_ + step };
    if (resultingCoords.x_ < 0
     || resultingCoords.y_ < 0
     || resultingCoords.z_ < 0
     || resultingCoords.x_ >= mapSize.x_
     || resultingCoords.y_ >= mapSize.y_
     || resultingCoords.z_ >= mapSize.z_)
    {
        return;
    }

    SetCoords(resultingCoords);
}

void EdddyCursor::SetCoords(IntVector3 coords)
{
    BlockMap* currentMap{ GetSubsystem<EditMaster>()->GetCurrentBlockMap() };
    Vector3 blockSize{ Vector3::ONE };

    if (currentMap)
        blockSize = currentMap->GetBlockSize();


    if (currentMap && !currentMap->Contains(coords)) {

        if (!hidden_) {
            Hide();
        }
        return;

    } else if (coords == coords_ && !hidden_) {

        return;
    }

    if (hidden_)
        Show();

    coords_ = coords;

//    GetSubsystem<EffectMaster>()->TranslateTo(node_, Vector3(coords) * blockSize, 0.13f);
    node_->SetPosition(Vector3(coords) * blockSize);
    SendEvent(E_CURSORSTEP);
}

void EdddyCursor::MoveTo(Vector3 position)
{
    BlockMap* currentMap{ GetSubsystem<EditMaster>()->GetCurrentBlockMap() };
    Vector3 blockSize{ currentMap ? currentMap->GetBlockSize() : Vector3::ONE };

    IntVector3 coords{ VectorRoundToInt(position / blockSize) };

    SetCoords(IntVector3(
            axisLock_[0] ? coords.x_ : coords_.x_,
            axisLock_[1] ? coords.y_ : coords_.y_,
            axisLock_[2] ? coords.z_ : coords_.z_));
}

void EdddyCursor::Rotate(bool clockWise)
{
    Vector3 axis{ Vector3::UP };
    targetRotation_ = Quaternion(clockWise ? 90.0f :  -90.0f, axis) * targetRotation_;
    GetSubsystem<EffectMaster>()->RotateTo(node_, targetRotation_, 0.1f);

    SendEvent(E_CURSORSTEP);
}
void EdddyCursor::SetRotation(Quaternion rot)
{
//    node_->RemoveAttributeAnimation("Position");
    node_->SetRotation(rot);

    if (targetRotation_ != rot) {

        targetRotation_ = rot;
        SendEvent(E_CURSORSTEP);
    }
}

void EdddyCursor::HandleMouseMove()
{
    Ray mouseRay{ GetSubsystem<InputMaster>()->GetMouseRay() };

    PODVector<PhysicsRaycastResult> hitResults{};

    if (GetSubsystem<CastMaster>()->PhysicsRayCast(hitResults, mouseRay, M_LARGE_VALUE, M_MAX_UNSIGNED)){

        float closest{ M_INFINITY };
        PhysicsRaycastResult closestResult{};

        for (PhysicsRaycastResult result: hitResults)
            if (((((axisLock_.count() == 2) != (axisLock_[1] != axisLock_[2])) && Abs(result.normal_.x_) > 0.5f)
              || (((axisLock_.count() == 2) != (axisLock_[0] != axisLock_[2])) && Abs(result.normal_.y_) > 0.5f)
              || (((axisLock_.count() == 2) != (axisLock_[0] != axisLock_[1])) && Abs(result.normal_.z_) > 0.5f))
              && result.distance_ < closest)
            {
                closest = result.distance_;
                closestResult = result;
            }
        if (closestResult.body_)
            MoveTo(closestResult.position_);
    }
}
