//
// 3D Chess - A cross platform Chess game for 2 players made using OpenGL and c++
// Copyright (C) 2016-2020 Matan Keren <xmakerenx@gmail.com>
//
// This file is part of 3D Chess.
//
// 3D Chess is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// 3D Chess is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with 3D Chess.  If not, see <http://www.gnu.org/licenses/>.
//

#include "Chess.h"
#include <Render/GUI/ListBoxUI.cpp>
#include <Render/GUI/ComboBoxUI.cpp>
#include "MainMenuDef.h"
#include "pawnsDef.h"
#include "creditsDef.h"
#include "gameoverDef.h"
#include <fstream>

//-----------------------------------------------------------------------------
// Name : Chess (constructor)
//-----------------------------------------------------------------------------
Chess::Chess()
{
    m_scene = new ChessScene(m_promotionGUI, m_gameOverDialog);
    m_sceneInput = false;
}

//-----------------------------------------------------------------------------
// Name : Chess (destructor)
//-----------------------------------------------------------------------------
Chess::~Chess()
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
void Chess::initGUI()
{
    initMainMenu();
    initOptionsMenu();
    initCreditsMenu();
    initPromotionMenu();
    initKeysMenu();
    initGameOverMenu();
}

//-----------------------------------------------------------------------------
// Name : initMainMenu
//-----------------------------------------------------------------------------
void Chess::initMainMenu()
{
    m_mainMenuDialog.init(200,320, 18, "Main Menu", "", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), m_asset);
    m_mainMenuDialog.setClipboardFunctions(m_window->getCopyToClipboardFunc(), m_window->getPasteClipboardFunc());
    m_mainMenuDialog.setCaption(false);
    m_mainMenuDialog.setLocation( (m_window->getWidth() / 2) - (m_mainMenuDialog.getWidth() / 2), (m_window->getHeight() / 2) - (m_mainMenuDialog.getHeight() / 2) );
    m_mainMenuDialog.initWoodControlElements(m_asset);
    if (!m_mainMenuDialog.LoadDialogFromFile("data/dialogs/MainMenu.txt"))
        return;
        
    StaticUI* gameTitle;
    m_mainMenuDialog.addStatic(50, "Chess", -100, -150, 380, 100, &gameTitle);
    std::vector<ELEMENT_FONT> gameTitleFont;
    gameTitleFont.emplace_back(FontInfo("data/fonts/RosewoodStd-Regular.otf", 64) , m_asset.getFont("data/fonts/RosewoodStd-Regular.otf", 64, true));
    gameTitle->setControlFonts(gameTitleFont);
    gameTitle->setTextOrientation(mkFont::TextFormat::Center);
    
    m_mainMenuDialog.getButton(IDC_NEWGAME)->connectToClick(boost::bind(&Chess::onNewGame, this , _1));
    m_mainMenuDialog.getButton(IDC_Continue)->connectToClick(boost::bind(&Chess::onContinueGame, this , _1));
    m_mainMenuDialog.getButton(IDC_OPTIONS)->connectToClick(boost::bind(&Chess::onOptions, this , _1));
    m_mainMenuDialog.getButton(IDC_CREDITS)->connectToClick(boost::bind(&Chess::onCredits, this , _1));
    m_mainMenuDialog.getButton(IDC_EXIT)->connectToClick(boost::bind(&Chess::onExitPressed, this , _1));
    
    std::ifstream saveFile("board.sav");
    if (!saveFile.good())
        m_mainMenuDialog.getButton(IDC_Continue)->setEnabled(false);
}

//-----------------------------------------------------------------------------
// Name : initOptionsMenu
//-----------------------------------------------------------------------------
void Chess::initOptionsMenu()
{
    m_optionDialog.setParentWindow(m_window);
    m_optionDialog.init(100, 100, 18, "Options", "data/textures/GUI/woodBack.png", glm::vec4(1.0f,1.0f,1.0f, 0.8), m_asset);
    m_optionDialog.setClipboardFunctions(m_window->getCopyToClipboardFunc(), m_window->getPasteClipboardFunc());
    m_optionDialog.setLocation( (m_window->getWidth() / 2) - (m_optionDialog.getWidth() / 2), (m_window->getHeight() / 2) - (m_optionDialog.getHeight() / 2) );
    m_optionDialog.setVisible(false);
    m_optionDialog.setCaption(false);
    
    m_optionDialog.getButton(IDC_CANCELBUTTON)->connectToClick(boost::bind(&Chess::onOptionMenuCancel, this, _1));
    m_optionDialog.getButton(IDC_OKBUTTON)->connectToClick(boost::bind(&Chess::onOptionMenuOK, this, _1));
}

