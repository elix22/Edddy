#include "edddycursor.h"
#include "resourcemaster.h"
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

    for (int p{0}; p < 3; ++p){
        Node* hitNode{ node_->CreateChild(String()+(p <  2 ? "X" : "Y") +
                                          (p == 1 ? "Y" : "Z") +
                                          "_PLANE") };
        Vector<String> tag{};
        tag.Push(String("Hit"));
        hitNode->SetTags(tag);

        hitNode->SetRotation(Quaternion(p == 1? -90.0f : 0.0f,
                                        0.0f,
                                        p == 2? -90.0f : 0.0f));
        hitNode->SetScale(1000.0f);

        StaticModel* hitPlane{ hitNode->CreateComponent<StaticModel>() };
        hitPlane->SetModel(RM->GetModel("Plane"));
        hitPlane->SetMaterial(RM->GetMaterial("Invisible"));

        hitPlanes_.Push(hitNode);
    }
    UpdateHitPlanes();

    boxNode_ = node_->CreateChild("BOX");
    boxModel_ = boxNode_->CreateComponent<StaticModel>();
    boxModel_->SetModel(RM->GetModel("Cursor"));
    boxModel_->SetMaterial(RM->GetMaterial("TransparentGlow"));

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

        UpdateHitPlanes();
    }
}
void EdddyCursor::UpdateHitPlanes()
{
    for (unsigned p{0}; p < hitPlanes_.Size(); ++p)
            switch (p) {
            default: break;
            case 0:
                hitPlanes_[p]->SetEnabled((axisLock_.count() == 2) ^ (axisLock_[0] ^ axisLock_[2]));
                break;
            case 1:
                hitPlanes_[p]->SetEnabled((axisLock_.count() == 2) ^ (axisLock_[0] ^ axisLock_[1]));
                break;
            case 2:
                hitPlanes_[p]->SetEnabled((axisLock_.count() == 2) ^ (axisLock_[1] ^ axisLock_[2]));
                break;
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
        case 1: step = IntVector3(step.x_ + step.y_, 0, 0);
            break;
        case 2: step = IntVector3(0, step.x_ + step.y_, 0);
            break;
        case 4: step = IntVector3(0, 0, step.x_ + step.y_);
            break;
        }
    }


    IntVector3 resultingCoords{ coords_ + step };
    if (resultingCoords.x_ < 0 ||
        resultingCoords.y_ < 0 ||
        resultingCoords.z_ < 0 ||
        resultingCoords.x_ >= MAP_WIDTH  ||
        resultingCoords.y_ >= MAP_HEIGHT ||
        resultingCoords.z_ >= MAP_DEPTH)
    {
        return;
    }

    SetCoords(resultingCoords);
}

void EdddyCursor::SetCoords(IntVector3 coords)
{
    coords_ = coords;
    node_->SetPosition(Vector3{ coords.x_ * BLOCK_WIDTH,
                                coords.y_ * BLOCK_HEIGHT,
                                coords.z_ * BLOCK_DEPTH});
    SendEvent(E_CURSORSTEP);
}

void EdddyCursor::Rotate(bool clockWise)
{
    Vector3 axis{ Vector3::UP };
    node_->Rotate(Quaternion(clockWise ? 90.0f :  -90.0f, axis), TS_WORLD);
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
