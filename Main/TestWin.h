#ifndef  _TESTWIN_H
#define  _TESTWIN_H

#include "../GameWin.h"
#include "ChessScene.h"

class TestWin : public GameWin
{
public:
    TestWin();
    ~TestWin();
protected:
    virtual void initGUI();
    virtual void renderGUI();
    
    virtual void sendKeyEvent(unsigned char key, bool down);
    virtual void sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates);
    virtual void sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual void onSizeChanged();
    
    DialogUI m_dialog;
};

#endif  //_TESTWIN_H
