#include "resourcemaster.h"
#include "blockmap.h"
#include "gridblock.h"

#include "edddycam.h"

void GridBlock::RegisterObject(Context *context)
{
    context->RegisterFactory<GridBlock>();
}
GridBlock::GridBlock(Context* context) : BlockInstance(context)
{

}
void GridBlock::OnNodeSet(Node *node)
{ (void)node;

    gridNode_ = node_->CreateChild("GRID");

    centerNode_ = gridNode_->CreateChild("CENTER");
    StaticModel* centerModel = centerNode_->CreateComponent<StaticModel>();
    centerModel->SetModel(RM->GetModel("Center"));
    centerModel->SetMaterial(RM->GetMaterial("VCol"));

    for (int c{0}; c < 8; ++c) {

        Node* cornerNode{ gridNode_->CreateChild("CORNER") };
//        cornerNode->SetPosition(Vector3(
//                                    ((c % 2)     ? -0.5f : 0.5f) * BLOCK_WIDTH,
//                                   (((c / 2) %2) ? -0.5f : 0.5f) * BLOCK_HEIGHT,
//                                    ((c / 4)     ? -0.5f : 0.5f) * BLOCK_DEPTH));
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
        cornerModel->SetModel(RM->GetModel("Corner"));
        cornerModel->SetMaterial(RM->GetMaterial("Glow"));

        cornerNodes_.Push(cornerNode);
    }
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

    centerNode_->SetScale(1.0f - Clamp(Distance(node_->GetPosition(), MC->camera_->GetNode()->GetPosition()) * 0.1f - 0.5f, 0.0f, 1.0f));
}
