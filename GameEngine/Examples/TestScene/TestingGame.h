#ifndef  _TestingGame_H
#define  _TestingGame_H

#include <BaseGame.h>
#include <Render/GUI/DialogUI.h>

class TestingGame : public BaseGame
{
public:
    TestingGame();
protected:
    virtual void initGUI();
    virtual void renderGUI();
    
    virtual void sendKeyEvent(unsigned char key, bool down);
    virtual void sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates);
    virtual void sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);  
    
    DialogUI m_testDialog;
    
};

#endif  //_TestingGame_H
