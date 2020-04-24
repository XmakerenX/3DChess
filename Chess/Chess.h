/* * 3D Chess - A cross platform Chess game for 2 players made using OpenGL and c++
 * Copyright (C) 2016-2020 Matan Keren <xmakerenx@gmail.com>
 *
 * This file is part of 3D Chess.
 *
 * 3D Chess is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 3D Chess is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 3D Chess.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef  _Chess_H
#define  _Chess_H

#include <BaseGame.h>
#include "OptionDialog.h"
#include "ChessScene.h"

class Chess : public BaseGame
{
public:
    Chess();
    ~Chess();
        
protected:
    virtual void initGUI();
    void initMainMenu();
    void initOptionsMenu();
    void initCreditsMenu();
    void initPromotionMenu();
    void initKeysMenu();
    void initGameOverMenu();
    
    virtual void renderGUI();
    
    virtual void sendKeyEvent(unsigned char key, bool down);
    virtual void sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates);
    virtual void sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual void onSizeChanged();
        
    void onNewGame(ButtonUI* newGameButton);
    void onContinueGame(ButtonUI* continuButton);
    void onOptions(ButtonUI* optionsButton);
    void onCredits(ButtonUI* creditsButton);
    void onExitPressed(ButtonUI* exitButton);
    
    void onOptionMenuCancel(ButtonUI* cancelButton);
    void onOptionMenuOK(ButtonUI* okButton);
    
    void onCreditsOkClicked(ButtonUI* okbutton);
    void onMainMenuClicked(ButtonUI* mainMenuButton);
    
    DialogUI m_mainMenuDialog;
    OptionDialog m_optionDialog;
    DialogUI m_creditsDialog;
    DialogUI m_promotionGUI;
    DialogUI m_keysDialog;
    DialogUI m_gameOverDialog;
};

#endif  //_Chess_H
