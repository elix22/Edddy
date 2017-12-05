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
#include "castmaster.h"
#include "guimaster.h"
#include "edddycam.h"
#include "edddycursor.h"

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
    for (int key : pressedKeys_) {
        if (keyBindings_.Contains(key)) {
            InputAction action{keyBindings_[key]};
            if (!activeActions.Contains(action))
                activeActions.Push(action);
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
}

void InputMaster::HandleActions(const InputActions& actions, float timeStep)
{
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

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

                GetSubsystem<EditMaster>()->NextBlock();
            } break;
            case ACTION_PREVIOUS_BLOCK: {

                GetSubsystem<EditMaster>()->PreviousBlock();
            } break;
            case ACTION_ROTATE_CW: {

                cursor_->Rotate(true);
            } break;
            case ACTION_ROTATE_CCW: {

                cursor_->Rotate(false);
            } break;
            case ACTION_PICKBLOCK: {

                GetSubsystem<EditMaster>()->PickBlock();
            } break;
            case ACTION_CONFIRM: {

                GetSubsystem<EditMaster>()->PutBlock();
            } break;
            case ACTION_CANCEL:           break;
            default: break;
            }
        }
        actionTime_[action] += timeStep;
    }

    for (int a : unusedActions) {

            actionTime_[a] = 0.0f;
    }
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

    pressedKeys_.Insert(key);

    switch (key){
    case KEY_KP_5: {
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
    case KEY_Q: { if (ctrlDown_)
            MC->Exit();
    } break;
    case KEY_S: { if (ctrlDown_) {
            EditMaster* editMaster{ GetSubsystem<EditMaster>() };
            editMaster->SaveMap(GetSubsystem<EditMaster>()->GetCurrentBlockMap(), BLOCKMAP);
        }
    } break;
    case KEY_N: { if (ctrlDown_)
            GetSubsystem<GUIMaster>()->OpenNewMapDialog();
    }
    default: break;
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
    shiftDown_ = INPUT->GetKeyDown(KEY_SHIFT) || INPUT->GetKeyDown(KEY_LSHIFT) || INPUT->GetKeyDown(KEY_RSHIFT);
    ctrlDown_  = INPUT->GetKeyDown(KEY_CTRL)  || INPUT->GetKeyDown(KEY_LCTRL)  || INPUT->GetKeyDown(KEY_RCTRL);
    altDown_   = INPUT->GetKeyDown(KEY_ALT)   || INPUT->GetKeyDown(KEY_LALT)   || INPUT->GetKeyDown(KEY_RALT);
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