//-----------------------------------------------------------------------------
// Name : initCreditsMenu
//-----------------------------------------------------------------------------
void Chess::initCreditsMenu()
{
    //m_creditsDialog.init(200, 320, 18, "Credits", "data/textures/woodBack.png" ,glm::vec4(1.0f,1.0f,1.0f, 0.8), m_asset);
    m_creditsDialog.init(200, 320, 18, "Credits", "data/textures/GUI/woodBack.png" ,glm::vec4(1.0f,1.0f,1.0f, 0.8), m_asset);
    m_creditsDialog.setClipboardFunctions(m_window->getCopyToClipboardFunc(), m_window->getPasteClipboardFunc());
    m_creditsDialog.setLocation( (m_window->getWidth() / 2) - (m_creditsDialog.getWidth() / 2), (m_window->getHeight() / 2) - (m_creditsDialog.getHeight() / 2) );
    m_creditsDialog.initWoodControlElements(m_asset);
    m_creditsDialog.LoadDialogFromFile("data/dialogs/credits.txt");
    m_creditsDialog.getButton(IDC_CREDITSOK)->connectToClick( boost::bind(&Chess::onCreditsOkClicked, this, _1) );
    m_creditsDialog.setVisible(false);
    m_creditsDialog.setCaption(false);
}

//-----------------------------------------------------------------------------
// Name : initPromotionMenu
//-----------------------------------------------------------------------------
void Chess::initPromotionMenu()
{
    m_promotionGUI.init(500, 100, 18,"Select Pawn", "data/textures/GUI/woodBack.png", glm::vec4(1.0f, 1.0f, 1.0f, 0.8), m_asset);
    m_promotionGUI.setClipboardFunctions(m_window->getCopyToClipboardFunc(), m_window->getPasteClipboardFunc());
    m_promotionGUI.setCaption(false);
    m_promotionGUI.initWoodControlElements(m_asset);
    if(!m_promotionGUI.LoadDialogFromFile("data/dialogs/pawns.txt"))
        return;
    
    m_promotionGUI.setLocation( (m_window->getWidth() / 2) - (m_promotionGUI.getWidth() / 2), (m_window->getHeight() / 2) - (m_promotionGUI.getHeight() / 2) );
    m_promotionGUI.setVisible(false);
    GLuint promotionTextureName = m_asset.getTexture("data/textures/GUI/pawnsButtons.png");
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
// Name : initKeysMenu
//-----------------------------------------------------------------------------
void Chess::initKeysMenu()
{
    m_keysDialog.init(100, 100, 18, "Options", "", glm::vec4(1.0f,1.0f,1.0f, 0.0), m_asset);
    m_keysDialog.initWoodControlElements(m_asset);
    m_keysDialog.setLocation( m_window->getWidth() - 400,  (m_window->getHeight() / 2) - (m_keysDialog.getHeight() / 2) - 130 );
    m_keysDialog.LoadDialogFromFile("data/dialogs/keys.txt");
    m_keysDialog.setVisible(true);
    m_keysDialog.setCaption(false);
}

//-----------------------------------------------------------------------------
// Name : initGameOverMenu
//-----------------------------------------------------------------------------
void Chess::initGameOverMenu()
{
    m_gameOverDialog.init(200,320, 18,"Game Over", "data/textures/GUI/woodBack.png", glm::vec4(1.0f, 1.0f, 1.0f ,0.8f), m_asset);
    m_gameOverDialog.initWoodControlElements(m_asset);
    m_gameOverDialog.LoadDialogFromFile("data/dialogs/gameover.txt");
    m_gameOverDialog.setVisible(false);
    m_gameOverDialog.setCaption(false);
    m_gameOverDialog.setLocation( (m_window->getWidth() / 2) - (m_gameOverDialog.getWidth() / 2), (m_window->getHeight() / 2) - (m_gameOverDialog.getHeight() / 2) );

    m_gameOverDialog.getButton(IDC_MainMenu)->connectToClick( boost::bind(&Chess::onMainMenuClicked, this, _1) );
}

//-----------------------------------------------------------------------------
// Name : renderGUI
//-----------------------------------------------------------------------------
void Chess::renderGUI()
{
      m_mainMenuDialog.OnRender(m_sprites, m_topSprites, m_asset, m_timer.getCurrentTime());
      m_optionDialog.OnRender(m_sprites, m_topSprites, m_asset, m_timer.getCurrentTime());
      m_creditsDialog.OnRender(m_sprites, m_topSprites, m_asset, m_timer.getCurrentTime());
      m_promotionGUI.OnRender(m_sprites, m_topSprites, m_asset, m_timer.getCurrentTime());
      m_keysDialog.OnRender(m_sprites, m_topSprites, m_asset, m_timer.getCurrentTime());
      m_gameOverDialog.OnRender(m_sprites, m_topSprites, m_asset, m_timer.getCurrentTime());
}

//-----------------------------------------------------------------------------
// Name : sendKeyEvent
//-----------------------------------------------------------------------------
void Chess::sendKeyEvent(unsigned char key, bool down)
{
    if (key == 27 && down)
    {
        if (!m_optionDialog.getVisible() && !m_promotionGUI.getVisible())
        {
            m_mainMenuDialog.setVisible(!m_mainMenuDialog.getVisible());
            m_keysDialog.setVisible(!m_keysDialog.getVisible());
            if (m_mainMenuDialog.getVisible())
                static_cast<ChessScene*>(m_scene)->setCameraRotationMode(ChessScene::RotationMode::Infinite);
            else
                static_cast<ChessScene*>(m_scene)->setCameraRotationMode(ChessScene::RotationMode::ReturnToWhite);
            m_sceneInput = !m_mainMenuDialog.getVisible();
        }
    }
        
    m_mainMenuDialog.handleKeyEvent(key, down);
    m_optionDialog.handleKeyEvent(key, down);
    m_creditsDialog.handleKeyEvent(key, down);
    m_promotionGUI.handleKeyEvent(key, down);
    m_keysDialog.handleKeyEvent(key, down);
    m_gameOverDialog.handleKeyEvent(key, down);
}

//-----------------------------------------------------------------------------
// Name : sendVirtualKeyEvent
//-----------------------------------------------------------------------------
void Chess::sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates)
{
    m_mainMenuDialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
    m_optionDialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
    m_creditsDialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
    m_promotionGUI.handleVirtualKeyEvent(virtualKey, down, modifierStates);
    m_keysDialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
    m_gameOverDialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
}

