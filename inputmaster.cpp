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

#include "mastercontrol.h"
#include "editmaster.h"
#include "history.h"
#include "castmaster.h"
#include "guimaster.h"
#include "edddycam.h"
#include "edddycursor.h"

#include "tool.h"

#include "inputmaster.h"

using namespace LucKey;

InputMaster::InputMaster(Context* context) : Object(context),
//    mousePos_{ Vector2::ONE * 0.5f },
    wheelStep_{},
    pressedKeys_{},
    pressedMouseButtons_{},
    pressedJoystickButtons_{},
    shiftDown_{},
    ctrlDown_{},
    altDown_{}
{
    for (int a{0}; a < ALL_ACTIONS; ++a){
        actionTime_[a] = 0.0f;
    }

    keyBindings_[KEY_UP]           = joystickButtonBindings_[SB_DPAD_UP]                                    = ACTION_UP;
    keyBindings_[KEY_DOWN]         = joystickButtonBindings_[SB_DPAD_DOWN]                                  = ACTION_DOWN;
    keyBindings_[KEY_LEFT]         = joystickButtonBindings_[SB_DPAD_LEFT]                                  = ACTION_LEFT;
    keyBindings_[KEY_RIGHT]        = joystickButtonBindings_[SB_DPAD_RIGHT]                                 = ACTION_RIGHT;
    keyBindings_[KEY_RIGHTBRACKET] = joystickButtonBindings_[SB_R1]                                         = ACTION_FORWARD;
    keyBindings_[KEY_LEFTBRACKET]  = joystickButtonBindings_[SB_L1]                                         = ACTION_BACK;
    keyBindings_[KEY_X]                                                                                     = ACTION_X_AXIS;
    keyBindings_[KEY_Y]                                                                                     = ACTION_Y_AXIS;
    keyBindings_[KEY_Z]                                                                                     = ACTION_Z_AXIS;
    keyBindings_[KEY_PERIOD]                                                                                = ACTION_NEXT_BLOCK;
    keyBindings_[KEY_COMMA]                                                                                 = ACTION_PREVIOUS_BLOCK;
    keyBindings_[KEY_0]            = joystickButtonBindings_[SB_R2]                                         = ACTION_ROTATE_CW;
    keyBindings_[KEY_9]            = joystickButtonBindings_[SB_L2]                                         = ACTION_ROTATE_CCW;
    keyBindings_[KEY_SLASH]                                            = mouseButtonBindings_[MOUSEB_RIGHT] = ACTION_PICKBLOCK;
    keyBindings_[KEY_RETURN]       = joystickButtonBindings_[SB_CROSS] = mouseButtonBindings_[MOUSEB_LEFT]  = ACTION_CONFIRM;
    keyBindings_[KEY_ESCAPE]       = joystickButtonBindings_[SB_CIRCLE]                                     = ACTION_CANCEL;
    keyBindings_[KEY_DELETE]                                                                                = ACTION_DELETE;
    keyBindings_[KEY_B]                                                                                     = ACTION_BRUSH;
    keyBindings_[KEY_F]                                                                                     = ACTION_FILL;

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER(InputMaster, HandleJoyButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, URHO3D_HANDLER(InputMaster, HandleJoyButtonUp));
    SubscribeToEvent(E_JOYSTICKAXISMOVE, URHO3D_HANDLER(InputMaster, HandleJoystickAxisMove));
    SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(InputMaster, HandleMouseMove));
    SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(InputMaster, HandleMouseButtonDown));
    SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(InputMaster, HandleMouseButtonUp));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(InputMaster, HandleUpdate));

    SubscribeToEvent(E_CURSORSTEP, URHO3D_HANDLER(InputMaster, HandleCursorStep));

    INPUT->SetMouseVisible(true);
}
void InputMaster::HandleCursorStep(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    actionTime_[ACTION_CONFIRM] = ACTION_INTERVAL;

    GetSubsystem<EditMaster>()->GetTool()->UpdatePreview(shiftDown_, ctrlDown_, altDown_);
}

void InputMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    float timeStep{ eventData[Update::P_TIMESTEP].GetFloat() };
    InputActions activeActions{};

    UpdateModifierKeys();

    //Handle wheel input
    int wheel{ INPUT->GetMouseMoveWheel() };
    wheelStep_ += wheel;

    if ( wheelStep_ != 0) {
        wheelStep_ = 0;

        if (ctrlDown_) {

            if (wheel > 0)
                activeActions.Push(ACTION_ROTATE_CW);
            if (wheel < 0)
                activeActions.Push(ACTION_ROTATE_CCW);

        } else if (altDown_) {

            if (wheel > 0)
                activeActions.Push(ACTION_NEXT_BLOCK);
            if (wheel < 0)
                activeActions.Push(ACTION_PREVIOUS_BLOCK);

        } else {

            if (wheel > 0)
                activeActions.Push(ACTION_FORWARD);
            if (wheel < 0)
                activeActions.Push(ACTION_BACK);
        }
    }

    //Convert key presses to actions
    if (!(ctrlDown_ || altDown_)) {
        for (int key : pressedKeys_) {
            if (keyBindings_.Contains(key)) {
                InputAction action{keyBindings_[key]};
                if (!activeActions.Contains(action))
                    activeActions.Push(action);
            }
        }
    }
    //Convert mouse button presses to actions
    if (!GetSubsystem<UI>()->GetElementAt(INPUT->GetMousePosition())) {
        for (int button : pressedMouseButtons_) {
            if (pressedMouseButtons_.Size() == 1 && mouseButtonBindings_.Contains(button)) {

                InputAction action{ mouseButtonBindings_[button] };
                if (!activeActions.Contains(action))
                    activeActions.Push(action);
            }
        }
    }
    //Convert jostick button presses to actions
    for (int button : pressedJoystickButtons_) {
        if (joystickButtonBindings_.Contains(button)) {
            InputAction action{ joystickButtonBindings_[button] };
            if (!activeActions.Contains(action))
                activeActions.Push(action);
        }
    }

    HandleActions(activeActions, timeStep);

    KeyCameraMovement();
}

void InputMaster::HandleActions(const InputActions& actions, float timeStep)
{
    UI* ui{ GetSubsystem<UI>() };
    if (ui->GetFocusElement())
        return;

    bool hovering{ false };
    if (ui->GetElementAt(INPUT->GetMousePosition())) {

        hovering = true;
    }

    IntVector3 step{ GetStepFromActions(actions) };
    if (step != IntVector3::ZERO) {
        actionTime_[ACTION_CONFIRM] = 0.0f;
        cursor_->Step(step);
    }

    Vector<int> unusedActions{};
    for (int a{0}; a < ALL_ACTIONS; ++a){
        unusedActions.Push(a);
    }

    //Handle actions and reset action timers
    EditMaster* editMaster{ GetSubsystem<EditMaster>() };
    for (InputAction action : actions){

        if (unusedActions.Contains(action))
            unusedActions.Remove(action);

        if (actionTime_[action] == 0.0f
         || actionTime_[action] >= ACTION_INTERVAL) {

            actionTime_[action] = 10e-5f;

            switch (action) {
            case ACTION_UP:       case ACTION_DOWN:
            case ACTION_LEFT:     case ACTION_RIGHT:
            case ACTION_FORWARD:  case ACTION_BACK:
                break;
            case ACTION_X_AXIS: case ACTION_Y_AXIS: case ACTION_Z_AXIS: {

                std::bitset<3> lock{};
                lock[0] = action == ACTION_X_AXIS;
                lock[1] = action == ACTION_Y_AXIS;
                lock[2] = action == ACTION_Z_AXIS;
                if (shiftDown_)
                    lock.flip();
                cursor_->SetAxisLock(lock);

            } break;
            case ACTION_NEXT_BLOCK: {

                editMaster->NextBlock();
            } break;
            case ACTION_PREVIOUS_BLOCK: {

                editMaster->PreviousBlock();
            } break;
            case ACTION_ROTATE_CW: {

                cursor_->Rotate(true);
            } break;
            case ACTION_ROTATE_CCW: {

                cursor_->Rotate(false);
            } break;
            case ACTION_PICKBLOCK: {

                editMaster->PickBlock();
            } break;
            case ACTION_CONFIRM: {

                editMaster->ApplyTool(shiftDown_, ctrlDown_, altDown_);
            } break;
            case ACTION_DELETE: {

                editMaster->ClearBlock();
            } break;
            case ACTION_BRUSH: {
                editMaster->SetTool(Tool::GetTool(StringHash("Brush")));
            } break;
            case ACTION_FILL: {
                editMaster->SetTool(Tool::GetTool(StringHash("Fill")));
            } break;
            case ACTION_CANCEL: {
                editMaster->SetCurrentBlock(nullptr);
            } break;
            default: break;
            }
        }
        actionTime_[action] += timeStep;
    }

    for (int a : unusedActions) {

            actionTime_[a] = 0.0f;
    }
}

