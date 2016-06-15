#include "blockinstance.h"
#include "blockmap.h"


BlockInstance::BlockInstance(Context* context) : LogicComponent(context),
    block_{ nullptr }
{

}

void BlockInstance::OnNodeSet(Node *node)
{ (void)node;

    Vector<String> tag{};
    tag.Push(String("Block"));
    node_->SetTags(tag);

    blockNode_ = node_->CreateChild("BLOCK");
    blockNode_->SetPosition(Vector3::DOWN * BLOCK_HEIGHT * 0.5f);

    blockModel_ = blockNode_->CreateComponent<StaticModel>();

}

bool BlockInstance::SetBlock(Block* block, Quaternion rotation)
{
    if (block){
        block_ = block;
        blockModel_->SetModel(block->GetModel());
        blockMaterial_ = block->GetMaterial();
        blockModel_->SetMaterial(blockMaterial_);

        rotation_ = rotation;
        blockNode_->SetRotation(rotation_);

        return true;
    } else return false;
}

