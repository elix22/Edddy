#include "freeblock.h"

void FreeBlock::RegisterObject(Context *context)
{
    context->RegisterFactory<FreeBlock>();
}
FreeBlock::FreeBlock(Context* context) : BlockInstance(context)
{

}





