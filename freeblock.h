#ifndef FREEBLOCK_H
#define FREEBLOCK_H

#include <Urho3D/Urho3D.h>
#include "blockinstance.h"

class FreeBlock : public BlockInstance
{
    URHO3D_OBJECT(FreeBlock, BlockInstance);
public:
    FreeBlock(Context* context);
    static void RegisterObject(Context* context);
private:
    Vector3 location_;
};

#endif // FREEBLOCK_H
