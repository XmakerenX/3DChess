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
    m_dialog.init(200,320, 18, "Main Menu", "", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), m_asset);
    m_dialog.setCaption(false);
    m_dialog.setLocation( (m_winWidth / 2) - (m_dialog.getWidth() / 2), (m_winHeight / 2) - (m_dialog.getHeight() / 2) );
    //m_dialog.initDefControlElements(m_asset);
    m_dialog.initWoodControlElements(m_asset);
    m_dialog.LoadDialogFromFile("MainMenu2.txt");
        
    StaticUI* gameTitle;
    m_dialog.addStatic(50, "Chess", -100, -150, 380, 100, &gameTitle);
    std::vector<ELEMENT_FONT> gameTitleFont;
    gameTitleFont.emplace_back(FontInfo("RosewoodStd-Regular.otf", 64) , m_asset.getFont("RosewoodStd-Regular.otf", 64, true));
    gameTitle->setControlFonts(gameTitleFont);
}

//-----------------------------------------------------------------------------
// Name : renderGUI
//-----------------------------------------------------------------------------
void TestWin::renderGUI()
{
      m_dialog.OnRender(m_sprites, m_topSprites, m_asset, timer.getCurrentTime());
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

//-----------------------------------------------------------------------------
// Name : onSizeChanged
//-----------------------------------------------------------------------------
void TestWin::onSizeChanged()
{
    m_dialog.setLocation( (m_winWidth / 2) - (m_dialog.getWidth() / 2), (m_winHeight / 2) - (m_dialog.getHeight() / 2) );
}
