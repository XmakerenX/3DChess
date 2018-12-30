#include "EditorDialog.h"
#include "../Graphics/GUI/DialogUI.cpp"

//-----------------------------------------------------------------------------
// Name : EditorDialogUI (constructor)
//-----------------------------------------------------------------------------
EditDialogUI::EditDialogUI()
{    
    m_GenControlNum = 0;
    m_curControlID = IDC_GENCONTROLID + m_GenControlNum;
    m_controlInCreation = false;
    m_controlRelocate= false;
    m_pCurSelectedControl = nullptr;
    
    m_GenDialog.connectToControlRightClicked( boost::bind(&EditDialogUI::GenControlRightClicked, this, _1));
}

//-----------------------------------------------------------------------------
// Name : EditDialogUI (destructor)
//-----------------------------------------------------------------------------
EditDialogUI::~EditDialogUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : handleKeyEvent ()
//-----------------------------------------------------------------------------
bool EditDialogUI::handleKeyEvent(unsigned char key, bool down)
{
    if (m_GenDialog.handleKeyEvent(key, down))
        return true;

    if (DialogUI::handleKeyEvent(key, down))
        return true;

    return false;
}

//-----------------------------------------------------------------------------
// Name : handleVirtualKeyEvent ()
//-----------------------------------------------------------------------------
bool EditDialogUI::handleVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates &modifierStates)
{
    if (m_GenDialog.handleVirtualKeyEvent(virtualKey, down, modifierStates))
        return true;

    if(DialogUI::handleVirtualKeyEvent(virtualKey, down, modifierStates))
        return true;

    return false;
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent ()
//-----------------------------------------------------------------------------
bool EditDialogUI::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{    
    if (m_GenDialog.handleMouseEvent(event, modifierStates))
        return true;

    if (DialogUI::handleMouseEvent(event, modifierStates))
        return true;

    Point& cursorPos = event.cursorPos;
    bool& down = event.down;

    switch(event.type)
    {
    case MouseEventType::LeftButton:
    {
        if (down)
        {
            //set new control in the his location
            if (m_controlInCreation)
            {
                Point genDailogLoc = m_GenDialog.getLocation();
                genDailogLoc.y += m_GenDialog.getCaptionHeight();
                int x = cursorPos.x - genDailogLoc.x;
                int y = cursorPos.y - genDailogLoc.y;
                m_GenDialog.getControl(m_curControlID + 1)->setLocation(x, y);
                m_GenDialog.getControl(m_curControlID + 1)->setEnabled(true);
                m_controlInCreation = false;
                m_GenControlNum++;
                m_curControlID++;
                return true;
            }

            // relocate the selected control to his new location
            if (m_controlRelocate && m_pCurSelectedControl)
            {
                Point mousePoint;
                Point dialogPoint = m_GenDialog.getLocation();
                mousePoint.x = cursorPos.x - dialogPoint.x;
                mousePoint.y = cursorPos.y - ( dialogPoint.y + m_GenDialog.getCaptionHeight() );
                //ClientToScreen(m_hWnd,&mousePoint);

                m_pCurSelectedControl->setLocation(mousePoint.x,mousePoint.y);
                m_pCurSelectedControl->setEnabled(true);
                m_controlRelocate = false;
                return true;
            }
        }
    }break;

    case MouseEventType::MouseMoved:
    {
        // move the control being created with the mouse
        if (m_controlInCreation)
        {
            Point genDialogLog = m_GenDialog.getLocation();
            genDialogLog.y += m_GenDialog.getCaptionHeight();
            int x = cursorPos.x;
            int y = cursorPos.y;
            m_GenDialog.getControl(m_curControlID + 1)->setLocation( x - genDialogLog.x, y - genDialogLog.y);
        }

        // move the control being relocated with the mouse
        if (m_controlRelocate && m_pCurSelectedControl)
        {
            Point mousePoint;
            Point dialogPoint = m_GenDialog.getLocation();
            dialogPoint.y += m_GenDialog.getCaptionHeight();

            mousePoint.x = cursorPos.x;
            mousePoint.y = cursorPos.y;
            //ClientToScreen(hWnd,&mousePoint);

            m_pCurSelectedControl->setLocation(mousePoint.x - dialogPoint.x ,mousePoint.y - dialogPoint.y );
        }
        return false;
    }break;

    case MouseEventType::RightButton:
    {
        if (down)
        {
            // if RightButton was pressed but not handled by Gendailog then no control
            // in it was pressed , reset to defualt ui state
            getButton(IDC_CREATECONTROL)->setVisible(true);
            getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
            getButton(IDC_RELOCATEBUTTON)->setEnabled(false);
            return true;
        }
    }break;
    
    default:
        return false;

    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : OnRender ()
//-----------------------------------------------------------------------------
bool EditDialogUI::OnRender(Sprite sprites[ControlUI::SPRITES_SIZE], Sprite topSprites[ControlUI::SPRITES_SIZE], AssetManager& assetManger, double timeStamp)
{
    bool ret = DialogUI::OnRender(sprites, topSprites, assetManger, timeStamp);

    if (ret)
        return m_GenDialog.OnRender(sprites, topSprites, assetManger, timeStamp);
    else
        return ret;

}

//-----------------------------------------------------------------------------
// Name : CreateDialogGUI ()
//-----------------------------------------------------------------------------
void EditDialogUI::CreateDialogUI(AssetManager& assetManager)
{
    RemoveAllControls();

    //-----------------------------------------------------------------------------
    // Dialog initialization
    //-----------------------------------------------------------------------------
    addStatic(IDC_CONTROLTYPESTATIC, "Control Type", 125, 0, 200, 24);

    ComboBoxUI<int>* pComboBox;
    addComboBox<int>(IDC_COMBOX, "", 125, 25, 200, 24, 0, &pComboBox);
    pComboBox->AddItem("Static",      ControlUI::STATIC );
    pComboBox->AddItem("Button",      ControlUI::BUTTON);
    pComboBox->AddItem("CheckBox",    ControlUI::CHECKBOX);
    pComboBox->AddItem("RadioButton", ControlUI::RADIOBUTTON);
    pComboBox->AddItem("ComboBox",    ControlUI::COMBOBOX);
    pComboBox->AddItem("ListBox",     ControlUI::LISTBOX);
    pComboBox->AddItem("EditBox",     ControlUI::EDITBOX);
    pComboBox->AddItem("Slider",      ControlUI::SLIDER);

    pComboBox->ConnectToSelectChg(boost::bind(&EditDialogUI::ComboboxSelChg, this, _1));

    addStatic(IDC_WIDTHSTATIC, "Width", 125, 50, 60, 24);
    addStatic(IDC_HEIGHTSTATIC, "Height", 265, 50, 60, 24);

    addEditbox(IDC_WIDTHEDITBOX, "", 125, 75, 70, 32);
    addEditbox(IDC_HEIGHTEDITBOX, "", 255, 75, 70, 32);

    addStatic(IDC_IDSTATIC, "Control ID",125, 110, 200, 24);
    addEditbox(IDC_IDEDITBOX, "IDC_", 125, 134, 200, 34);

    addStatic(IDC_CONTROLTEXT, "Control Text", 125, 168, 200, 34);
    addEditbox(IDC_TEXTEDITBOX, "", 125, 202, 200, 34);

    //-----------------------------------------------------------------------------
    // Radio button menu initialization
    //-----------------------------------------------------------------------------
    StaticUI* pRadioLabel = nullptr;
    EditBoxUI* pRadioEditbox = nullptr;
    addStatic(IDC_RADIOGROUPTEXT, "Radio Button Group", 125, 236, 200, 34, &pRadioLabel);
    addEditbox(IDC_RADIOBUTTONGROUP, "0", 125, 270, 30, 34, &pRadioEditbox);

    pRadioLabel->setVisible(false);
    pRadioEditbox->setVisible(false);

    //-----------------------------------------------------------------------------
    // Slider menu initialization
    //-----------------------------------------------------------------------------
    StaticUI* pSliderLabel = nullptr;
    EditBoxUI* pSliderMinEditbox = nullptr;
    EditBoxUI* pSliderMaxEditbox = nullptr;

    addStatic(IDC_SLIDERSTATIC, "Slider Range", 125, 236, 200, 34, &pSliderLabel);
    addEditbox(IDC_SLIDERMINEDITBOX, "0", 125, 270, 50, 34, &pSliderMinEditbox);
    addEditbox(IDC_SLIDERMAXEDITBOX, "100", 275, 270, 50, 34, &pSliderMaxEditbox);

    pSliderLabel->setVisible(false);
    pSliderMinEditbox->setVisible(false);
    pSliderMaxEditbox->setVisible(false);

    //-----------------------------------------------------------------------------
    // listbox menu initialization
    //-----------------------------------------------------------------------------
    ListBoxUI<int>* pListBoxNewItems = nullptr;
    StaticUI* pListBoxStatic = nullptr;
    EditBoxUI* pListBoxEditBox = nullptr;
    ButtonUI* pAddItem = nullptr;
    ButtonUI* pRemoveItem = nullptr;

    addListBox(IDC_LISTBOXITEMS, 125, 190, 200, 170, 0, &pListBoxNewItems);
    addStatic(IDC_LISTBOXSTATIC, "ListBox Item text", 125, 355, 200, 34, &pListBoxStatic);
    addEditbox(IDCLISTOXEDITBOX,"", 125, 380, 200, 34, &pListBoxEditBox);
    addButton(IDC_LISTBOXITEMSADD, "Add Item", 125, 424, 85, 34, 0, &pAddItem);
    addButton(IDC_LISTBOXITEMSREMOVE, "Remove Item", 220, 424, 105, 34, 0, &pRemoveItem);

    pListBoxNewItems->setVisible(false);
    pListBoxStatic->setVisible(false);
    pListBoxEditBox->setVisible(false);
    pAddItem->setVisible(false);
    pRemoveItem->setVisible(false);

    pAddItem->connectToClick(boost::bind(&EditDialogUI::AddListBoxItemClicked, this, _1) );
    pRemoveItem->connectToClick(boost::bind(&EditDialogUI::RemoveListBoxItemClikced, this, _1) );

    //-----------------------------------------------------------------------------
    // Combobox menu initialization
    //-----------------------------------------------------------------------------
    ComboBoxUI<int>* pComboboxNewItems = nullptr;
    ButtonUI* pComboAddItems = nullptr;
    ButtonUI* pComboRemoveItems = nullptr;


    addComboBox<int>(IDC_COMBOXITEMS,"", 125, 190, 200, 95, 0, &pComboboxNewItems);
    addButton(IDC_COMBOBOXITEMSADD, "Add Item", 125, 424, 85, 34, 0, &pComboAddItems);
    addButton(IDC_COMBOBOXITEMSREMOVE, "Remove Item", 220, 424, 105, 34, 0, &pComboRemoveItems);

    pComboboxNewItems->setVisible(false);
    pComboAddItems->setVisible(false);
    pComboRemoveItems->setVisible(false);

    pComboAddItems->connectToClick(boost::bind(&EditDialogUI::AddComboBoxItemClicked, this, _1) );
    pComboRemoveItems->connectToClick(boost::bind(&EditDialogUI::RemoveComboBoxItemClicked, this, _1));

    //-----------------------------------------------------------------------------
    // The end of Dialog initialization and creation of create control button
    //-----------------------------------------------------------------------------
    ButtonUI* pCreateControlButton = nullptr;
    ButtonUI* pSetChangesButton = nullptr;

    addButton(IDC_CREATECONTROL, "Create Control",150, 310, 150, 25, 0, &pCreateControlButton);
    pCreateControlButton->connectToClick( boost::bind(&EditDialogUI::CreateControlClicked, this, _1) );

    addButton(IDC_SETCHANGESBUTTON, "Set Changes", 150, 310, 150, 25,0, &pSetChangesButton);
    pSetChangesButton->connectToClick( boost::bind(&EditDialogUI::SetChangesButtonClicked, this, _1) );
    pSetChangesButton->setVisible(false);


    //-----------------------------------------------------------------------------
    // Dialog resize menu creation
    //-----------------------------------------------------------------------------
    StaticUI* pDialogStatic = nullptr;
    EditBoxUI* pDialogWidth = nullptr;
    EditBoxUI* pDialogHeight = nullptr;
    ButtonUI* pDialogSet = nullptr;

    addStatic(IDC_DIALOGSTATIC,"Dialog Size", 125, 480, 200, 60, &pDialogStatic);
    addEditbox(IDC_DIALOGWIDTH, "", 125, 530, 50, 34, &pDialogWidth);
    addEditbox(IDC_DIALOGHEIGHT, "", 275, 530, 50, 34, &pDialogHeight );
    addButton(IDC_DIALOGSETSIZE, "Set Size", 150, 569, 150, 25, 0,&pDialogSet);

    pDialogSet->connectToClick(boost::bind(&EditDialogUI::SetGenDialogSize, this, _1) );

    //-----------------------------------------------------------------------------
    // Dialog resize menu creation
    //-----------------------------------------------------------------------------
    StaticUI* pFileNameStatic = nullptr;
    EditBoxUI* pFileNameEditBox = nullptr;
    ButtonUI* pLoadFileButton = nullptr;
    ButtonUI* pSaveFilButton = nullptr;

    addStatic(IDC_FILENAMESTATIC,"FIle Name", 125, 579, 200, 60, &pFileNameStatic);
    addEditbox(IDC_FILENAMEEDITBOX, "", 125, 624, 200, 34, &pFileNameEditBox);
    addButton(IDC_LOADFILEBUTTON, "Load",125, 663, 70, 34, 0, &pLoadFileButton);
    addButton(IDC_SAVEFILEBUTTON, "Save", 255, 663,70, 34, 0, &pSaveFilButton);

    pLoadFileButton->connectToClick(boost::bind(&EditDialogUI::LoadDialogButtonClicked, this, _1) );
    pSaveFilButton->connectToClick(boost::bind(&EditDialogUI::SaveDialogButtonClicked, this, _1) );

    EditBoxUI* pControlXEditBox = nullptr;
    EditBoxUI* pControlYEditBox = nullptr;

    addStatic(IDC_CONTROLXSTATIC, "X", 330, 592, 50, 34);
    addStatic(IDC_CONTROLYSTATIC, "Y", 385, 592, 50, 34);

    addEditbox(IDC_CONTROLX, "", 330, 625, 50, 34, &pControlXEditBox);
    addEditbox(IDC_CONTROLY, "", 385, 625, 50, 34, &pControlYEditBox);

    ButtonUI* pRelocateButton = nullptr;
    addButton(IDC_RELOCATEBUTTON, "Relocate Control", 330, 553, 100, 34, 0, &pRelocateButton);
    pRelocateButton->connectToClick(boost::bind(&EditDialogUI::RelocateControlClicked, this, _1) );
    pRelocateButton->setEnabled(false);

    ButtonUI* pDeleteButton = nullptr;
    addButton(IDC_DELETEBUTTON, "Delete Contorl", 330, 500, 100, 34, 0, &pDeleteButton);
    pDeleteButton->connectToClick(boost::bind(&EditDialogUI::DeleteControlClicked, this, _1) );
    pDeleteButton->setEnabled(false);

    //-----------------------------------------------------------------------------
    // Dialog initialization of the generated Dialog
    //-----------------------------------------------------------------------------
    m_GenDialog.init(500, 200, 18, "Generated Dialog", "data/textures/GUI/dialog.png", glm::vec4(1.0f, 1.0f, 1.0f, 0.8f), assetManager);
    m_GenDialog.setClipboardFunctions(m_clipboardCopyFunc, m_clipboardPasteFunc);
    m_GenDialog.setLocation(0, 50);
    m_GenDialog.initDefControlElements(assetManager);
}

//-----------------------------------------------------------------------------
// Name : GenControlRightClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::GenControlRightClicked(ControlUI* pRightClickedControl)
{
    getComboBox<int>(IDC_COMBOX)->SetSelectedByIndex(pRightClickedControl->getType());

    long long controlWidth = pRightClickedControl->getWidth();
    long long controlHeight = pRightClickedControl->getHeight();

    getEditBox(IDC_WIDTHEDITBOX)->setText( std::to_string(controlWidth).c_str());
    getEditBox(IDC_HEIGHTEDITBOX)->setText( std::to_string(controlHeight).c_str());
    getEditBox(IDC_IDEDITBOX)->setText(m_GenDialog.getControlIDText(pRightClickedControl->getID()));

    long long ControlX = pRightClickedControl->getX();
    long long ControlY = pRightClickedControl->getY();

    getEditBox(IDC_CONTROLX)->setText( std::to_string(ControlX).c_str());
    getEditBox(IDC_CONTROLY)->setText( std::to_string(ControlY).c_str());

    switch (pRightClickedControl->getType())
    {
    case ControlUI::STATIC:
    case ControlUI::BUTTON:
    case ControlUI::EDITBOX:
    case ControlUI::CHECKBOX:
    {
        getEditBox(IDC_TEXTEDITBOX)->setText(static_cast<StaticUI*>(pRightClickedControl)->getText());
        SetStaticGUI(true);
    }break;

    case ControlUI::RADIOBUTTON:
    {
        getEditBox(IDC_TEXTEDITBOX)->setText( static_cast<StaticUI*>(pRightClickedControl)->getText() );

        long long controlButtonGroup = static_cast<RadioButtonUI*>(pRightClickedControl)->getButtonGroup();
        getEditBox(IDC_RADIOBUTTONGROUP)->setText( std::to_string(controlButtonGroup).c_str() );
        SetRadioButtonGUI(true);
    }break;

    case ControlUI::SLIDER:
    {
        int  sliderMin;
        int  sliderMax;
        static_cast<SliderUI*>(pRightClickedControl)->GetRange(sliderMin, sliderMax);

        getEditBox(IDC_SLIDERMINEDITBOX)->setText( std::to_string(static_cast<long long>(sliderMin)).c_str() );
        getEditBox(IDC_SLIDERMAXEDITBOX)->setText( std::to_string(static_cast<long long>(sliderMax)).c_str() );

        SetSliderGUI(true);
    }break;
    
    case ControlUI::LISTBOX:
    {
        getListBox(IDC_LISTBOXITEMS)->CopyItemsFrom(*(static_cast<ListBoxUI<int>*>(pRightClickedControl)));
        getEditBox(IDCLISTOXEDITBOX)->setText("");

        SetListBoxGUI(true);

    }break;
    
    case ControlUI::COMBOBOX:
    {
        getComboBox<int>(IDC_COMBOXITEMS)->CopyItemsFrom(static_cast<ComboBoxUI<int>*>(pRightClickedControl) );
        getEditBox(IDCLISTOXEDITBOX)->setText("");

        SetComboBoxGUI(true);
    }break;
    
    }

    m_pCurSelectedControl = pRightClickedControl;
    getButton(IDC_RELOCATEBUTTON)->setEnabled(true);
}


//-----------------------------------------------------------------------------
// Name : CreateControlClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::CreateControlClicked(ButtonUI* createControl)
{
    Point cursorPoint;
    std::string pControlText;
    std::string pControlIDText;

    int selectedItem = *(getComboBox<int>(IDC_COMBOX)->GetSelectedData());
    GLuint  controlWidth =  std::stoi( getEditBox(IDC_WIDTHEDITBOX)->getText());
    GLuint  controlHeight = std::stoi( getEditBox(IDC_HEIGHTEDITBOX)->getText());

    pControlText = getEditBox(IDC_TEXTEDITBOX)->getText();
    pControlIDText = getEditBox(IDC_IDEDITBOX)->getText();

    cursorPoint = m_window->getCursorPos();

    switch(selectedItem)
    {
        case ControlUI::BUTTON:
        {
            m_GenDialog.addButton(m_curControlID + 1, pControlText, cursorPoint.x,
            cursorPoint.y, controlWidth, controlHeight, 0, nullptr, pControlIDText);
            m_controlInCreation = true;
        }break;

        case ControlUI::CHECKBOX:
        {
            m_GenDialog.addCheckBox(m_curControlID + 1, cursorPoint.x, cursorPoint.y,
            controlWidth, controlHeight, 0, nullptr, pControlIDText);
            m_controlInCreation = true;
        }break;

        case ControlUI::RADIOBUTTON:
        {
            m_GenDialog.addRadioButton(m_curControlID + 1, cursorPoint.x,
            cursorPoint.y, controlWidth, controlHeight, 0, 0, nullptr, pControlIDText);
            m_controlInCreation = true;
        }break;

        case ControlUI::COMBOBOX:
        {
            m_GenDialog.addComboBox<int>(m_curControlID + 1, pControlText, cursorPoint.x,
            cursorPoint.y, controlWidth, controlHeight, 0, nullptr, pControlIDText);

            GLuint comboboxSize = getComboBox<int>(IDC_COMBOXITEMS)->GetNumItems();

            for (GLuint itemIndex = 0; itemIndex < comboboxSize; itemIndex++)
            {
                Item<int>* pCurItem = getComboBox<int>(IDC_COMBOXITEMS)->GetItem(itemIndex);
                m_GenDialog.getComboBox<int>(m_curControlID + 1)->AddItem(pCurItem->strText, pCurItem->data);
            }

            getComboBox<int>(IDC_COMBOXITEMS)->RemoveAllItems();

            m_controlInCreation = true;
        }break;

        case ControlUI::STATIC:
        {
            m_GenDialog.addStatic(m_curControlID + 1, pControlText, cursorPoint.x,
            cursorPoint.y, controlWidth, controlHeight, nullptr, pControlIDText);
            m_controlInCreation = true;
        }break;

        case ControlUI::EDITBOX:
        {
            m_GenDialog.addEditbox(m_curControlID + 1, pControlText, cursorPoint.x,
            cursorPoint.y, controlWidth, controlHeight, nullptr, pControlIDText);
            m_controlInCreation = true;
        }break;

        case ControlUI::LISTBOX:
        {
            m_GenDialog.addListBox(m_curControlID + 1, cursorPoint.x, cursorPoint.y,
            controlWidth, controlHeight, 0, nullptr, pControlIDText);

            GLuint listboxSize = getListBox(IDC_LISTBOXITEMS)->GetNumItems();

            for (GLuint itemIndex = 0; itemIndex < listboxSize; itemIndex++)
            {
                Item<int>* pCurItem = getListBox(IDC_LISTBOXITEMS)->GetItem(itemIndex);
                m_GenDialog.getListBox(m_curControlID + 1)->AddItem(pCurItem->strText, pCurItem->data);
            }

            getListBox(IDC_LISTBOXITEMS)->RemoveAllItems();
            m_controlInCreation = true;
        }break;

        case ControlUI::SLIDER:
        {
            int minValue = std::stoi(getEditBox(IDC_SLIDERMINEDITBOX)->getText());
            int maxValue = std::stoi(getEditBox(IDC_SLIDERMAXEDITBOX)->getText());

            m_GenDialog.addSlider(m_curControlID + 1, cursorPoint.x, cursorPoint.y,
            controlWidth, controlHeight, minValue, maxValue, (maxValue - minValue) / 2, nullptr, pControlIDText);
            m_controlInCreation = true;
        }break;
        
    }

    if (m_controlInCreation)
        // Temporally disabling the control to avoid it picking up messages before it is in place.
        m_GenDialog.getControl(m_curControlID + 1)->setEnabled(false);
}

//-----------------------------------------------------------------------------
// Name : AddListBoxItemClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::AddListBoxItemClicked(ButtonUI* pAddListBoxItemButton)
{
    std::string itemText = getEditBox(IDCLISTOXEDITBOX)->getText();
    getListBox(IDC_LISTBOXITEMS)->AddItem(itemText, 0);
}

//-----------------------------------------------------------------------------
// Name : RemoveListBoxItemClikced ()
//-----------------------------------------------------------------------------
void EditDialogUI::RemoveListBoxItemClikced(ButtonUI* pRecloateControlButton)
{
    GLuint itemIndex = getListBox(IDC_LISTBOXITEMS)->GetSelectedIndices().back();
    getListBox(IDC_LISTBOXITEMS)->RemoveItem(itemIndex);
}

//-----------------------------------------------------------------------------
// Name : AddComboBoxItemClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::AddComboBoxItemClicked(ButtonUI* pAddComboBoxItemButton)
{
    std::string itemText = getEditBox(IDCLISTOXEDITBOX)->getText();
    getComboBox<int>(IDC_COMBOXITEMS)->AddItem(itemText, 0);
}

//-----------------------------------------------------------------------------
// Name : RemoveComboBoxItemClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::RemoveComboBoxItemClicked(ButtonUI* pRemoveComboBoxItemButton)
{
    GLuint itemIndex = getComboBox<int>(IDC_COMBOXITEMS)->GetSelectedIndex();
    getComboBox<int>(IDC_COMBOXITEMS)->RemoveItem(itemIndex);
}

//-----------------------------------------------------------------------------
// Name : SetChangesButtonClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::SetChangesButtonClicked(ButtonUI* pSetChangesButton)
{
    GLuint controlWidth = std::stoi(getEditBox(IDC_WIDTHEDITBOX)->getText() );
    GLuint controlHeight = std::stoi(getEditBox(IDC_HEIGHTEDITBOX)->getText() );

    int controlX = std::stoi(getEditBox(IDC_CONTROLX)->getText());
    int controlY = std::stoi(getEditBox(IDC_CONTROLY)->getText());

    m_pCurSelectedControl->setLocation(controlX, controlY);
    m_pCurSelectedControl->setSize(controlWidth, controlHeight);

    int selectedItem = *(getComboBox<int>(IDC_COMBOX)->GetSelectedData());

    switch(selectedItem)
    {
        case ControlUI::STATIC:
        case ControlUI::BUTTON:
        case ControlUI::EDITBOX:
        case ControlUI::CHECKBOX:
        {
            const std::string& pControlText = getEditBox(IDC_TEXTEDITBOX)->getText();
            static_cast<StaticUI*>(m_pCurSelectedControl)->setText(pControlText);
        }break;

        case ControlUI::RADIOBUTTON:
        {
            const std::string& pControlText = getEditBox(IDC_TEXTEDITBOX)->getText();
            static_cast<RadioButtonUI*>(m_pCurSelectedControl)->setText(pControlText);

            GLuint buttonGroup = std::stoi(getEditBox(IDC_RADIOBUTTONGROUP)->getText() ) ;
            static_cast<RadioButtonUI*>(m_pCurSelectedControl)->setButtonGroup(buttonGroup);
        }break;

        case ControlUI::SLIDER:
        {
            int sliderMin = std::stoi( getEditBox(IDC_SLIDERMINEDITBOX)->getText() );
            int sliderMax = std::stoi( getEditBox(IDC_SLIDERMAXEDITBOX)->getText() );

            static_cast<SliderUI*>(m_pCurSelectedControl)->SetRange(sliderMin, sliderMax);
        }break;

        case ControlUI::LISTBOX:
        {
            static_cast<ListBoxUI<int>*>(m_pCurSelectedControl)->CopyItemsFrom( *(getListBox(IDC_LISTBOXITEMS)) );
        }break;

        case ControlUI::COMBOBOX:
        {
            static_cast<ComboBoxUI<int>*>(m_pCurSelectedControl)->CopyItemsFrom( getComboBox<int>(IDC_COMBOXITEMS) );
        }break;
        
    }

    const std::string& pControlIDText = getEditBox(IDC_IDEDITBOX)->getText();
    m_GenDialog.UpdateControlDefText( pControlIDText, m_pCurSelectedControl->getID());
}

//-----------------------------------------------------------------------------
// Name : SaveDialogButtonClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::SaveDialogButtonClicked(ButtonUI* pSaveButton)
{
    m_GenDialog.SaveDilaogToFile(getEditBox(IDC_FILENAMEEDITBOX)->getText(), m_curControlID );
}

//-----------------------------------------------------------------------------
// Name : LoadDialogButtonClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::LoadDialogButtonClicked(ButtonUI* pLoadButton)
{
    m_GenDialog.LoadDialogFromFile( getEditBox(IDC_FILENAMEEDITBOX)->getText());
    m_curControlID = m_GenDialog.getCurControlID();
    m_GenControlNum = m_GenDialog.getControlsNum();
}

//-----------------------------------------------------------------------------
// Name : SetGenDialogSize ()
//-----------------------------------------------------------------------------
void EditDialogUI::SetGenDialogSize(ButtonUI* pDialogSetButton)
{
    GLuint dialogWidth = std::stoi(getEditBox(IDC_DIALOGWIDTH)->getText() );
    GLuint dialogHeight = std::stoi(getEditBox(IDC_DIALOGHEIGHT)->getText() );

    m_GenDialog.setSize(dialogWidth, dialogHeight);
}

//-----------------------------------------------------------------------------
// Name : RelocateControlClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::RelocateControlClicked(ButtonUI* pRecloateControlButton)
{
    Point mousePoint;
    Point dialogPoint = m_GenDialog.getLocation();
    dialogPoint.y +=  m_GenDialog.getCaptionHeight();
    mousePoint.x = m_pCurSelectedControl->getX() + dialogPoint.x;
    mousePoint.y = m_pCurSelectedControl->getY() + dialogPoint.y;

    m_window->setCursorPos(mousePoint);

    // Temporally disabled the control to prevent it form processing messages till it is placed.
    m_pCurSelectedControl->setEnabled(false);
    m_controlRelocate = true;
}

//-----------------------------------------------------------------------------
// Name : DeleteControlClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::DeleteControlClicked(ButtonUI* pDeleteButton)
{
    m_GenDialog.RemoveControl(m_pCurSelectedControl->getID());
    m_pCurSelectedControl = nullptr;

    getButton(IDC_CREATECONTROL)->setVisible(true);
    getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
    getButton(IDC_RELOCATEBUTTON)->setEnabled(false);
    getButton(IDC_DELETEBUTTON)->setEnabled(false);
}

//-----------------------------------------------------------------------------
// Name : OptionsControlClicked ()
//-----------------------------------------------------------------------------
void EditDialogUI::OptionsControlClicked(ButtonUI* pOptionsButton)
{
	setVisible(true);
}

//-----------------------------------------------------------------------------
// Name : ComboboxSelChg ()
//-----------------------------------------------------------------------------
void EditDialogUI::ComboboxSelChg(ComboBoxUI<int>* pCombobox)
{
    Item<int>* pSelectedItem =  pCombobox->GetSelectedItem();

    int selectedItem = pSelectedItem->data;

    switch(selectedItem)
    {
        case ControlUI::STATIC:
        case ControlUI::BUTTON:
        case ControlUI::EDITBOX:
        case ControlUI::CHECKBOX:
        {
            SetStaticGUI();
        }break;
        
        case ControlUI::RADIOBUTTON:
        {
            SetRadioButtonGUI();
        }break;
        
        case ControlUI::SLIDER:
        {
            SetSliderGUI();
        }break;
        
        case ControlUI::LISTBOX:
        {
            SetListBoxGUI();
        }break;

        case ControlUI::COMBOBOX:
        {
            SetComboBoxGUI();
        }
        
    }
}

//-----------------------------------------------------------------------------
// Name : ComboboxSelChg ()
//-----------------------------------------------------------------------------
void EditDialogUI::SetStaticGUI(bool ControlSelected /* = false */)
{
    getStatic(IDC_RADIOGROUPTEXT)->setVisible(false);
    getEditBox(IDC_RADIOBUTTONGROUP)->setVisible(false);
    getStatic(IDC_SLIDERSTATIC)->setVisible(false);
    getEditBox(IDC_SLIDERMINEDITBOX)->setVisible(false);
    getEditBox(IDC_SLIDERMAXEDITBOX)->setVisible(false);

    getListBox(IDC_LISTBOXITEMS)->setVisible(false);
    getStatic(IDC_LISTBOXSTATIC)->setVisible(false);
    getEditBox(IDCLISTOXEDITBOX)->setVisible(false);
    getButton(IDC_LISTBOXITEMSADD)->setVisible(false);
    getButton(IDC_LISTBOXITEMSREMOVE)->setVisible(false);

    getComboBox<int>(IDC_COMBOXITEMS)->setVisible(false);
    getButton(IDC_COMBOBOXITEMSADD)->setVisible(false);
    getButton(IDC_COMBOBOXITEMSREMOVE)->setVisible(false);

    getStatic(IDC_CONTROLTEXT)->setVisible(true);
    getEditBox(IDC_TEXTEDITBOX)->setVisible(true);

    getButton(IDC_CREATECONTROL)->setLocation(150, 310);
    getButton(IDC_SETCHANGESBUTTON)->setLocation(150, 310);

    if (!ControlSelected)
    {
        getButton(IDC_CREATECONTROL)->setVisible(true);
        getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
    }
    else
    {
        getButton(IDC_SETCHANGESBUTTON)->setVisible(true);
        getButton(IDC_CREATECONTROL)->setVisible(false);
        getButton(IDC_DELETEBUTTON)->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------
// Name : SetRadioButtonGUI ()
//-----------------------------------------------------------------------------
void EditDialogUI::SetRadioButtonGUI(bool ControlSelected /* = false */)
{
    getStatic(IDC_SLIDERSTATIC)->setVisible(false);
    getEditBox(IDC_SLIDERMINEDITBOX)->setVisible(false);
    getEditBox(IDC_SLIDERMAXEDITBOX)->setVisible(false);

    getListBox(IDC_LISTBOXITEMS)->setVisible(false);
    getStatic(IDC_LISTBOXSTATIC)->setVisible(false);
    getEditBox(IDCLISTOXEDITBOX)->setVisible(false);
    getButton(IDC_LISTBOXITEMSADD)->setVisible(false);
    getButton(IDC_LISTBOXITEMSREMOVE)->setVisible(false);

    getComboBox<int>(IDC_COMBOXITEMS)->setVisible(false);
    getButton(IDC_COMBOBOXITEMSADD)->setVisible(false);
    getButton(IDC_COMBOBOXITEMSREMOVE)->setVisible(false);

    getStatic(IDC_RADIOGROUPTEXT)->setVisible(true);
    getEditBox(IDC_RADIOBUTTONGROUP)->setVisible(true);

    getStatic(IDC_CONTROLTEXT)->setVisible(true);
    getEditBox(IDC_TEXTEDITBOX)->setVisible(true);

    getButton(IDC_CREATECONTROL)->setLocation(150, 310);
    getButton(IDC_SETCHANGESBUTTON)->setLocation(150, 310);

    if (!ControlSelected)
    {
        getButton(IDC_CREATECONTROL)->setVisible(true);
        getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
    }
    else
    {
        getButton(IDC_SETCHANGESBUTTON)->setVisible(true);
        getButton(IDC_CREATECONTROL)->setVisible(false);
        getButton(IDC_DELETEBUTTON)->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------
// Name : SetSliderGUI ()
//-----------------------------------------------------------------------------
void EditDialogUI::SetSliderGUI(bool ControlSelected /* = false */)
{
    getStatic(IDC_RADIOGROUPTEXT)->setVisible(false);
    getEditBox(IDC_RADIOBUTTONGROUP)->setVisible(false);

    getListBox(IDC_LISTBOXITEMS)->setVisible(false);
    getStatic(IDC_LISTBOXSTATIC)->setVisible(false);
    getEditBox(IDCLISTOXEDITBOX)->setVisible(false);
    getButton(IDC_LISTBOXITEMSADD)->setVisible(false);
    getButton(IDC_LISTBOXITEMSREMOVE)->setVisible(false);

    getComboBox<int>(IDC_COMBOXITEMS)->setVisible(false);
    getButton(IDC_COMBOBOXITEMSADD)->setVisible(false);
    getButton(IDC_COMBOBOXITEMSREMOVE)->setVisible(false);

    getStatic(IDC_CONTROLTEXT)->setVisible(true);
    getEditBox(IDC_TEXTEDITBOX)->setVisible(true);

    getStatic(IDC_SLIDERSTATIC)->setVisible(true);
    getEditBox(IDC_SLIDERMINEDITBOX)->setVisible(true);
    getEditBox(IDC_SLIDERMAXEDITBOX)->setVisible(true);

    getButton(IDC_CREATECONTROL)->setLocation(150, 310);
    getButton(IDC_SETCHANGESBUTTON)->setLocation(150, 310);

    if (!ControlSelected)
    {
        getButton(IDC_CREATECONTROL)->setVisible(true);
        getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
    }
    else
    {
        getButton(IDC_SETCHANGESBUTTON)->setVisible(true);
        getButton(IDC_CREATECONTROL)->setVisible(false);
        getButton(IDC_DELETEBUTTON)->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------
// Name : SetListBoxGUI ()
//-----------------------------------------------------------------------------
void EditDialogUI::SetListBoxGUI(bool ControlSelected /* = false */)
{
    getStatic(IDC_CONTROLTEXT)->setVisible(false);
    getEditBox(IDC_TEXTEDITBOX)->setVisible(false);

    getStatic(IDC_RADIOGROUPTEXT)->setVisible(false);
    getEditBox(IDC_RADIOBUTTONGROUP)->setVisible(false);

    getStatic(IDC_SLIDERSTATIC)->setVisible(false);
    getEditBox(IDC_SLIDERMINEDITBOX)->setVisible(false);
    getEditBox(IDC_SLIDERMAXEDITBOX)->setVisible(false);

    getComboBox<int>(IDC_COMBOXITEMS)->setVisible(false);
    getButton(IDC_COMBOBOXITEMSADD)->setVisible(false);
    getButton(IDC_COMBOBOXITEMSREMOVE)->setVisible(false);

    getListBox(IDC_LISTBOXITEMS)->setVisible(true);
    getStatic(IDC_LISTBOXSTATIC)->setVisible(true);
    getStatic(IDC_LISTBOXSTATIC)->setText("ListBox Item text");
    getEditBox(IDCLISTOXEDITBOX)->setVisible(true);
    getButton(IDC_LISTBOXITEMSADD)->setVisible(true);
    getButton(IDC_LISTBOXITEMSREMOVE)->setVisible(true);

    getButton(IDC_CREATECONTROL)->setLocation(150, 468);
    getButton(IDC_SETCHANGESBUTTON)->setLocation(150, 468);

    if (!ControlSelected)
    {
        getButton(IDC_CREATECONTROL)->setVisible(true);
        getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
    }
    else
    {
        getButton(IDC_SETCHANGESBUTTON)->setVisible(true);
        getButton(IDC_CREATECONTROL)->setVisible(false);
        getButton(IDC_DELETEBUTTON)->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------
// Name : SetComboBoxGUI ()
//-----------------------------------------------------------------------------
void EditDialogUI::SetComboBoxGUI(bool ControlSelected /* = false */)
{
    getStatic(IDC_CONTROLTEXT)->setVisible(false);
    getEditBox(IDC_TEXTEDITBOX)->setVisible(false);

    getStatic(IDC_RADIOGROUPTEXT)->setVisible(false);
    getEditBox(IDC_RADIOBUTTONGROUP)->setVisible(false);

    getStatic(IDC_SLIDERSTATIC)->setVisible(false);
    getEditBox(IDC_SLIDERMINEDITBOX)->setVisible(false);
    getEditBox(IDC_SLIDERMAXEDITBOX)->setVisible(false);

    getListBox(IDC_LISTBOXITEMS)->setVisible(false);
    getButton(IDC_LISTBOXITEMSADD)->setVisible(false);
    getButton(IDC_LISTBOXITEMSREMOVE)->setVisible(false);

    getStatic(IDC_LISTBOXSTATIC)->setVisible(true);
    getStatic(IDC_LISTBOXSTATIC)->setText("Combobox Item text");
    getEditBox(IDCLISTOXEDITBOX)->setVisible(true);
    getComboBox<int>(IDC_COMBOXITEMS)->setVisible(true);
    getButton(IDC_COMBOBOXITEMSADD)->setVisible(true);
    getButton(IDC_COMBOBOXITEMSREMOVE)->setVisible(true);

    getButton(IDC_CREATECONTROL)->setLocation(150, 468);
    getButton(IDC_SETCHANGESBUTTON)->setLocation(150, 468);


    if (!ControlSelected)
    {
        getButton(IDC_CREATECONTROL)->setVisible(true);
        getButton(IDC_SETCHANGESBUTTON)->setVisible(false);
    }
    else
    {
        getButton(IDC_SETCHANGESBUTTON)->setVisible(true);
        getButton(IDC_CREATECONTROL)->setVisible(false);
        getButton(IDC_DELETEBUTTON)->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------
// Name : SetWindow ()
//-----------------------------------------------------------------------------
void EditDialogUI::SetWindow(BaseWindow* window)
{
    m_window = window;
}
