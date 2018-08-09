#ifndef  _GUIEDITORWIN_H
#define  _GUIEDITORWIN_H

#include "../GameWin.h"
#include "EditorDialog.h"

class GUIEditorWin : public GameWin
{
public:
    GUIEditorWin();
protected:
    virtual void initGUI();
    virtual void renderGUI();
    
    virtual void sendKeyEvent(unsigned char key, bool down);
    virtual void sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates);
    virtual void sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);  
    
    EditDialogUI m_editDialog;
};

#endif  //_GUIEDITORWIN_H
