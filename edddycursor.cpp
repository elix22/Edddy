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

#include "edddycursor.h"
#include "edddycam.h"
#include "resourcemaster.h"
#include "inputmaster.h"
#include "effectmaster.h"
#include "castmaster.h"
#include "editmaster.h"
#include "blockmap.h"

void EdddyCursor::RegisterObject(Context *context)
{
    context->RegisterFactory<EdddyCursor>();
}

EdddyCursor::EdddyCursor(Context* context) : LogicComponent(context),
    coords_{},
    axisLock_{},
    previousAxisLock_{}
{
    axisLock_.flip();
    axisLock_[1] = false;

    previousAxisLock_ = axisLock_;
    previousAxisLock_.flip();
}

void EdddyCursor::OnNodeSet(Node *node)
{ (void)node;

    blockNode_ = node_->CreateChild("PREVIEW");
    blockModel_ = blockNode_->CreateComponent<StaticModel>();

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
    boxModel_->SetMaterial(GetSubsystem<ResourceMaster>()->GetMaterial("TransparentGlow"));

    UpdateSizeAndOffset();
    SetCoords(IntVector3(MAP_WIDTH / 2, 0, MAP_DEPTH / 2));

}
void EdddyCursor::DelayedStart()
{
    SendEvent(E_CURSORSTEP);
    SubscribeToEvent(E_CURRENTBLOCKCHANGE, URHO3D_HANDLER(EdddyCursor, UpdateModel));
}

void EdddyCursor::UpdateSizeAndOffset()
{
    boxNode_->SetScale(Vector3(BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_DEPTH));
    blockNode_->SetPosition(Vector3::DOWN * BLOCK_HEIGHT * 0.5f);
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
    if (axisLock_.count() == 2){
        switch (axisLock_.to_ulong()) {
        case 5: step = IntVector3(step.x_, step.z_, step.y_);
            break;
        case 3: step = IntVector3(step.x_, step.y_, step.z_);
            break;
        case 6: step = IntVector3(step.z_, step.y_, step.x_);
            break;
        }
    } else {
        switch (axisLock_.to_ulong()) {
        case 1: step = IntVector3(step.x_ + step.y_ + step.z_, 0, 0);
            break;
        case 2: step = IntVector3(0, step.x_ + step.y_ + step.z_, 0);
            break;
        case 4: step = IntVector3(0, 0, step.x_ + step.y_ + step.z_);
            break;
        }
    }


    IntVector3 resultingCoords{ coords_ + step };
    if (resultingCoords.x_ < 0
     || resultingCoords.y_ < 0
     || resultingCoords.z_ < 0
     || resultingCoords.x_ >= MAP_WIDTH
     || resultingCoords.y_ >= MAP_HEIGHT
     || resultingCoords.z_ >= MAP_DEPTH)
    {
        return;
    }

    SetCoords(resultingCoords);
}

void EdddyCursor::SetCoords(IntVector3 coords)
{
    if ( (coords.x_ < 0 || coords.x_ >= MAP_WIDTH)
      || (coords.y_ < 0 || coords.y_ >= MAP_HEIGHT)
      || (coords.z_ < 0 || coords.z_ >= MAP_DEPTH)
      ||  coords == coords_)
        return;

    coords_ = coords;

    GetSubsystem<EffectMaster>()->TranslateTo(node_, Vector3{
                                coords.x_ * BLOCK_WIDTH,
                                coords.y_ * BLOCK_HEIGHT,
                                coords.z_ * BLOCK_DEPTH }, 0.13f);
    SendEvent(E_CURSORSTEP);
}

void EdddyCursor::MoveTo(Vector3 position)
{
    //if (grid)
    IntVector3 coords{ static_cast<int>(round(position.x_ / BLOCK_WIDTH)),
                       static_cast<int>(round(position.y_ / BLOCK_HEIGHT)),
                       static_cast<int>(round(position.z_ / BLOCK_DEPTH)) };

    SetCoords(IntVector3(
            axisLock_[0] ? coords.x_ : coords_.x_,
            axisLock_[1] ? coords.y_ : coords_.y_,
            axisLock_[2] ? coords.z_ : coords_.z_
                  ));
}

void EdddyCursor::Rotate(bool clockWise)
{
    Vector3 axis{ Vector3::UP };
    node_->Rotate(Quaternion(clockWise ? 90.0f :  -90.0f, axis), TS_WORLD);
}

void EdddyCursor::HandleMouseMove()
{
    Ray mouseRay{ GetSubsystem<InputMaster>()->MouseRay() };

    PODVector<PhysicsRaycastResult> hitResults{};

    if (GetSubsystem<CastMaster>()->PhysicsRayCast(hitResults, mouseRay, M_LARGE_VALUE, M_MAX_UNSIGNED)){

        float closest{ M_INFINITY };
        PhysicsRaycastResult closestResult;
        for (PhysicsRaycastResult result: hitResults)
            if (((((axisLock_.count() == 2) != (axisLock_[1] != axisLock_[2])) && Abs(result.normal_.x_) > 0.5f)
              || (((axisLock_.count() == 2) != (axisLock_[0] != axisLock_[2])) && Abs(result.normal_.y_) > 0.5f)
              || (((axisLock_.count() == 2) != (axisLock_[0] != axisLock_[1])) && Abs(result.normal_.z_) > 0.5f))
              && result.distance_ < closest)
            {
                closest = result.distance_;
                closestResult = result;
            }
        MoveTo(closestResult.position_);
    }
}

void EdddyCursor::UpdateModel(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;
    Block* currentBlock{ GetSubsystem<EditMaster>()->GetCurrentBlock() };
    Model* model{ currentBlock->GetModel() };
    if (model)    {
        blockModel_->SetModel(model);
        blockModel_->SetMaterial(boxModel_->GetMaterial());
        if (boxNode_->IsEnabled())
            boxNode_->SetEnabled(false);
    } else {
        blockModel_->SetModel(nullptr);
        if (!boxNode_->IsEnabled())
            boxNode_->SetEnabled(true);
    }
}
