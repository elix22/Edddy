#ifndef GRIDBLOCK_H
#define GRIDBLOCK_H

#include <Urho3D/Urho3D.h>

#include "intvector3.h"
#include "blockinstance.h"

class GridBlock : public BlockInstance
{
    friend class BlockMap;

    URHO3D_OBJECT(GridBlock, BlockInstance);
public:
    GridBlock(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);

    virtual void Update(float timeStep);
    void HandleCursorStep(StringHash eventType, VariantMap& evenData);
    virtual void DelayedStart();
private:
    IntVector3 coords_;

    Node* gridNode_;
    Node* centerNode_;
    Vector<Node*> cornerNodes_;
    AnimatedModel* centerModel_;
    //Called by BlockMap
    void Init(IntVector3 coords);
    void SetCoords(IntVector3 coords);
};

#endif // GRIDBLOCK_H
