#ifndef GUIMASTER_H
#define GUIMASTER_H

#include <Urho3D/Urho3D.h>

#include "luckey.h"

class GUIMaster : public Object
{
    URHO3D_OBJECT(GUIMaster, Object);
public:
    GUIMaster(Context* context);
    void HandleDragMove(StringHash eventType, VariantMap& eventData);
    void HandleDragBegin(StringHash eventType, VariantMap& eventData);
    void HandleNewMapButtonPushed(StringHash eventType, VariantMap& eventData);
    void HandleCloseButtonPushed(StringHash eventType, VariantMap& eventData);
    void CleanIntInput(StringHash eventType, VariantMap& eventData);
    void CreateNewMapWindow();
    void CleanFloatInput(StringHash eventType, VariantMap& eventData);

    void OpenNewMapDialog();
    void FixTrailingZeros(String& cleanText);
    
    void HandleScreenMode(StringHash, VariantMap& eventData);
private:
    BorderImage* menuBar_;
    Window* newMapWindow_;
    SharedPtr<XMLFile> defaultStyle_;
    IntVector2 dragBeginPositionMouse_;
    IntVector2 dragBeginPositionElement_;

    SharedPtr<UIElement> uiRoot_;
    LineEdit* lineEdit_;
    HashMap<String, Text*> input_;
    void CreateMenuBar();
};

#endif // GUIMASTER_H
