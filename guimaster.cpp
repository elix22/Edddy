#include "editmaster.h"
#include "history.h"
#include "inputmaster.h"

#include "brush.h"
#include "fill.h"

#include "guimaster.h"

GUIMaster::GUIMaster(Context* context) : Object(context),
    defaultStyle_{CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml")},
    dragBeginPositionMouse_{},
    dragBeginPositionElement_{}
{
    UI* ui{ GetSubsystem<UI>() };
    uiRoot_ = ui->GetRoot();
    uiRoot_->SetDefaultStyle(defaultStyle_);

    CreateToolBar();
    CreateNewMapWindow();
}

void GUIMaster::CreateToolBar()
{
    int buttonSize{ 32 };

    toolBar_ = new BorderImage(context_);
    toolBar_->SetLayoutMode(LM_HORIZONTAL);
    toolBar_->SetLayoutSpacing(8);
    toolBar_->SetFixedHeight(buttonSize + 4);

    uiRoot_->AddChild(toolBar_);
    toolBar_->SetStyle("Menu");

    for (String group : {"File", "Edit", "Tools", "Help"}) {

        bool seperator{ true };

        StringVector items{};
        if (group == "File") {
            items = { "New Map", "Open Map", "Save Map" };
        } else if (group == "Edit") {
            items = { "Undo", "Redo" };
        } else if (group == "Tools") {
            items = { "Rectangle Select", "Brush", "Fill" };
        } else if (group == "Help") {
            items = { "Manual", "About" };
            seperator = false;
        }

        for (String item : items) {
            //Create button
            Button* button{ toolBar_->CreateChild<Button>(item) };
            button->SetVerticalAlignment(VA_CENTER);

            button->SetBlendMode(BLEND_ALPHA);
            Texture* icon{ new Texture2D(context_) };
            icon->LoadFile("Resources/Textures/Icons/" + item.Split(' ').Front() + ".png");
            button->SetTexture(icon);
            button->SetImageRect(IntRect(0, 0, 128, 128));
            button->SetHoverOffset(0, 4);
            button->SetPressedOffset(0, -4);

            button->SetFixedSize(buttonSize, buttonSize);
            button->SetFocusMode(FM_NOTFOCUSABLE);

            //Create tooltip
            ToolTip* toolTip{ button->CreateChild<ToolTip>() };
            toolTip->SetPosition(buttonSize - 4, buttonSize / 3);

            BorderImage* toolTipTextHolder{ toolTip->CreateChild<BorderImage>() };
            toolTipTextHolder->SetStyle("ToolTipBorderImage");

            Text* toolTipText{ toolTipTextHolder->CreateChild<Text>() };
            toolTipText->SetStyleAuto();
            toolTipText->SetText(item);

            SubscribeToEvent(button, E_RELEASED, URHO3D_HANDLER(GUIMaster, HandleToolBarButtonPressed));

            if (item == "Rectangle Select"
             || item == "Manual"
                    || item == "About") {
                button->SetColor(Color(0.0f, 1.0f, 1.0f));
                button->SetBlendMode(BLEND_SUBTRACT);
            }
        }
        //End groups with seperators
        if (seperator) {

            BorderImage* seperatorElement{ toolBar_->CreateChild<BorderImage>() };
            seperatorElement->SetFixedWidth(16);
            seperatorElement->SetFixedHeight(buttonSize);
            seperatorElement->SetBlendMode(BLEND_MULTIPLY);
        }
    }
}

void GUIMaster::HandleToolBarButtonPressed(StringHash eventType, VariantMap& eventData)
{
    UIElement* element{ static_cast<UIElement*>(eventData[Released::P_ELEMENT].GetPtr()) };
    String name{ element->GetName() };

    EditMaster* editMaster{ GetSubsystem<EditMaster>() };

    if (name == "New Map") {
        OpenNewMapDialog();
    } else if (name == "Open Map") {
        editMaster->LoadMap(BLOCKMAP);
    } else if (name == "Save Map") {
        editMaster->SaveMap(editMaster->GetCurrentBlockMap(), BLOCKMAP);
    } else if (name == "Undo") {
        GetSubsystem<History>()->Undo();
    } else if (name == "Redo") {
        GetSubsystem<History>()->Redo();
    } else if (name == "Brush" || name == "Fill") {
        editMaster->SetTool(Tool::GetTool(StringHash(name)));
    }
}

void GUIMaster::CreateNewMapWindow()
{
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
        rowLabel->SetVerticalAlignment(VA_CENTER);
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
            label->SetVerticalAlignment(VA_CENTER);
            subRow->AddChild(label);

            LineEdit* numberEdit{ new LineEdit(context_) };
            numberEdit->SetName((i == 0 ? "Map" : "Block") + axisString);
            numberEdit->SetText(i == 0 ? "10" : "1.0");
            numberEdit->SetMinSize(IntVector2(48, 32));
            numberEdit->SetMaxSize(IntVector2(48, 32));
            numberEdit->SetChildOffset(IntVector2(8, 0));
            Text* lineEditText{ numberEdit->GetTextElement() };
            input_[numberEdit->GetName()] = lineEditText;

            if (i == 0) {

                SubscribeToEvent(numberEdit, E_TEXTFINISHED, URHO3D_HANDLER(GUIMaster, CleanIntInput));
                SubscribeToEvent(numberEdit, E_DEFOCUSED, URHO3D_HANDLER(GUIMaster, CleanIntInput));

            } else {

                SubscribeToEvent(numberEdit, E_TEXTFINISHED, URHO3D_HANDLER(GUIMaster, CleanFloatInput));
                SubscribeToEvent(numberEdit, E_DEFOCUSED, URHO3D_HANDLER(GUIMaster, CleanFloatInput));
            }

            subRow->AddChild(numberEdit);
            inputRow->AddChild(subRow);

            numberEdit->SetStyleAuto();
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

    SubscribeToEvent(GRAPHICS, E_SCREENMODE, URHO3D_HANDLER(GUIMaster, HandleScreenMode));

    newMapWindow_->SetVisible(false);
}

void GUIMaster::HandleScreenMode(StringHash, VariantMap& eventData)
{
    toolBar_->SetFixedWidth(eventData[ScreenMode::P_WIDTH].GetInt());
}

void GUIMaster::OpenNewMapDialog()
{
    newMapWindow_->SetVisible(true);
}

void GUIMaster::HandleDragBegin(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    UIElement* draggedElement{ static_cast<UIElement*>(eventData[DragBegin::P_ELEMENT].GetPtr()) };
    if (!draggedElement)
        return;

    dragBeginPositionMouse_ = INPUT->GetMousePosition();
    dragBeginPositionElement_ = draggedElement->GetPosition();
}


void GUIMaster::HandleDragMove(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    UIElement* draggedElement{ static_cast<UIElement*>(eventData[DragBegin::P_ELEMENT].GetPtr()) };
    if (!draggedElement)
        return;


    IntVector2 dragCurrentPosition{ INPUT->GetMousePosition() };
    draggedElement->SetPosition(dragCurrentPosition - dragBeginPositionMouse_ + dragBeginPositionElement_);
}

void GUIMaster::HandleCloseButtonPushed(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    newMapWindow_->SetVisible(false);
}
void GUIMaster::HandleNewMapButtonPushed(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    for (Text* t : input_.Values()) {
        Log::Write(3, t->GetText());
    }

    IntVector3 mapSize{ std::stoi(input_["MapX"]->GetText().Trimmed().Split('.').Front().Split(',').Front().CString()),
                        std::stoi(input_["MapY"]->GetText().Trimmed().Split('.').Front().Split(',').Front().CString()),
                        std::stoi(input_["MapZ"]->GetText().Trimmed().Split('.').Front().Split(',').Front().CString()) };

    Vector3 blockSize{ std::stof(input_["BlockX"]->GetText().CString()),
                       std::stof(input_["BlockY"]->GetText().CString()),
                       std::stof(input_["BlockZ"]->GetText().CString()) };

    GetSubsystem<EditMaster>()->NewMap(mapSize, blockSize);

    newMapWindow_->SetVisible(false);
}

void GUIMaster::CleanIntInput(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    LineEdit* lineEdit{ static_cast<LineEdit*>(eventData[TextChanged::P_ELEMENT].GetPtr()) };

    if (!lineEdit)
        return;

    String text{ lineEdit->GetText() };
    String cleanText{};
    bool appendZeros{false};

    for (unsigned i{0}; i < text.Length(); ++i) {

        char c{ text.At(i) };

        if (c >= '0' && c <= '9') {

            if (appendZeros || c != '0') {

                cleanText.Append(c);
                appendZeros = true;
            }
        } else if (c == '.' || c == ',') {

            break;
        }
    }

    if (cleanText.Empty() || cleanText == "0")
        cleanText = "1";

    lineEdit->SetText(cleanText);
}
void GUIMaster::CleanFloatInput(StringHash eventType, VariantMap& eventData)
{ (void)eventType;

    LineEdit* lineEdit{ static_cast<LineEdit*>(eventData[TextChanged::P_ELEMENT].GetPtr()) };

    if (!lineEdit)
        return;

    String text{ lineEdit->GetText() };
    String cleanText{};
    bool appendZeros{ false };
    bool foundDecimalMark{ false };

    //Pick out numbers and the first stop or comma
    for (unsigned i{0}; i < text.Length(); ++i) {

        char c{ text.At(i) };

        if (c >= '0' && c <= '9') {

            if (c != '0' || foundDecimalMark)
                appendZeros = true;

            if (c != '0' || appendZeros)
                cleanText.Append(c);

        } else if (!foundDecimalMark && (c == '.' || c == ',')) {

            //Add zero before the decimal mark if no non-zero numbers have been found
            if (!cleanText.Length())
                cleanText.Append('0');

            cleanText.Append('.');
            foundDecimalMark = true;
        }
    }
    if (foundDecimalMark)
        FixTrailingZeros(cleanText);

    if (cleanText.Empty() || cleanText == "0" || cleanText == "0.0")
        cleanText = "1.0";

    lineEdit->SetText(cleanText);
}
void GUIMaster::FixTrailingZeros(String& cleanText)
{
    if (cleanText.Length()) {
        for (unsigned i{ cleanText.Length() - 1 }; i > 1; --i) {

            if (cleanText.At(i) != '0')
                break;
            else if (cleanText.At(i - 1) != '.')
            {
                cleanText.Erase(i);
            }
        }
    }
    //Add a zero to numbers ending in a decimal mark
    if (cleanText.At(cleanText.Length() - 1) == '.')
        cleanText.Append('0');
}
