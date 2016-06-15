#ifndef BLOCK_H
#define BLOCK_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

//For holding resource pointers
class Block : public Object
{
    friend class EditMaster;

    URHO3D_OBJECT(Block, Object);
public:
    Block(Context* context);

    Model* GetModel() const { return model_; }
    Material* GetMaterial() const { return material_; }
private:
    String name_;
    Model* model_;
    Material* material_;
    void MatchMaterialw() {}
};

#endif // BLOCK_H
