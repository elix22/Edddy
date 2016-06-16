#ifndef EDDDYCURSOR_H
#define EDDDYCURSOR_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

URHO3D_EVENT(E_CURSORSTEP, CursorStep){}

class EdddyCursor : public LogicComponent
{
    URHO3D_OBJECT(EdddyCursor, LogicComponent);
public:
    EdddyCursor(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void DelayedStart();

    void UpdateSizeAndOffset();
    void SetAxisLock(std::bitset<3> lock);

    void Step(IntVector3 step);
    void SetCoords(IntVector3 coords);
    IntVector3 GetCoords() const { return coords_; }
    void MoveTo(Vector3 position);

    void Rotate(bool clockWise);

    void HandleMouseMove(Vector2 mousePos);
private:
    IntVector3 coords_;
    Node* boxNode_;
    StaticModel* boxModel_;
    Node* blockNode_;
    StaticModel* blockModel_;

    std::bitset<3> axisLock_;
    std::bitset<3> previousAxisLock_;
    AnimatedModel* hitPlanes_;

    void UpdateHitPlanes();
    void UpdateModel(StringHash eventType, VariantMap& eventData);
};


#endif // EDDDYCURSOR_H
