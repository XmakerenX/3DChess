#include "TestWin.h"
#include "../GUI/ListBoxUI.cpp"
#include "MainMenu2Def.h"
#include <fstream>

//-----------------------------------------------------------------------------
// Name : TestWin (constructor)
//-----------------------------------------------------------------------------
TestWin::TestWin()
{
    //m_scene = new Scene();
    m_scene = new ChessScene();
    m_sceneInput = false;
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
    
    m_dialog.getButton(IDC_NEWGAME)->connectToClick(boost::bind(&TestWin::onNewGame, this , _1));
    m_dialog.getButton(IDC_Continue)->connectToClick(boost::bind(&TestWin::onContinueGame, this , _1));
    m_dialog.getButton(IDC_EXIT)->connectToClick(boost::bind(&TestWin::onExitPressed, this , _1));
    
    std::ifstream saveFile("board.sav");
    if (!saveFile.good())
        m_dialog.getButton(IDC_Continue)->setEnabled(false);
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
    if (key == 27 && down)
    {
        m_dialog.setVisible(!m_dialog.getVisible());
        m_sceneInput = !m_dialog.getVisible();
    }
        
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

//-----------------------------------------------------------------------------
// Name : onNewGame
//-----------------------------------------------------------------------------
void TestWin::onNewGame(ButtonUI* newGameButton)
{
    m_dialog.setVisible(false);
    m_sceneInput = true;
    static_cast<ChessScene*>(m_scene)->newGame();
}

//-----------------------------------------------------------------------------
// Name : onNewGame
//-----------------------------------------------------------------------------
void TestWin::onContinueGame(ButtonUI* continuButton)
{
    m_dialog.setVisible(false);
    m_sceneInput = true;
    static_cast<ChessScene*>(m_scene)->loadGame();
}

//-----------------------------------------------------------------------------
// Name : onExitPressed
//-----------------------------------------------------------------------------
void TestWin::onExitPressed(ButtonUI* exitButton)
{
    gameRunning = false;
}
