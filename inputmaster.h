/* Edddy
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Commercial licenses are available through frode@lindeijer.nl
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef INPUTMASTER_H
#define INPUTMASTER_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

enum InputAction { ACTION_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_FORWARD, ACTION_BACK,
                   ACTION_X_AXIS, ACTION_Y_AXIS, ACTION_Z_AXIS, ACTION_H_AXIS, ACTION_V_AXIS, ACTION_D_AXIS,
                   ACTION_NEXT_BLOCK, ACTION_PREVIOUS_BLOCK, ACTION_NEXT_PALETTE, ACTION_PREVIOUS_PALETTE,
                   ACTION_ROTATE_CW, ACTION_ROTATE_CCW, ACTION_PICKBLOCK,
                   ACTION_UNDO, ACTION_REDO, ACTION_CONFIRM, ACTION_CANCEL, ACTION_DELETE, ACTION_SAVE, ACTION_QUIT, ALL_ACTIONS};

typedef Vector<InputAction> InputActions;

class EdddyCursor;

#define ACTION_INTERVAL 0.16f

class InputMaster : public Object
{
    URHO3D_OBJECT(InputMaster, Object);
public:
    InputMaster(Context* context);

    void SetCursor(EdddyCursor* cursor) { cursor_ = cursor; }
    EdddyCursor* GetCursor() const { return cursor_; }
    bool CheckActionable(InputAction action, const InputActions& inputActions, bool reset = false);
    Ray GetMouseRay();

private:
    Ray mouseRay_;
    EdddyCursor* cursor_;
    int wheelStep_;

    HashMap<int, InputAction> keyBindings_;
    HashMap<int, InputAction> mouseButtonBindings_;
    HashMap<int, InputAction> joystickButtonBindings_;

    HashSet<int> pressedKeys_;
    HashSet<int> pressedMouseButtons_;
    HashSet<int> pressedJoystickButtons_;
    bool shiftDown_;
    bool ctrlDown_;
    bool altDown_;

    Vector2 leftStickPosition_;
    Vector2 rightStickPosition_;

    HashMap<int, float> actionTime_;

    void HandleUpdate(StringHash eventType, VariantMap &eventData);
    void HandleKeyDown(StringHash eventType, VariantMap &eventData);
    void HandleKeyUp(StringHash eventType, VariantMap &eventData);
    void HandleJoyButtonDown(StringHash eventType, VariantMap &eventData);
    void HandleJoyButtonUp(StringHash eventType, VariantMap &eventData);
    void HandleJoystickAxisMove(StringHash eventType, VariantMap& eventData);
    void HandleMouseMove(StringHash eventType, VariantMap& eventData);
    void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData);
    void HandleMouseButtonUp(StringHash eventType, VariantMap& eventData);

    void HandleCursorStep(StringHash eventType, VariantMap& eventData);

    void HandleActions(const InputActions &actions, float timeStep);
    IntVector3 GetStepFromActions(const InputActions& actions);
    IntVector3 CorrectForCameraYaw(IntVector3 intVec3);
    void UpdateModifierKeys();
    void KeyCameraMovement();
};

#endif // INPUTMASTER_H
