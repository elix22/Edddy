#include "resourcemaster.h"
#include "inputmaster.h"
#include "blockmap.h"
#include "gridblock.h"

#include "edddycam.h"
#include "edddycursor.h"

void GridBlock::RegisterObject(Context *context)
{
    context->RegisterFactory<GridBlock>();
}
GridBlock::GridBlock(Context* context) : BlockInstance(context)
{
}
void GridBlock::OnNodeSet(Node *node)
{

    BlockInstance::OnNodeSet(node);

    gridNode_ = node_->CreateChild("GRID");

    centerNode_ = gridNode_->CreateChild("CENTER");
    centerModel_ = centerNode_->CreateComponent<AnimatedModel>();
    centerModel_->SetModel(GetSubsystem<ResourceMaster>()->GetModel("Center"));
    centerModel_->SetMaterial(GetSubsystem<ResourceMaster>()->GetMaterial("VCol"));

    for (int c{0}; c < 8; ++c) {

        Node* cornerNode{ gridNode_->CreateChild("CORNER") };
        cornerNode->SetPosition(Vector3(
                                    0.5f * BLOCK_WIDTH,
                                    0.5f * BLOCK_HEIGHT,
                                    0.5f * BLOCK_DEPTH));
        cornerNode->RotateAround(cornerNode->GetParent()->GetPosition(),
                                 Quaternion(
                                    c/4 * 180,
                                    (c % 4) * 90.0f,
                                    0.0f), TS_PARENT);

        StaticModel* cornerModel{ cornerNode->CreateComponent<StaticModel>() };
        cornerModel->SetModel(GetSubsystem<ResourceMaster>()->GetModel("Corner"));
        cornerModel->SetMaterial(GetSubsystem<ResourceMaster>()->GetMaterial("CornerGlowInactive"));

        cornerNodes_.Push(cornerNode);
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

    node_->SetPosition(Vector3(
                           coords_.x_ * BLOCK_WIDTH,
                           coords_.y_ * BLOCK_HEIGHT,
                           coords_.z_ * BLOCK_DEPTH));
}

void GridBlock::Update(float timeStep)
{ (void)timeStep;

}


void GridBlock::HandleCursorStep(StringHash eventType, VariantMap& evenData)
{

    float distanceToCursor{ Distance(node_->GetPosition() / BLOCK_SIZE,
                                     GetSubsystem<InputMaster>()->GetCursor()->GetNode()->GetPosition() / BLOCK_SIZE) };
    float centerScale{1.0f - Clamp(distanceToCursor * 0.25f - 0.5f, 0.0f, 0.666f)};
    centerNode_->SetScale(centerScale);
    centerModel_->SetMorphWeight(0, distanceToCursor > 1.0f);

    float cornerScale{1.0f - Clamp(distanceToCursor * 0.1f - 0.1f, 0.0f, 0.666f)};
    if (distanceToCursor < 0.1f) cornerScale = 0.0f;
    for (Node* cornerNode : cornerNodes_) {
        cornerNode->SetScale(cornerScale);
    }

}
