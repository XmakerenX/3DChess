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

    ButtonUI					(DialogUI* pParentDialog, int ID, const std::string& strText, int x, int y, GLuint width, GLuint height, GLuint nHotkey);
    ButtonUI					(std::istream& inputFile);
    virtual ~ButtonUI			(void);

    virtual void	Render				(Sprite &sprite, Sprite& textSprite , double timeStamp);

    virtual bool handleKeyEvent(unsigned char key, bool down);
    //virtual bool handleVirtualKey(GK_VirtualKey virtualKey, bool down);
    virtual bool handleMouseEvent(MouseEvent event);
    //virtual bool    HandleMouse			(GLuint uMsg, Point mousePoint, INPUT_STATE inputstate, Timer* timer );

    virtual bool Pressed (Point pt, INPUT_STATE inputState, float timeStamp);
    virtual bool Released(Point pt);
    //virtual bool	Pressed				(HWND hWnd, Point pt, INPUT_STATE inputState, Timer *timer);
    //virtual bool    Released			( HWND hWnd, POINT pt);

    virtual bool    CanHaveFocus		();


    void	setHotKey					( GLuint nHotKey);

    void    connectToClick				( const signal_clicked::slot_type& subscriber);

    virtual bool SaveToFile				(std::ostream& SaveFile);

protected:
    GLuint	    m_nHotkey;

    bool	    m_bPressed;

    boost::signals2::signal<void (ButtonUI*)> m_clickedSig;

private:
    enum ELEMENTS{BUTTON, MOUSEOVER};
};

#endif  //_BUTTONUI_H
