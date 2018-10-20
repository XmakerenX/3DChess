#include "ChessWindow.h"
#include "../GUI/ListBoxUI.cpp"
#include "../GUI/ComboBoxUI.cpp"
#include "MainMenuDef.h"
#include "pawnsDef.h"
#include <fstream>

//-----------------------------------------------------------------------------
// Name : ChessWindow (constructor)
//-----------------------------------------------------------------------------
ChessWindow::ChessWindow()
:m_optionDialog(*this)
{
    m_scene = new ChessScene(m_promotionGUI);
    m_sceneInput = false;
}

//-----------------------------------------------------------------------------
// Name : ChessWindow (destructor)
//-----------------------------------------------------------------------------
ChessWindow::~ChessWindow()
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
void ChessWindow::initGUI()
{
    initMainMenu();
    initOptionsMenu();
    initPromotionMenu();
}

//-----------------------------------------------------------------------------
// Name : initMainMenu
//-----------------------------------------------------------------------------
void ChessWindow::initMainMenu()
{
    m_dialog.init(200,320, 18, "Main Menu", "", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), m_asset);
    m_dialog.setCaption(false);
    m_dialog.setLocation( (m_winWidth / 2) - (m_dialog.getWidth() / 2), (m_winHeight / 2) - (m_dialog.getHeight() / 2) );
    //m_dialog.initDefControlElements(m_asset);
    m_dialog.initWoodControlElements(m_asset);
    m_dialog.LoadDialogFromFile("MainMenu.txt");
        
    StaticUI* gameTitle;
    m_dialog.addStatic(50, "Chess", -100, -150, 380, 100, &gameTitle);
    std::vector<ELEMENT_FONT> gameTitleFont;
    gameTitleFont.emplace_back(FontInfo("RosewoodStd-Regular.otf", 64) , m_asset.getFont("RosewoodStd-Regular.otf", 64, true));
    gameTitle->setControlFonts(gameTitleFont);
    gameTitle->setTextOrientation(mkFont::TextFormat::Center);
    
    m_dialog.getButton(IDC_NEWGAME)->connectToClick(boost::bind(&ChessWindow::onNewGame, this , _1));
    m_dialog.getButton(IDC_Continue)->connectToClick(boost::bind(&ChessWindow::onContinueGame, this , _1));
    m_dialog.getButton(IDC_OPTIONS)->connectToClick(boost::bind(&ChessWindow::onOptions, this , _1));
    m_dialog.getButton(IDC_CREDITS)->connectToClick(boost::bind(&ChessWindow::onCredits, this , _1));
    m_dialog.getButton(IDC_EXIT)->connectToClick(boost::bind(&ChessWindow::onExitPressed, this , _1));
    
    std::ifstream saveFile("board.sav");
    if (!saveFile.good())
        m_dialog.getButton(IDC_Continue)->setEnabled(false);
}

//-----------------------------------------------------------------------------
// Name : initOptionsMenu
//-----------------------------------------------------------------------------
void ChessWindow::initOptionsMenu()
{
    m_optionDialog.init(100, 100, 18, "Options", "GUITextures/woodBack.png", glm::vec4(1.0f,1.0f,1.0f, 0.8), m_asset);
    m_optionDialog.setLocation( (m_winWidth / 2) - (m_optionDialog.getWidth() / 2), (m_winHeight / 2) - (m_optionDialog.getHeight() / 2) );
    m_optionDialog.setVisible(false);
    m_optionDialog.setCaption(false);
    
    m_optionDialog.getButton(IDC_CANCELBUTTON)->connectToClick(boost::bind(&ChessWindow::onOptionMenuCancel, this, _1));
    m_optionDialog.getButton(IDC_OKBUTTON)->connectToClick(boost::bind(&ChessWindow::onOptionMenuOK, this, _1));
}

