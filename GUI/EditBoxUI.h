#ifndef  _EDITBOXUI_H
#define  _EDITBOXUI_H

#include <string>
#include <assert.h>
#include <ostream>
#include "ControlUI.h"

//std::ostream& operator<<(std::ostream& os, const glm::vec4 v)
//{
//    os << v.x << " " << v.y << " " << v.z << " " << v.w;
//    return os;
//}

//std::istream& operator>>(std::istream& is, glm::vec4 v)
//{
//    float x,y,z,w;
//    is >> x;
//    is >> y;
//    is >> z;
//    is >> w;
//    v = glm::vec4(x,y,z,w);
//    return is;
//}

class EditBoxUI : public ControlUI
{
public:
    typedef boost::signals2::signal<void (EditBoxUI*)>  signal_editbox;
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
    EditBoxUI(DialogUI *pParentDialog, int ID, std::string strText, int x, int y, int width, int height);
    EditBoxUI(std::istream& inputFile);
    virtual ~EditBoxUI(void);

	//-------------------------------------------------------------------------
	// Functions that handle user input logic
	//-------------------------------------------------------------------------
    virtual bool    handleKeyEvent		(unsigned char key , bool down);
    virtual bool    handleVirtualKey	(GK_VirtualKey virtualKey , bool down, const ModifierKeysStates &modifierStates);
    virtual bool    handleMouseEvent    (MouseEvent event);

    virtual bool	Pressed				(Point pt, INPUT_STATE inputState, double timeStamp);
    virtual bool    Released			( Point pt);
    virtual bool    Dragged				(Point pt);

	void		    connectToEditboxChg	( const signal_editbox::slot_type& subscriber);

    virtual bool    MsgProc				();

	//-------------------------------------------------------------------------
	// Functions that handle Rendering Logic
	//-------------------------------------------------------------------------
	virtual bool    CanHaveFocus();
	virtual void    OnFocusIn();
	virtual void    OnFocusOut();

	virtual bool	SaveToFile(std::ostream& SaveFile);

	virtual void    UpdateRects();
    virtual void    Render(Sprite& sprite, Sprite& textSprite, double timeStamp);

	void            SetText(std::string strText, bool bSelected = false );
    std::string     GetText();
	int             GetTextLength();
    bool            GetTextCopy(std::string strDest, GLuint bufferCount );
	void            ClearText();

	virtual void    SetTextColor(glm::vec4 Color );
	void            SetSelectedTextColor(glm::vec4 Color );
	void            SetSelectedBackColor(glm::vec4 Color );

	void            SetCaretColor(glm::vec4 Color );
	void            SetBorderWidth( int nBorder );
	void            SetSpacing( int nSpacing );

	//void            ParseFloatArray( float* pNumbers, int nCount );
	void            SetTextFloatArray( const float* pNumbers, int nCount );

	int				CalcFirstVisibleCharUp();
	int				CalcFirstVisibleCharDown();

protected:
	int				CalcCaretPosByPoint(Point pt );
	void            PlaceCaret( int nCP );
	void            DeleteSelectionText();
	void            ResetCaretBlink();
	void            CopyToClipboard();
	void            PasteFromClipboard();

    std::string	 m_buffer;        // Buffer to hold text
	int			 m_nBorder;       // Border of the window
	int			 m_nSpacing;      // Spacing between the text and the edge of border
	int			 m_nVisibleChars; //
	int			 m_nBackwardChars;
    Rect		 m_rcText;        // Bounding rectangle for the text
    Rect		 m_rcRender[9];   // Convenient rectangles for rendering elements
	double		 m_dfBlink;       // Caret blink time in milliseconds
	double		 m_dfLastBlink;   // Last timestamp of caret blink
	bool		 m_bCaretOn;      // Flag to indicate whether caret is currently visible
	int			 m_nCaret;        // Caret position, in characters
	bool		 m_bInsertMode;   // If true, control is in insert mode. Else, overwrite mode.
	int			 m_nSelStart;     // Starting position of the selection. The caret marks the end.
	int			 m_nFirstVisible; // First visible character in the edit control
    glm::vec4	 m_TextColor;     // Text color
    glm::vec4	 m_SelTextColor;  // Selected text color
    glm::vec4	 m_SelBkColor;    // Selected background color
    glm::vec4	 m_CaretColor;    // Caret color

    boost::signals2::signal<void (EditBoxUI*)> m_editboxChangedSig;

    //CAssetManager* m_assetManger;

	// Mouse-specific
	bool m_bMouseDrag;       // True to indicate drag in progress

	// Static
	static bool s_bHideCaret;   // If true, we don't render the caret.s
    static int s_caretBlinkTime;
};

#endif  //_CEDITBOXUI_H 
