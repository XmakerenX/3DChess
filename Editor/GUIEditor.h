#ifndef  _GUIEditor_H
#define  _GUIEditor_H

#include "../BaseGame.h"
#include "EditorDialog.h"

class GUIEditor : public BaseGame
{
public:
    GUIEditor();
protected:
    void initGUI();
    virtual void renderGUI();
    
    virtual void sendKeyEvent(unsigned char key, bool down);
    virtual void sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates);
    virtual void sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);  
    
    EditDialogUI m_editDialog;
};

#endif  //_GUIEditor_H
