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

#include "inputmaster.h"
#include "editmaster.h"
#include "edddycam.h"
#include "edddycursor.h"

using namespace LucKey;

InputMaster::InputMaster(Context* context) : Object(context)
{
    for (int a{0}; a < static_cast<int>(InputAction::ALL_ACTIONS); ++a){
        actionTime_[a] = 0.0f;
    }

    keyBindings_[KEY_UP]     = buttonBindings_[static_cast<int>(SB_DPAD_UP)]    = ACTION_UP;
    keyBindings_[KEY_DOWN]   = buttonBindings_[static_cast<int>(SB_DPAD_DOWN)]  = ACTION_DOWN;
    keyBindings_[KEY_LEFT]   = buttonBindings_[static_cast<int>(SB_DPAD_LEFT)]  = ACTION_LEFT;
    keyBindings_[KEY_RIGHT]  = buttonBindings_[static_cast<int>(SB_DPAD_RIGHT)] = ACTION_RIGHT;
    keyBindings_[93]/*]*/    = buttonBindings_[static_cast<int>(SB_DPAD_RIGHT)] = ACTION_FORWARD;
    keyBindings_[91]/*[*/    = buttonBindings_[static_cast<int>(SB_DPAD_RIGHT)] = ACTION_BACK;
    keyBindings_[KEY_X]      = ACTION_X_AXIS;
    keyBindings_[KEY_Y]      = ACTION_Y_AXIS;
    keyBindings_[KEY_Z]      = ACTION_Z_AXIS;
    keyBindings_[46]/*.*/    = ACTION_NEXT_BLOCK;
    keyBindings_[44]/*,*/    = ACTION_PREVIOUS_BLOCK;
    keyBindings_[KEY_0]      = ACTION_ROTATE_CCW;
    keyBindings_[KEY_9]      = ACTION_ROTATE_CW;
    keyBindings_[47]/*/*/    = ACTION_PICKBLOCK;
    keyBindings_[KEY_RETURN] = buttonBindings_[static_cast<int>(SB_CROSS)]      = ACTION_CONFIRM;
    keyBindings_[KEY_ESCAPE] = buttonBindings_[static_cast<int>(SB_CIRCLE)]     = ACTION_CANCEL;

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER(InputMaster, HandleJoyButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, URHO3D_HANDLER(InputMaster, HandleJoyButtonUp));
    SubscribeToEvent(E_JOYSTICKAXISMOVE, URHO3D_HANDLER(InputMaster, HandleJoystickAxisMove));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(InputMaster, HandleUpdate));

    INPUT->SetMouseMode(MM_FREE);
}

void InputMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    float timeStep{ eventData[Update::P_TIMESTEP].GetFloat() };
    InputActions activeActions{};

    //Convert key presses to actions
    for (int key : pressedKeys_){
        if (keyBindings_.Contains(key)){
            InputAction action{keyBindings_[key]};
            if (!activeActions.Contains(action))
                activeActions.Push(action);
        }
    }
    //Check for joystick button presses
//    for (int p : MC->GetPlayers()){

//        for (int button : pressedJoystickButtons_[p-1])
//            if (buttonBindingsPlayer_[p].Contains(button)){
//                PlayerInputAction action{ buttonBindingsPlayer_[p][button]};
//                if (!activeActions.player_[p].Contains(action))
//                    activeActions.player_[p].Push(action);
//            }
//    }

    HandleActions(activeActions, timeStep);
}