//-----------------------------------------------------------------------------
// Name : initPromotionMenu
//-----------------------------------------------------------------------------
void ChessWindow::initPromotionMenu()
{
    m_promotionGUI.init(500, 100, 18,"Select Pawn", "GUITextures/woodBack.png", glm::vec4(1.0f, 1.0f, 1.0f, 0.8), m_asset);
    m_promotionGUI.setCaption(false);
    m_promotionGUI.initWoodControlElements(m_asset);
    m_promotionGUI.LoadDialogFromFile("pawns.txt");
    m_promotionGUI.setLocation( (m_winWidth / 2) - (m_promotionGUI.getWidth() / 2), (m_winHeight / 2) - (m_promotionGUI.getHeight() / 2) );
    m_promotionGUI.setVisible(false);
    GLuint promotionTextureName = m_asset.getTexture("GUITextures/pawnsButtons.png");
	TextureInfo promotionTextureInfo = m_asset.getTextureInfo(promotionTextureName);
    
    std::vector<ELEMENT_GFX> knightGFX;
    knightGFX.emplace_back(Texture(promotionTextureName, promotionTextureInfo.width, promotionTextureInfo.height), Rect(0, 118, 145, 177));
    knightGFX.emplace_back(Texture(promotionTextureName, promotionTextureInfo.width, promotionTextureInfo.height), Rect(0, 177, 145, 236));
    m_promotionGUI.getButton(IDC_KNIGHT)->setControlGFX(knightGFX);
    
    std::vector<ELEMENT_GFX> bishopGFX;
    bishopGFX.emplace_back(Texture(promotionTextureName, promotionTextureInfo.width, promotionTextureInfo.height), Rect(0, 0, 145, 59));
    bishopGFX.emplace_back(Texture(promotionTextureName, promotionTextureInfo.width, promotionTextureInfo.height), Rect(0, 59, 145, 118));
    m_promotionGUI.getButton(IDC_BISHOP)->setControlGFX(bishopGFX);
    
    std::vector<ELEMENT_GFX> rookGFX;
    rookGFX.emplace_back(Texture(promotionTextureName, promotionTextureInfo.width, promotionTextureInfo.height), Rect(145, 0, 290, 59));
    rookGFX.emplace_back(Texture(promotionTextureName, promotionTextureInfo.width, promotionTextureInfo.height), Rect(145, 59, 290, 118));
    m_promotionGUI.getButton(IDC_ROOK)->setControlGFX(rookGFX);
    
    std::vector<ELEMENT_GFX> queenGFX;
    queenGFX.emplace_back(Texture(promotionTextureName, promotionTextureInfo.width, promotionTextureInfo.height), Rect(290, 0, 435, 59));
    queenGFX.emplace_back(Texture(promotionTextureName, promotionTextureInfo.width, promotionTextureInfo.height), Rect(290, 59, 435, 118));
    m_promotionGUI.getButton(IDC_QUEEN)->setControlGFX(queenGFX);
}

//-----------------------------------------------------------------------------
// Name : renderGUI
//-----------------------------------------------------------------------------
void ChessWindow::renderGUI()
{
      m_dialog.OnRender(m_sprites, m_topSprites, m_asset, timer.getCurrentTime());
      m_optionDialog.OnRender(m_sprites, m_topSprites, m_asset, timer.getCurrentTime());
      m_promotionGUI.OnRender(m_sprites, m_topSprites, m_asset, timer.getCurrentTime());
}

//-----------------------------------------------------------------------------
// Name : sendKeyEvent
//-----------------------------------------------------------------------------
void ChessWindow::sendKeyEvent(unsigned char key, bool down)
{
    if (key == 27 && down)
    {
        if (!m_optionDialog.getVisible() && !m_promotionGUI.getVisible())
        {
            m_dialog.setVisible(!m_dialog.getVisible());
            if (m_dialog.getVisible())
                static_cast<ChessScene*>(m_scene)->setCameraRotationMode(ChessScene::RotationMode::Infinite);
            else
                static_cast<ChessScene*>(m_scene)->setCameraRotationMode(ChessScene::RotationMode::ReturnToWhite);
            m_sceneInput = !m_dialog.getVisible();
        }
    }
        
    m_dialog.handleKeyEvent(key, down);
    m_optionDialog.handleKeyEvent(key, down);
    m_promotionGUI.handleKeyEvent(key, down);
}

//-----------------------------------------------------------------------------
// Name : sendVirtualKeyEvent
//-----------------------------------------------------------------------------
void ChessWindow::sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates)
{
    m_dialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
    m_optionDialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
    m_promotionGUI.handleVirtualKeyEvent(virtualKey, down, modifierStates);
}

