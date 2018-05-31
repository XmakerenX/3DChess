#ifndef  _LISTBOXUI_H
#define  _LISTBOXUI_H

#include "ControlUI.h"
#include "ScrollBarUI.h"

//-------------------------------------------------------------------------
// Enumerators and Structures
//-------------------------------------------------------------------------
struct ListBoxItemUI
{
    std::string strText;
	void* pData;

    Rect rcActive;
	bool bSelected;
};

class ListBoxUI : public ControlUI
{
public:

    typedef boost::signals2::signal<void (ListBoxUI*)>  signal_listbox;
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
    ListBoxUI					(DialogUI* pParentDialog, int ID, int x, int y, int width, int height, GLuint dwStyle);
    ListBoxUI					(std::istream& inputFile);

    virtual ~ListBoxUI			(void);

	virtual bool onInit();

	//-------------------------------------------------------------------------
	// functions that handle user Input
	//-------------------------------------------------------------------------
    virtual bool    handleMouseEvent    (MouseEvent event);

    virtual bool	Pressed				( Point pt, INPUT_STATE inputState, double timeStamp);
    virtual bool    Released			( Point pt);
    virtual bool    Scrolled			( int nScrollAmount);
    virtual bool    Dragged				( Point pt);


	void		    ConnectToItemDBLCK	( const signal_listbox::slot_type& subscriber);
	void            ConnectToListboxSel ( const signal_listbox::slot_type& subscriber);

    virtual void    Render              (Sprite& sprite, Sprite& textSprite, double timeStamp);
	virtual void    UpdateRects			();

	virtual bool    CanHaveFocus		();

	virtual bool	SaveToFile			(std::ostream& SaveFile);
    void			CopyItemsFrom		(ListBoxUI* sourceListBox);

    GLuint          GetStyle			() const;
    void            SetStyle			(GLuint dwStyle );
	int             GetSize				() const;

	int             GetScrollBarWidth	() const;
	void            SetScrollBarWidth	( int nWidth );
	void            SetBorder			( int nBorder, int nMargin );

	//-------------------------------------------------------------------------
	// functions that ListBox actions
	//-------------------------------------------------------------------------
    bool            AddItem				(std::string strText, void* pData );
    bool            InsertItem			(int nIndex, std::string &strText, void* pData );
	void            RemoveItem			( int nIndex );
	void            RemoveItemByData	( void* pData );
	void            RemoveAllItems		();

	ListBoxItemUI*  GetItem				( int nIndex );
	int             GetSelectedIndex	( int nPreviousSelected = -1 );
	ListBoxItemUI*  GetSelectedItem		( int nPreviousSelected = -1 );
	void            SelectItem			( int nNewIndex );

	enum STYLE
	{
		MULTISELECTION = 1
	};

protected:
    Rect m_rcText;      // Text rendering bound
    Rect m_rcSelection; // Selection box bound

    ScrollBarUI m_ScrollBar;
	int m_nSBWidth; //Scrollbar width

	int m_nBorder;
	int m_nMargin;
	int m_nTextHeight;  // Height of a single line of text
    GLuint m_dwStyle;    // List box style
	int m_nSelected;    // Index of the selected item for single selection list box
	int m_nSelStart;    // Index of the item where selection starts (for handling multi-selection)
	bool m_bDrag;       // Whether the user is dragging the mouse to select

	std::vector<ListBoxItemUI*> m_Items;

    boost::signals2::signal<void (ListBoxUI*)> m_itemDBLCLKSig;
    boost::signals2::signal<void (ListBoxUI*)> m_listboxSelSig;
};

#endif  //_LISTBOXUI_H