void InputMaster::KeyCameraMovement()
{
    EdddyCam* camera{ MC->GetCamera() };
//    camera->Move(IntVector3(INPUT->GetKeyDown(KEY_D) - INPUT->GetKeyDown(KEY_A),
//                                     INPUT->GetKeyDown(KEY_E) - INPUT->GetKeyDown(KEY_Q),
//                                     INPUT->GetKeyDown(KEY_S) - INPUT->GetKeyDown(KEY_W)), MT_PAN);

    camera->Move((0.02f + 0.05f * shiftDown_) * Vector3::FORWARD * (pressedKeys_.Contains(KEY_KP_PLUS) - pressedKeys_.Contains(KEY_KP_MINUS)), MT_PAN);
    camera->Move((0.01f + 0.02f * shiftDown_) * Vector3(pressedKeys_.Contains(KEY_KP_4) - pressedKeys_.Contains(KEY_KP_6),
                                                        pressedKeys_.Contains(KEY_KP_8) - pressedKeys_.Contains(KEY_KP_2)), MT_ROTATE);
}

IntVector3 InputMaster::GetStepFromActions(const InputActions& actions)
{
    IntVector3 move{IntVector3::RIGHT   * (CheckActionable(ACTION_RIGHT, actions) - CheckActionable(ACTION_LEFT, actions))
                  + IntVector3::UP      * (CheckActionable(ACTION_UP, actions) - CheckActionable(ACTION_DOWN, actions))
                  + IntVector3::FORWARD * (CheckActionable(ACTION_FORWARD, actions) - CheckActionable(ACTION_BACK, actions))};

    return CorrectForCameraYaw(move);
}

bool InputMaster::CheckActionable(InputAction action, const InputActions& inputActions, bool reset)
{
    bool actionable{ inputActions.Contains(action) && (actionTime_[action] > ACTION_INTERVAL || actionTime_[action] == 0.0f) };
    if (actionable && reset)
        actionTime_[action] = 0.0f;

    return actionable;
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{ (void)eventType;
    

    int key{ eventData[KeyDown::P_KEY].GetInt() };
//    Log::Write(1, "Key pressed: " + String(key));

    bool uiFocus{ GUI->GetFocusElement() };

    if (!uiFocus)
        pressedKeys_.Insert(key);

    if (!shiftDown_ && !ctrlDown_ && !altDown_) {

        switch (key) {

        case KEY_KP_5: {
            if (!uiFocus)
                MC->GetCamera()->ToggleOrthogaphic();
        } break;

        case KEY_F12: {
            Image screenshot(context_);
            Graphics* graphics{ GetSubsystem<Graphics>() };
            graphics->TakeScreenShot(screenshot);
            //Here we save in the Data folder with date and time appended
            String fileName{ GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
                        Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png" };
            //Log::Write(1, fileName);
            screenshot.SavePNG(fileName);
        } break;

        default: break;
        }

    } else if (ctrlDown_) {

        switch (key) {

        case KEY_N: {
                GetSubsystem<GUIMaster>()->OpenNewMapDialog();
        } break;

        case KEY_Q: {
                MC->Exit();
        } break;

        case KEY_S: {
                EditMaster* editMaster{ GetSubsystem<EditMaster>() };
                editMaster->SaveMap(editMaster->GetCurrentBlockMap(), BLOCKMAP);
        } break;

        case KEY_Y: {
            GetSubsystem<History>()->Redo();

        } break;

        case KEY_Z: {
            if (!shiftDown_) {
                GetSubsystem<History>()->Undo();

            } else {
                GetSubsystem<History>()->Redo();
            }
        }

        default: break;
        }
    }
}

void InputMaster::HandleKeyUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    pressedKeys_.Erase( eventData[KeyUp::P_KEY].GetInt() );
}

void InputMaster::HandleJoyButtonDown(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    pressedJoystickButtons_.Insert( eventData[JoystickButtonDown::P_BUTTON].GetInt() );
}
void InputMaster::UpdateModifierKeys()
{
    bool oldShift { shiftDown_ };
    bool oldCtrl  { ctrlDown_  };
    bool oldAlt   { altDown_   };

    shiftDown_ = INPUT->GetKeyDown(KEY_SHIFT) || INPUT->GetKeyDown(KEY_LSHIFT) || INPUT->GetKeyDown(KEY_RSHIFT);
    ctrlDown_  = INPUT->GetKeyDown(KEY_CTRL)  || INPUT->GetKeyDown(KEY_LCTRL)  || INPUT->GetKeyDown(KEY_RCTRL);
    altDown_   = INPUT->GetKeyDown(KEY_ALT)   || INPUT->GetKeyDown(KEY_LALT)   || INPUT->GetKeyDown(KEY_RALT);

    if (shiftDown_ != oldShift
     || ctrlDown_  != oldCtrl
     || altDown_   != oldAlt)

        GetSubsystem<EditMaster>()->GetTool()->UpdatePreview(shiftDown_, ctrlDown_, altDown_);
}

void InputMaster::HandleJoyButtonUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    pressedJoystickButtons_.Erase( eventData[JoystickButtonUp::P_BUTTON].GetInt() );
}
void InputMaster::HandleJoystickAxisMove(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    int axis{ eventData[JoystickAxisMove::P_AXIS].GetInt() };
    float position{ eventData[JoystickAxisMove::P_POSITION].GetFloat() };

    if (axis == 0){
        leftStickPosition_.x_ = position;
    } else if (axis == 1) {
        leftStickPosition_.y_ = -position;
    } else if (axis == 2) {
        rightStickPosition_.x_ = position;
    } else if (axis == 3) {
        rightStickPosition_.y_ = -position;
    }
}

