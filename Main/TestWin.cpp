#include "TestWin.h"
#include "../GUI/ListBoxUI.cpp"

//-----------------------------------------------------------------------------
// Name : TestWin (constructor)
//-----------------------------------------------------------------------------
TestWin::TestWin()
{
    //m_scene = new Scene();
    m_scene = new ChessScene();
}

//-----------------------------------------------------------------------------
// Name : TestWin (destructor)
//-----------------------------------------------------------------------------
TestWin::~TestWin()
{
    if (m_scene != nullptr)
    {
        delete m_scene;
        m_scene = nullptr;
    }
}


//-----------------------------------------------------------------------------
// Name : initGUI
//-----------------------------------------------------------------------------
void TestWin::initGUI()
{
    m_dialog.init(300,300, 18, "Caption!", "", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), m_asset);
    m_dialog.setLocation(50, 100);
    m_dialog.initDefControlElements(m_asset);
    //m_dialog.initWoodControlElements(m_asset);
    ButtonUI* pButton;
    m_dialog.addButton(1, "button text", 20,20, 200, 25, 0, &pButton);
    pButton->setEnabled(false);
    m_dialog.addButton(2, "enabled button", 20, 60, 200, 25, 0);
    m_dialog.addCheckBox(3, 100,100, 50, 50, 0);
    m_dialog.addRadioButton(4, 30, 150, 25,25,0,1);
    m_dialog.addRadioButton(5, 90, 150, 25,25,0,1);
    ComboBoxUI* pCombo;
    //m_dialog.addComboBox(6, "Box", 20, 200, 200, 40, 0, &pCombo);
    m_dialog.addComboBox(6, "Box", 20, 200, 300, 60, 0, &pCombo);
    pCombo->AddItem("Sunday", 1);
    pCombo->AddItem("Monday", 2);
    pCombo->AddItem("Tuesday", 3);
    pCombo->AddItem("Wednesday", 4);
    pCombo->AddItem("Thursday", 5);
    pCombo->AddItem("Friday", 6);
    pCombo->AddItem("Saturday", 7);

    ListBoxUI<int>* pListbox;
    m_dialog.addListBox(7, 350,200, 200,100, true, &pListbox);
    pListbox->AddItem("Sunday", 1);
    pListbox->AddItem("Monday", 2);
    pListbox->AddItem("Tuesday", 3);
    pListbox->AddItem("Wednesday", 4);
    pListbox->AddItem("Thursday", 5);
    pListbox->AddItem("Friday", 6);
    pListbox->AddItem("Saturday", 7);

    m_dialog.addSlider(8, 0, 0, 200, 40, 0, 100, 50);

    m_dialog.addEditbox(9, "Test TgTy",180, 100, 100, 35, nullptr );

    //m_dialog.addButton(10, "Center me", 20, 300, 400, 141, 0);

    m_dialog.addEditbox(11, "Test TgTy",420, 300, 400, 100, nullptr );
}

//-----------------------------------------------------------------------------
// Name : renderGUI
//-----------------------------------------------------------------------------
void TestWin::renderGUI()
{
//      m_dialog.OnRender(m_sprites, m_topSprites, m_asset, timer.getCurrentTime());
//     Point textSize = font_->calcTextRect("[]a");
//     font_->renderToRect(m_sprites[1], "[]a", Rect(0,50, 0 + textSize.x, 50 + textSize.y), WHITE_COLOR);
//     m_sprites[0].AddTintedQuad(Rect(0,50,0 + textSize.x, 50 + textSize.y), glm::vec4(1.0f, 0.0, 0.0, 1.0f));
}

//-----------------------------------------------------------------------------
// Name : sendKeyEvent
//-----------------------------------------------------------------------------
void TestWin::sendKeyEvent(unsigned char key, bool down)
{
    m_dialog.handleKeyEvent(key, down);
}

//-----------------------------------------------------------------------------
// Name : sendVirtualKeyEvent
//-----------------------------------------------------------------------------
void TestWin::sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates)
{
    m_dialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
}

//-----------------------------------------------------------------------------
// Name : sendMouseEvent
//-----------------------------------------------------------------------------
void TestWin::sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    GameWin::sendMouseEvent(event, modifierStates);
    m_dialog.handleMouseEvent(event, modifierStates);
}
