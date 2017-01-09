#include "editmaster.h"
#include "inputmaster.h"

#include "guimaster.h"

GUIMaster::GUIMaster(Context* context) : Object(context),
    defaultStyle_{CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml")},
    dragBeginPositionMouse_{},
    dragBeginPositionElement_{}
{
    UI* ui{ GetSubsystem<UI>() };
    uiRoot_ = ui->GetRoot();

    CreateNewMapWindow();
}

void GUIMaster::CreateNewMapWindow()
{
    uiRoot_->SetDefaultStyle(defaultStyle_);

    newMapWindow_ = new Window(context_);
    newMapWindow_->SetSize(400, 400);
    newMapWindow_->SetMinSize(100, 50);
    newMapWindow_->SetAlignment(HA_CENTER, VA_CENTER);
    newMapWindow_->SetLayout(LM_VERTICAL, 6, IntRect(5, 5, 5, 5));
    newMapWindow_->SetStyleAuto();
    uiRoot_->AddChild(newMapWindow_);

    UIElement* titleBar{ new UIElement(context_) };
    titleBar->SetLayout(LM_HORIZONTAL, 6, IntRect(5, 5, 5, 5));
    newMapWindow_->AddChild(titleBar);

    Text* title{ new Text(context_) };
    title->SetText("New Map");
    title->SetStyleAuto();
    titleBar->AddChild(title);

    Button* closeButton{ new Button(context_) };
    closeButton->SetStyle("CloseButton");
    titleBar->AddChild(closeButton);

    for (int i{0}; i < 2; ++i) {

        UIElement* inputRow{ new UIElement(context_) };
        inputRow->SetLayout(LM_HORIZONTAL, 6, IntRect(5, 5, 5, 5));
        newMapWindow_->AddChild(inputRow);

        Text* rowLabel{ new Text(context_) };
        rowLabel->SetText(i == 0 ? "Map Size" : "Block Size");
        inputRow->AddChild(rowLabel);

        for (int j{0}; j < 3; ++j) {

            UIElement* subRow{ new UIElement(context_) };
            subRow->SetLayout(LM_HORIZONTAL, 6, IntRect(2, 2, 2, 2));
            subRow->SetMaxWidth(70);

            String axisString{};
            switch(j) {
            case 0:
                axisString = "X";
                break;
            case 1:
                axisString = "Y";
                break;
            case 2:
                axisString = "Z";
                break;
            default:
                break;
            }

            Text* label{ new Text(context_) };
            label->SetText(axisString);
            label->SetHorizontalAlignment(HA_RIGHT);
            subRow->AddChild(label);

            LineEdit* lineEdit{ new LineEdit(context_) };
            lineEdit->SetName(i == 0 ? "Map" : "Block" + axisString);
            lineEdit->SetText(i == 0 ? "10" : "1.0");
            lineEdit->SetMinSize(IntVector2(32, 32));
            lineEdit->SetMaxSize(IntVector2(32, 32));
            Text* lineEditText{ lineEdit->GetTextElement() };
            lineEditText->SetHorizontalAlignment(HA_RIGHT);
            input_[StringHash(lineEdit->GetName())] = lineEditText;

            if (i == 0)
                SubscribeToEvent(lineEdit, E_TEXTFINISHED, URHO3D_HANDLER(GUIMaster, CleanIntInput));
            else
                SubscribeToEvent(lineEdit, E_TEXTFINISHED, URHO3D_HANDLER(GUIMaster, CleanFloatInput));

            subRow->AddChild(lineEdit);
            inputRow->AddChild(subRow);

            lineEdit->SetStyleAuto();
            label->SetStyleAuto();
        }
        rowLabel->SetStyleAuto();

    }

    Button* newMapButton{ new Button(context_) };
    newMapButton->SetPosition(IntVector2(100, 200));
    newMapButton->SetMinSize(IntVector2(128, 32));
    newMapButton->SetMaxSize(IntVector2(256, 64));
    newMapButton->SetHorizontalAlignment(HA_RIGHT);
    newMapButton->SetStyleAuto();
    newMapWindow_->AddChild(newMapButton);

    Text* text{ new Text(context_) };
    text->SetAlignment(HA_CENTER, VA_CENTER);
    text->SetText("New Map");
    text->SetStyleAuto();
    newMapButton->AddChild(text);

    SubscribeToEvent(newMapWindow_, E_DRAGBEGIN, URHO3D_HANDLER(GUIMaster, HandleDragBegin));
    SubscribeToEvent(newMapWindow_, E_DRAGMOVE, URHO3D_HANDLER(GUIMaster, HandleDragMove));
    SubscribeToEvent(closeButton, E_RELEASED, URHO3D_HANDLER(GUIMaster, HandleCloseButtonPushed));
    SubscribeToEvent(newMapButton, E_RELEASED, URHO3D_HANDLER(GUIMaster, HandleNewMapButtonPushed));

    newMapWindow_->SetVisible(false);
}

void GUIMaster::OpenNewMapWindow()
{
    newMapWindow_->SetVisible(true);
}

void GUIMaster::HandleDragBegin(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    UIElement* draggedElement{ static_cast<UIElement*>(eventData[DragBegin::P_ELEMENT].GetPtr()) };
    if (!draggedElement)
        return;


    dragBeginPositionMouse_ = GetSubsystem<InputMaster>()->GetMouseScreenPos();
    dragBeginPositionElement_ = draggedElement->GetPosition();
}


void GUIMaster::HandleDragMove(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    UIElement* draggedElement{ static_cast<UIElement*>(eventData[DragBegin::P_ELEMENT].GetPtr()) };
    if (!draggedElement)
        return;


    IntVector2 dragCurrentPosition{ GetSubsystem<InputMaster>()->GetMouseScreenPos() };
    draggedElement->SetPosition(dragCurrentPosition - dragBeginPositionMouse_ + dragBeginPositionElement_);
}

void GUIMaster::HandleCloseButtonPushed(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    newMapWindow_->SetVisible(false);
}
void GUIMaster::HandleNewMapButtonPushed(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

//    GetSubsystem<EditMaster>()->NewMap(IntVector3(std::stoi(input_[StringHash("MapX")]->GetText().Trimmed().CString()),
//                                                  std::stoi(input_[StringHash("MapY")]->GetText().Trimmed().CString()),
//                                                  std::stoi(input_[StringHash("MapZ")]->GetText().Trimmed().CString())),
//                                        Vector3(std::stof(input_[StringHash("BlockX")]->GetText().Trimmed().CString()),
//                                                std::stof(input_[StringHash("BlockY")]->GetText().Trimmed().CString()),
//                                                std::stof(input_[StringHash("BlockZ")]->GetText().Trimmed().CString())));

    newMapWindow_->SetVisible(false);
}

void GUIMaster::CleanIntInput(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    LineEdit* lineEdit{ static_cast<LineEdit*>(eventData[TextChanged::P_ELEMENT].GetPtr()) };

    if (!lineEdit)
        return;

    String text{ lineEdit->GetText() };
    String cleanText{};
    for (unsigned i{0}; i < text.Length(); ++i) {
        char c{ text.At(i) };

        if (c < 10) {
            cleanText.Append(c);
        }
    }
    lineEdit->SetText(cleanText);
}

void GUIMaster::CleanFloatInput(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    LineEdit* lineEdit{ static_cast<LineEdit*>(eventData[TextChanged::P_ELEMENT].GetPtr()) };

    if (!lineEdit)
        return;

    String text{ lineEdit->GetText() };
    String cleanText{};
    for (unsigned i{0}; i < text.Length(); ++i) {
        bool foundStop{ false };
        char c{ text.At(i) };

        if (c < 10) {

            cleanText.Append(c);

        } else  if (!foundStop && c == '.') {

            cleanText.Append(c);
            foundStop = true;
        }
    }
    lineEdit->SetText(cleanText);
}

//void GUIMaster::HandleNewMapButtonPressed


