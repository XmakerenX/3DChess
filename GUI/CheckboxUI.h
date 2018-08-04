#ifndef  _CHECKBOXUI_H
#define  _CHECKBOXUI_H

#include "ButtonUI.h"

class CheckboxUI : public ButtonUI
{
public:
    CheckboxUI                  (DialogUI* pParentDialog, int ID, int x, int y, GLuint width, GLuint height, GLuint nHotkey);
    CheckboxUI                  (std::istream& inputFile);
    virtual ~CheckboxUI         (void);

    virtual void    Render              (Sprite &sprite, Sprite &textSprite, double timeStamp);

    virtual bool    handleMouseEvent    (MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual bool    Pressed             (Point pt, const ModifierKeysStates &modifierStates, double timeStamp);
    virtual bool    Released            (Point pt);

    virtual bool    SaveToFile          (std::ostream& SaveFile);

            bool    getChecked          ();

protected:
    bool m_bChecked;

private:
    enum ELEMENTS{BUTTON, MOUSEOVER};
};

#endif  //_CHECKBOXUI_H
