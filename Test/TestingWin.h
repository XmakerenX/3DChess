#ifndef  _TESTINGWIN_H
#define  _TESTINGWIN_H

#include "../GameWin.h"
#include "../GUI/DialogUI.h"

class TestingWin : public GameWin
{
public:
    TestingWin();
protected:
    virtual void initGUI();
    virtual void renderGUI();
    
    virtual void sendKeyEvent(unsigned char key, bool down);
    virtual void sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates);
    virtual void sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);  
    
    DialogUI m_testDialog;
    
};

#endif  //_TESTINGWIN_H
