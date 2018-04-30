#ifndef  _CONTROLUI_H
#define  _CONTROLUI_H

#include <fstream>
#include <boost/signals2/signal.hpp>
#include <boost/bind/bind.hpp>
#include "../RenderTypes.h"
#include "../timer.h"
#include "../AssetManager.h"
#include "../Sprite.h"
#include "../Font.h"

//-----------------------GLuint------------------------------------------------------
// Typedefs, Structures and Enumerators
//-----------------------------------------------------------------------------

#define EVENT_BUTTON_CLICKED                0x0101
#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0201
#define EVENT_RADIOBUTTON_CHANGED           0x0301
#define EVENT_CHECKBOX_CHANGED              0x0401
#define EVENT_SLIDER_VALUE_CHANGED          0x0501
#define EVENT_EDITBOX_STRING                0x0601
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes
// due to user input.
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in
// a single selection list box.
#define EVENT_LISTBOX_SELECTION             0x0702
#define EVENT_LISTBOX_SELECTION_END         0x0703

struct INPUT_STATE
{
    INPUT_STATE()
    {
        bDoubleClick = false;
        bShift = false;
        bCtrl = false;
        nScrollAmount = -1;
    }

    INPUT_STATE(bool newDoubleClick, bool newShift, bool newCtrl, int newScrollAmount)
    {
        bDoubleClick = newDoubleClick;
        bShift = newShift;
        bCtrl = newCtrl;
        nScrollAmount = newScrollAmount;
    }

    bool bDoubleClick;
    bool bShift;
    bool bCtrl;
    int  nScrollAmount;
};

struct ELEMENT_FONT
{
    ELEMENT_FONT()
    {
        font = nullptr;
    }

    ELEMENT_FONT(FontInfo newFontInfo, mkFont* newFont)
        :fontInfo(newFontInfo)
    {
        font = newFont;
    }

    void setFont(FontInfo& newFontInfo, mkFont* newFont)
    {
        fontInfo = newFontInfo;
        font = newFont;
    }

    FontInfo fontInfo;
    mkFont* font;
};

struct ELEMENT_GFX
{
    ELEMENT_GFX()
    {
        iTexture = 0;
    }

    ELEMENT_GFX(GLuint newITexture, Rect newRcTexture)
    {
        setGFX(newITexture, newRcTexture);
    }

    void setGFX(GLuint newITexture, Rect newRcTexture)
    {
        iTexture	   = newITexture;
        rcTexture	   = newRcTexture;
    }

    GLuint iTexture;
    Rect rcTexture;

};

//-----------------------------------------------------------------------------
// Classes forward declaration
//-----------------------------------------------------------------------------
class DialogUI;

//-----------------------------------------------------------------------------
// CControlUI Class declaration
//-----------------------------------------------------------------------------
class ControlUI
{
public:
    //-----------------------------------------------------------------------------
    // Public Enumerators
    //-----------------------------------------------------------------------------
    enum CONTROLS{STATIC, BUTTON, CHECKBOX, RADIOBUTTON, COMBOBOX, EDITBOX, LISTBOX, SCROLLBAR, SLIDER};

    ControlUI(void);
    ControlUI(DialogUI* pParentDialog, int ID, int x, int y, GLuint width, GLuint height);
    ControlUI(std::istream& inputFile);

    virtual ~ControlUI(void);

    virtual bool onInit();

    //-------------------------------------------------------------------------
    // functions that handle user input
    //-------------------------------------------------------------------------
    //virtual bool    HandleKeyboard		( HWND hWnd, GLuint uMsg, WPARAM wParam, LPARAM lParam );
    //virtual bool	HandleMouse			( HWND hWnd, GLuint uMsg, Point mousePoint, INPUT_STATE inputstate, Timer* timer );

    //virtual bool	Pressed				( HWND hWnd, Point pt, INPUT_STATE inputState, Timer* timer);
    //virtual bool    Released			( HWND hWnd, Point pt);
    virtual bool    Scrolled			( int nScrollAmount);
    virtual bool    Dragged				( Point pt);

    //-------------------------------------------------------------------------
    // Windows message handler
    //-------------------------------------------------------------------------
    //virtual bool    MsgProc				(GLuint uMsg, WPARAM wParam, LPARAM lParam );

     //TODO: need to decide how time between frames will be given to the render function
    virtual void    Render	(Sprite& sprite, Sprite& textSprite ,AssetManager& assetManger) = 0; //pure abstract function

    void	renderRect		(Sprite& sprite, Rect &rcWindow, GLuint textureName, Rect &rcTexture, glm::vec4 color, Point offset);
    //void	RenderText		(const char strText[], Rect rcDest, LPD3DXFONT pFont, DWORD format, LPD3DXSPRITE pSprite, D3DCOLOR textColor, POINT offset);

    bool ContainsPoint(Point pt);

    virtual void onMouseEnter();
    virtual void onMouseLeave();

    virtual bool CanHaveFocus();
    virtual void OnFocusIn();
    virtual void OnFocusOut();

    void	SetID				(int ID);
    void	setLocation			(int x, int y);
    void	setSize				(GLuint width, GLuint height);
    void	setParent			(DialogUI* pParentDialog);
    void    setControlGFX		(std::vector<ELEMENT_GFX>& elementsGFX);
    void    setControlFonts     (std::vector<ELEMENT_FONT>& elementsFonts);
    void	setEnabled			(bool bEnabled);
    void    setVisible			(bool bVisible);

    int		getID				();
    GLuint  getType				();
    bool	getEnabled			();
    bool    getVisible			();
    int		getX				();
    int		getY				();
    int		getWidth			();
    int		getHeight			();
    DialogUI* getParentDialog();

    virtual bool	SaveToFile  (std::ostream& SaveFile);

    virtual void UpdateRects();
protected:

    DialogUI* m_pParentDialog;

    // Size, scale, and positioning members
    int m_x, m_y;
    int m_width, m_height;

    int m_ID;
    GLuint m_type;

    Rect m_rcBoundingBox;           // Rectangle defining the active region of the control

    bool m_bVisible;                // Shown/hidden flag
    bool m_bEnabled;                // Enabled/disabled flag

    bool m_bMouseOver;              // Mouse pointer is above control
    bool m_bHasFocus;               // Control has input focus;

    //CONTROL_GFX m_controlGfx;
    std::vector<ELEMENT_GFX>  m_elementsGFX;
    std::vector<ELEMENT_FONT> m_elementsFonts;
};

#endif  //_CONTROLUI_H

