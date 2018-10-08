#ifndef  _CHESSWINDOW_H
#define  _CHESSWINDOW_H

#include "../GameWin.h"
#include "OptionDialog.h"
#include "ChessScene.h"

class ChessWindow : public GameWin
{
public:
    ChessWindow();
    ~ChessWindow();
        
protected:
    virtual void initGUI();
    void initMainMenu();
    void initOptionsMenu();
    void initPromotionMenu();
    
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
    
    DialogUI m_dialog;
    OptionDialog m_optionDialog;
    DialogUI m_promotionGUI;
};

#endif  //_CHESSWINDOW_H