//-----------------------------------------------------------------------------
// Name : sendMouseEvent
//-----------------------------------------------------------------------------
void Chess::sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    BaseGame::sendMouseEvent(event, modifierStates);
    m_mainMenuDialog.handleMouseEvent(event, modifierStates);
    m_optionDialog.handleMouseEvent(event, modifierStates);
    m_creditsDialog.handleMouseEvent(event, modifierStates);
    m_promotionGUI.handleMouseEvent(event, modifierStates);
    m_keysDialog.handleMouseEvent(event, modifierStates);
    m_gameOverDialog.handleMouseEvent(event, modifierStates);
}

//-----------------------------------------------------------------------------
// Name : onSizeChanged
//-----------------------------------------------------------------------------
void Chess::onSizeChanged()
{
    m_mainMenuDialog.setLocation( (m_window->getWidth() / 2) - (m_mainMenuDialog.getWidth() / 2), (m_window->getHeight() / 2) - (m_mainMenuDialog.getHeight() / 2) );
    m_promotionGUI.setLocation( (m_window->getWidth() / 2) - (m_promotionGUI.getWidth() / 2), (m_window->getHeight() / 2) - (m_promotionGUI.getHeight() / 2) );
    m_optionDialog.setLocation( (m_window->getWidth() / 2) - (m_optionDialog.getWidth() / 2), (m_window->getHeight() / 2) - (m_optionDialog.getHeight() / 2) );
    m_creditsDialog.setLocation( (m_window->getWidth() / 2) - (m_creditsDialog.getWidth() / 2), (m_window->getHeight() / 2) - (m_creditsDialog.getHeight() / 2) );
    m_keysDialog.setLocation( m_window->getWidth() - 400,  (m_window->getHeight() / 2) - (m_keysDialog.getHeight() / 2) - 130 );
    m_gameOverDialog.setLocation( (m_window->getWidth() / 2) - (m_gameOverDialog.getWidth() / 2), (m_window->getHeight() / 2) - (m_gameOverDialog.getHeight() / 2) );
}

