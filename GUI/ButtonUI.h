#ifndef  _BUTTONUI_H
#define  _BUTTONUI_H

#include "../RenderTypes.h"
#include "../AssetManager.h"
#include "../timer.h"
#include "StaticUI.h"

class ButtonUI : public StaticUI
{
public:
    typedef boost::signals2::signal<void (ButtonUI*)>  signal_clicked;

                    ButtonUI            (DialogUI* pParentDialog, int ID, const std::string& strText, int x, int y, GLuint width, GLuint height, GLuint nHotkey);
                    ButtonUI            (std::istream& inputFile);
    virtual         ~ButtonUI           (void);

    virtual void    Render              (Sprite sprites[SPRITES_SIZE], Sprite topSprites[SPRITES_SIZE], double timeStamp);

    virtual bool    handleKeyEvent      (unsigned char key, bool down);
    virtual bool    handleMouseEvent    (MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual bool    Pressed             (Point pt, const ModifierKeysStates &modifierStates, double timeStamp);
    virtual bool    Released            (Point pt);

    virtual bool    CanHaveFocus        ();
            void    setHotKey           (GLuint nHotKey);

    virtual bool    SaveToFile          (std::ostream& SaveFile);

            void    connectToClick      (const signal_clicked::slot_type& subscriber);



protected:
    GLuint	    m_nHotkey;
    bool	    m_bPressed;

    boost::signals2::signal<void (ButtonUI*)> m_clickedSig;

private:
    enum ELEMENTS{BUTTON, MOUSEOVER};
};

#endif  //_BUTTONUI_H