void InputMaster::HandleActions(const InputActions& actions, float timeStep)
{
    IntVector3 step{GetMoveFromActions(actions)};
    if (step != IntVector3::ZERO){
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
         || actionTime_[action] > ACTION_INTERVAL)
        {
            actionTime_[action] = 0.0f;

            switch (action){
            case ACTION_UP:       case ACTION_DOWN:
            case ACTION_LEFT:     case ACTION_RIGHT:
            case ACTION_FORWARD:  case ACTION_BACK:
                break;
            case ACTION_X_AXIS:{
                std::bitset<3> lock{};
                lock[0] = true;
                if (INPUT->GetKeyDown(KEY_SHIFT))
                    lock.flip();
                cursor_->SetAxisLock(lock);
            } break;
            case ACTION_Y_AXIS:{
                std::bitset<3> lock{};
                lock[1] = true;
                if (INPUT->GetKeyDown(KEY_SHIFT))
                    lock.flip();
                cursor_->SetAxisLock(lock);
            } break;
            case ACTION_Z_AXIS:{
                std::bitset<3> lock{};
                lock[2] = true;
                if (INPUT->GetKeyDown(KEY_SHIFT))
                    lock.flip();
                cursor_->SetAxisLock(lock);
            } break;
            case ACTION_NEXT_BLOCK:{
                GetSubsystem<EditMaster>()->NextBlock();
            } break;
            case ACTION_PREVIOUS_BLOCK:{
                GetSubsystem<EditMaster>()->PreviousBlock();
            } break;
            case ACTION_ROTATE_CW:{
                cursor_->Rotate(true);
            } break;
            case ACTION_ROTATE_CCW:{
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
    for (int a : unusedActions){
        actionTime_[a] = 0.0f;
    }
}

IntVector3 InputMaster::GetMoveFromActions(const InputActions& actions)
{
    IntVector3 move{IntVector3::RIGHT   * (CheckActionable(ACTION_RIGHT, actions) - CheckActionable(ACTION_LEFT, actions))
                  + IntVector3::UP      * (CheckActionable(ACTION_UP, actions) - CheckActionable(ACTION_DOWN, actions))
                  + IntVector3::FORWARD * (CheckActionable(ACTION_FORWARD, actions) - CheckActionable(ACTION_BACK, actions))};

//    CorrectForCameraYaw(move);

    return move;
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

    int key{eventData[KeyDown::P_KEY].GetInt()};

    Log::Write(1, String(key));

    if (!pressedKeys_.Contains(key))
        pressedKeys_.Push(key);

    switch (key){
    case KEY_ESCAPE:{
        MC->Exit();
    } break;
    case KEY_9:{
        Image screenshot(context_);
        Graphics* graphics = GetSubsystem<Graphics>();
        graphics->TakeScreenShot(screenshot);
        //Here we save in the Data folder with date and time appended
        String fileName = GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
                Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png";
        //Log::Write(1, fileName);
        screenshot.SavePNG(fileName);
    } break;
    case KEY_Q:{
        if (INPUT->GetKeyDown(KEY_LCTRL) || INPUT->GetKeyDown(KEY_RCTRL))
            MC->Exit();

    }break;
    default: break;
    }
}

void InputMaster::HandleKeyUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int key{ eventData[KeyUp::P_KEY].GetInt() };

    if (pressedKeys_.Contains(key))
        pressedKeys_.Remove(key);
}

void InputMaster::HandleJoyButtonDown(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickButtonDown::P_JOYSTICKID].GetInt() };
    SixaxisButton button{ static_cast<SixaxisButton>(eventData[JoystickButtonDown::P_BUTTON].GetInt()) };

    if (!pressedJoystickButtons_.Contains(button))
        pressedJoystickButtons_.Push(button);
}
void InputMaster::HandleJoyButtonUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickButtonDown::P_JOYSTICKID].GetInt() };
    SixaxisButton button{ static_cast<SixaxisButton>(eventData[JoystickButtonUp::P_BUTTON].GetInt()) };

    if (pressedJoystickButtons_.Contains(button))
        pressedJoystickButtons_.Remove(button);
}
void InputMaster::HandleJoystickAxisMove(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickAxisMove::P_JOYSTICKID].GetInt() };
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

void InputMaster::CorrectForCameraYaw(Vector3& vec3)
{
    Vector3 camRot{MC->GetCamera()->GetRotation().EulerAngles()};
    vec3 = Quaternion(camRot.y_, Vector3::UP) * vec3;
}
