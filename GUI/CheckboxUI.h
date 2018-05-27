#ifndef  _CHECKBOXUI_H
#define  _CHECKBOXUI_H

#include "ButtonUI.h"

class CheckboxUI : public ButtonUI
{
public:
    CheckboxUI					(DialogUI* pParentDialog, int ID, int x, int y, GLuint width, GLuint height, GLuint nHotkey);
    CheckboxUI					(std::istream& inputFile);
    virtual ~CheckboxUI				(void);

    virtual void Render				(Sprite &sprite, Sprite &textSprite, double timeStamp);

    //virtual bool    handleKeyEvent		(unsigned char key, bool down);
    virtual bool    handleMouseEvent		(MouseEvent event);

    virtual bool    Pressed			(Point pt, INPUT_STATE inputState, double timeStamp);
    virtual bool    Released			(Point pt);

    virtual bool    SaveToFile			(std::ostream& SaveFile);

    bool	    getChecked			();

protected:
    bool m_bChecked;

private:
    enum ELEMENTS{BUTTON, MOUSEOVER};
};

#endif  //_CHECKBOXUI_H
