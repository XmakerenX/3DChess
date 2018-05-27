#ifndef  _RADIOBUTTONUI_H
#define  _RADIOBUTTONUI_H

#include "CheckboxUI.h"

class RadioButtonUI : public CheckboxUI
{
public:
    RadioButtonUI		(DialogUI* pParentDialog, int ID, int x, int y, GLuint width, GLuint height, GLuint nHotkey, GLuint nButtonGruop);
    RadioButtonUI		(std::istream& inputFile);
    virtual ~RadioButtonUI(void);

    virtual bool    handleMouseEvent    (MouseEvent event);

    virtual bool    Pressed             (Point pt, INPUT_STATE inputState, double timeStamp);
    virtual bool    Released            (Point pt);

    virtual bool	SaveToFile          (std::ostream& SaveFile);

    GLuint getButtonGroup();

    void setChecked(bool bChecked);
    void setButtonGroup(GLuint buttonGroup);

protected:
    GLuint m_nButtonGroup;
};

#endif  //_RADIOBUTTONUI_H

 