//-----------------------------------------------------------------------------
// Name : onNewGame
//-----------------------------------------------------------------------------
void Chess::onNewGame(ButtonUI* newGameButton)
{
    m_mainMenuDialog.setVisible(false);
    m_keysDialog.setVisible(false);
    m_sceneInput = true;
    static_cast<ChessScene*>(m_scene)->newGame();
    static_cast<ChessScene*>(m_scene)->setCameraRotationMode(ChessScene::RotationMode::ReturnToWhite);
}

//-----------------------------------------------------------------------------
// Name : onContinueGame
//-----------------------------------------------------------------------------
void Chess::onContinueGame(ButtonUI* continuButton)
{
    m_mainMenuDialog.setVisible(false);
    m_keysDialog.setVisible(false);
    m_sceneInput = true;
    static_cast<ChessScene*>(m_scene)->loadGame();
    static_cast<ChessScene*>(m_scene)->setCameraRotationMode(ChessScene::RotationMode::ReturnToWhite);
}

//-----------------------------------------------------------------------------
// Name : onOptions
//-----------------------------------------------------------------------------
void Chess::onOptions(ButtonUI* optionsButton)
{
    m_mainMenuDialog.setVisible(false);
    m_keysDialog.setVisible(false);
    m_optionDialog.setVisible(true);
}

//-----------------------------------------------------------------------------
// Name : onCredits
//-----------------------------------------------------------------------------
void Chess::onCredits(ButtonUI* creditsButton)
{
    m_creditsDialog.setVisible(true);
    m_mainMenuDialog.setVisible(false);
    m_keysDialog.setVisible(false);
}

//-----------------------------------------------------------------------------
// Name : onExitPressed
//-----------------------------------------------------------------------------
void Chess::onExitPressed(ButtonUI* exitButton)
{
    m_gameRunning = false;
}

//-----------------------------------------------------------------------------
// Name : onOptionMenuCancel
//-----------------------------------------------------------------------------
void Chess::onOptionMenuCancel(ButtonUI* cancelButton)
{
    m_optionDialog.setVisible(false);
    m_mainMenuDialog.setVisible(true);
    m_keysDialog.setVisible(true);
}

//-----------------------------------------------------------------------------
// Name : onOptionMenuOK
//-----------------------------------------------------------------------------
void Chess::onOptionMenuOK(ButtonUI* okButton)
{
    m_optionDialog.setVisible(false);
    m_mainMenuDialog.setVisible(true);
    m_keysDialog.setVisible(true);
    
    bool fullscreen  = m_optionDialog.getRadioButton(IDC_FULLSCREENRADIO)->getChecked();
    int monitorIndex = *(m_optionDialog.getComboBox<int>(IDC_MONITOR)->GetSelectedData());
    
    m_window->setFullScreenMode(false);
    m_window->moveWindowToMonitor(monitorIndex);
    
    if (fullscreen)
    {
        Resolution* newRes = m_optionDialog.getComboBox<Resolution>(IDC_RESOLUTIONCOM)->GetSelectedData();
        m_window->setMonitorResolution(monitorIndex, *newRes);
        m_window->setFullScreenMode(true);
    }
    
    int rotationEnabled = *(m_optionDialog.getComboBox<int>(IDC_ROTATIONCOM)->GetSelectedData());
    if (rotationEnabled == 1)
        static_cast<ChessScene*>(m_scene)->setCameraRotaion(true);
    else
        static_cast<ChessScene*>(m_scene)->setCameraRotaion(false);
    
}

//-----------------------------------------------------------------------------
// Name : onCreditsOkClicked
//-----------------------------------------------------------------------------
void Chess::onCreditsOkClicked(ButtonUI* okbutton)
{
    m_creditsDialog.setVisible(false);
    m_mainMenuDialog.setVisible(true);
    m_keysDialog.setVisible(true);
}

//-----------------------------------------------------------------------------
// Name : onMainMenuClicked
//-----------------------------------------------------------------------------
void Chess::onMainMenuClicked(ButtonUI* mainMenuButton)
{
    m_gameOverDialog.setVisible(false);
    m_mainMenuDialog.setVisible(true);
    m_keysDialog.setVisible(true);
    static_cast<ChessScene*>(m_scene)->setCameraRotationMode(ChessScene::RotationMode::Infinite);
}