IntVector3 InputMaster::CorrectForCameraYaw(IntVector3 intVec3)
{
    float quadrant{0.0f};
    for (float angle: {90.0f, 180.0f, 270.0f}){

        if (LucKey::Delta(MC->GetCamera()->GetYaw(), angle, true) <
            LucKey::Delta(MC->GetCamera()->GetYaw(), quadrant, true))
        {
            quadrant = angle;
        }

    }

    Vector3 vec3{ static_cast<float>(intVec3.x_),
                  static_cast<float>(intVec3.y_),
                  static_cast<float>(intVec3.z_) };

    vec3 = Quaternion(quadrant, Vector3::BACK) * vec3;

    intVec3 = IntVector3( static_cast<int>(Round(vec3.x_)),
                          static_cast<int>(Round(vec3.y_)),
                          static_cast<int>(Round(vec3.z_)) );

    return intVec3;
}

void InputMaster::HandleMouseMove(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    Vector2 mousePos{ static_cast<float>(eventData[MouseMove::P_X].GetInt()) / GRAPHICS->GetWidth(),
                      static_cast<float>(eventData[MouseMove::P_Y].GetInt()) / GRAPHICS->GetHeight()
                    };
    Vector2 dPos{ static_cast<float>(eventData[MouseMove::P_DX].GetInt()) / GRAPHICS->GetWidth(),
                  static_cast<float>(eventData[MouseMove::P_DY].GetInt()) / GRAPHICS->GetHeight()
                };

    mouseRay_ = MC->GetCamera()->GetScreenRay(mousePos.x_, mousePos.y_);

    if (INPUT->GetMouseButtonDown(2)) {

        if (shiftDown_ && ctrlDown_) {

            MC->GetCamera()->Move(Vector3::ONE * dPos.y_, MT_FOV);

        } else if (shiftDown_) {

            MC->GetCamera()->Move(Vector3(dPos.x_, dPos.y_, 0.0f), MT_PAN);

        } else if (ctrlDown_) {
            //Zoom
            MC->GetCamera()->Move(Vector3::FORWARD * -dPos.y_, MT_PAN);

        } else {

            MC->GetCamera()->Move(Vector3(dPos.x_, dPos.y_, 0.0f), MT_ROTATE);

        }

    } else {

        cursor_->HandleMouseMove();
    }
}

void InputMaster::HandleMouseButtonDown(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    pressedMouseButtons_.Insert( eventData[MouseButtonDown::P_BUTTON].GetInt() );

    if (pressedMouseButtons_.Contains(MOUSEB_MIDDLE))
        INPUT->SetMouseMode(MM_WRAP);
}

void InputMaster::HandleMouseButtonUp(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    pressedMouseButtons_.Erase( eventData[MouseButtonUp::P_BUTTON].GetInt() );

    if (!pressedMouseButtons_.Contains(MOUSEB_MIDDLE))
        INPUT->SetMouseMode(MM_FREE);
}

Ray InputMaster::GetMouseRay()
{
    return mouseRay_;
}