//-----------------------------------------------------------------------------
// Name : sendMouseEvent
//-----------------------------------------------------------------------------
void ChessWindow::sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    GameWin::sendMouseEvent(event, modifierStates);
    m_dialog.handleMouseEvent(event, modifierStates);
    m_optionDialog.handleMouseEvent(event, modifierStates);
    m_promotionGUI.handleMouseEvent(event, modifierStates);
}

//-----------------------------------------------------------------------------
// Name : onSizeChanged
//-----------------------------------------------------------------------------
void ChessWindow::onSizeChanged()
{
    m_dialog.setLocation( (m_winWidth / 2) - (m_dialog.getWidth() / 2), (m_winHeight / 2) - (m_dialog.getHeight() / 2) );
    m_promotionGUI.setLocation( (m_winWidth / 2) - (m_promotionGUI.getWidth() / 2), (m_winHeight / 2) - (m_promotionGUI.getHeight() / 2) );
    m_optionDialog.setLocation( (m_winWidth / 2) - (m_optionDialog.getWidth() / 2), (m_winHeight / 2) - (m_optionDialog.getHeight() / 2) );
}

//-----------------------------------------------------------------------------
// Name : onNewGame
//-----------------------------------------------------------------------------
void ChessWindow::onNewGame(ButtonUI* newGameButton)
{
    m_dialog.setVisible(false);
    m_sceneInput = true;
    static_cast<ChessScene*>(m_scene)->newGame();
    static_cast<ChessScene*>(m_scene)->setCameraRotationMode(ChessScene::RotationMode::ReturnToWhite);
}

//-----------------------------------------------------------------------------
// Name : onContinueGame
//-----------------------------------------------------------------------------
void ChessWindow::onContinueGame(ButtonUI* continuButton)
{
    m_dialog.setVisible(false);
    m_sceneInput = true;
    static_cast<ChessScene*>(m_scene)->loadGame();
    static_cast<ChessScene*>(m_scene)->setCameraRotationMode(ChessScene::RotationMode::ReturnToWhite);
}

//-----------------------------------------------------------------------------
// Name : onOptions
//-----------------------------------------------------------------------------
void ChessWindow::onOptions(ButtonUI* optionsButton)
{
    m_dialog.setVisible(false);
    m_optionDialog.setVisible(true);
}

//-----------------------------------------------------------------------------
// Name : onCredits
//-----------------------------------------------------------------------------
void ChessWindow::onCredits(ButtonUI* creditsButton)
{
    Point windowPos = getWindowPosition();
    std::cout << "Window Position is " << windowPos.x << "," << windowPos.y << "\n";
}

//-----------------------------------------------------------------------------
// Name : onExitPressed
//-----------------------------------------------------------------------------
void ChessWindow::onExitPressed(ButtonUI* exitButton)
{
    gameRunning = false;
}

//-----------------------------------------------------------------------------
// Name : onOptionMenuCancel
//-----------------------------------------------------------------------------
void ChessWindow::onOptionMenuCancel(ButtonUI* cancelButton)
{
    m_optionDialog.setVisible(false);
    m_dialog.setVisible(true);
}

//-----------------------------------------------------------------------------
// Name : onOptionMenuOK
//-----------------------------------------------------------------------------
void ChessWindow::onOptionMenuOK(ButtonUI* okButton)
{
    m_optionDialog.setVisible(false);
    m_dialog.setVisible(true);
    
    bool fullscreen = m_optionDialog.getRadioButton(IDC_FULLSCREENRADIO)->getChecked();
    int monitorIndex = *(m_optionDialog.getComboBox<int>(IDC_MONITOR)->GetSelectedData());
    
    setFullScreenMode(false);
    moveWindowToMonitor(monitorIndex);
    
    if (fullscreen)
    {
        Resolution* newRes = m_optionDialog.getComboBox<Resolution>(IDC_RESOLUTIONCOM)->GetSelectedData();
        setMonitorResolution(monitorIndex, *newRes);
        setFullScreenMode(true);
    }
    
    int rotationEnabled = *(m_optionDialog.getComboBox<int>(IDC_ROTATIONCOM)->GetSelectedData());
    if (rotationEnabled == 1)
        static_cast<ChessScene*>(m_scene)->setCameraRotaion(true);
    else
        static_cast<ChessScene*>(m_scene)->setCameraRotaion(false);
    
}
