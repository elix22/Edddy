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
#include "edddycam.h"

using namespace LucKey;

InputMaster::InputMaster(Context* context) : Object(context)
{
    keyBindings_[KEY_UP]     = buttonBindings_[static_cast<int>(SB_DPAD_UP)]    = InputAction::UP;
    keyBindings_[KEY_DOWN]   = buttonBindings_[static_cast<int>(SB_DPAD_DOWN)]  = InputAction::DOWN;
    keyBindings_[KEY_LEFT]   = buttonBindings_[static_cast<int>(SB_DPAD_LEFT)]  = InputAction::LEFT;
    keyBindings_[KEY_RIGHT]  = buttonBindings_[static_cast<int>(SB_DPAD_RIGHT)] = InputAction::RIGHT;
    keyBindings_[KEY_RETURN] = buttonBindings_[static_cast<int>(SB_CROSS)]      = InputAction::CONFIRM;
    keyBindings_[KEY_ESCAPE] = buttonBindings_[static_cast<int>(SB_CIRCLE)]     = InputAction::CANCEL;
    keyBindings_[KEY_PAUSE]  = buttonBindings_[static_cast<int>(SB_START)]      = InputAction::PAUSE;
    keyBindings_[KEY_ESCAPE] = InputAction::MENU;

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER(InputMaster, HandleJoyButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, URHO3D_HANDLER(InputMaster, HandleJoyButtonUp));
    SubscribeToEvent(E_JOYSTICKAXISMOVE, URHO3D_HANDLER(InputMaster, HandleJoystickAxisMove));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(InputMaster, HandleUpdate));

    INPUT->SetMouseMode(MM_FREE);
}

void InputMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

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

    //Handle the registered actions
    HandleActions(activeActions);
}

void InputMaster::HandleActions(const InputActions& actions)
{
    //Handle master actions
    for (InputAction action : actions){
        switch (action){
        case InputAction::UP:                 break;
        case InputAction::DOWN:               break;
        case InputAction::LEFT:               break;
        case InputAction::RIGHT:              break;
        case InputAction::CONFIRM:            break;
        case InputAction::CANCEL:             break;
        case InputAction::PAUSE:              break;
        case InputAction::MENU: MC->Exit();   break;
        default: break;
        }
    }


//    //Handle player actions
//    for (int p : MC->GetPlayers()){
//        auto playerInputActions = actions.player_[p];

//        Controllable* controlled{controlledByPlayer_[p]};
//        if (controlled){

//            Vector3 stickMove{ Vector3(leftStickPosition_[p-1].x_, 0.0f, leftStickPosition_[p-1].y_) };
//            CorrectForCameraYaw(stickMove);

//            controlled->SetMove(GetMoveFromActions(playerInputActions) + stickMove);

//            std::bitset<4>restActions{};
//            restActions[0] = playerInputActions->Contains(PlayerInputAction::RUN);
//            restActions[1] = playerInputActions->Contains(PlayerInputAction::JUMP);
//            restActions[2] = playerInputActions->Contains(PlayerInputAction::BUBBLE);
//            restActions[3] = playerInputActions->Contains(PlayerInputAction::INTERACT);

//            controlled->SetActions(restActions);
//        }
//    }
}

Vector3 InputMaster::GetMoveFromActions(Vector<InputAction>* actions)
{
    Vector3 move{Vector3::RIGHT *
                (actions->Contains(InputAction::RIGHT) -
                 actions->Contains(InputAction::LEFT))

                + Vector3::UP *
                (actions->Contains(InputAction::UP) -
                 actions->Contains(InputAction::DOWN))

                + Vector3::FORWARD *
                (actions->Contains(InputAction::FORWARD) -
                 actions->Contains(InputAction::BACK))
                };

    CorrectForCameraYaw(move);

    return move;
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int key{eventData[KeyDown::P_KEY].GetInt()};

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
