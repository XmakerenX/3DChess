#ifndef  _COMBOBOXUI_H
#define  _COMBOBOXUI_H

#include "ButtonUI.h"
#include "ScrollBarUI.h"
#include <string>

//-------------------------------------------------------------------------
//structs for This Class.
//-------------------------------------------------------------------------
struct ComboBoxItem
{
    ComboBoxItem(std::string text) : strText(text)
    {
        pData = nullptr;
        bVisible = false;
    }

    std::string strText;
    //char strText[256];
    void* pData;

    Rect rcActive;
    bool bVisible;
};

class ComboBoxUI : public ButtonUI
{
public:
    typedef boost::signals2::signal<void (ComboBoxUI*)>  signal_comboBox;

    //-------------------------------------------------------------------------
    // Constructors & Destructors for This Class.
    //-------------------------------------------------------------------------
    ComboBoxUI(DialogUI* pParentDialog, int ID, std::string strText, int x, int y, GLuint width, GLuint height, GLuint nHotkey);
    ComboBoxUI(std::istream& inputFile);
    virtual         ~ComboBoxUI();

    virtual bool onInit();

    //-------------------------------------------------------------------------
    //functions that handle user Input to this control
    //-------------------------------------------------------------------------
    virtual bool    handleMouseEvent    (MouseEvent event);
    virtual void    OnHotkey();

    virtual bool	Pressed				(Point pt, INPUT_STATE inputState, double timeStamp);
    virtual bool    Released			( Point pt);
    virtual bool    Scrolled			( int nScrollAmount);
    bool			Highlight			( Point mousePoint);

    void			ConnectToSelectChg  ( const signal_comboBox::slot_type& subscriber);

    //-------------------------------------------------------------------------
    //functions that handle control Rendering
    //-------------------------------------------------------------------------
    virtual void    Render              (Sprite& sprite, Sprite& textSprite, double timeStamp);
    virtual void    UpdateRects			();

    virtual bool    CanHaveFocus		();
    virtual void    OnFocusOut			();

    virtual bool	SaveToFile			(std::ostream& SaveFile);
    void			CopyItemsFrom		(ComboBoxUI* sourceComboBox);

    //-------------------------------------------------------------------------
    //functions that handle checkBox specific properties
    //-------------------------------------------------------------------------
    bool            AddItem(std::string strText, void* pData );
    void            RemoveItem(GLuint index );
    void            RemoveAllItems();
    int             FindItem(std::string strText, GLuint iStart = 0 );
    bool            ContainsItem(std::string strText, GLuint iStart = 0 );
    void    *   	GetItemData(std::string strText );
    void    *		GetItemData( int nIndex );

    void            SetDropHeight(GLuint nHeight );

    int             GetScrollBarWidth() const;
    void            SetScrollBarWidth( int nWidth );

    int             GetSelectedIndex() const;

    void*			GetSelectedData();
    ComboBoxItem*	GetSelectedItem();

    GLuint GetNumItems();
    ComboBoxItem*	GetItem(GLuint index );

    bool SetSelectedByIndex(GLuint index );
    bool SetSelectedByText(std::string strText );
    bool SetSelectedByData( void* pData );

protected:

    int m_iSelected;
    int m_iFocused;
    int m_nDropHeight;
    ScrollBarUI m_ScrollBar;
    int m_ScrollBarWidth;

    GLuint m_nFontHeight;

    bool m_bOpened;

    Rect m_rcText;
    Rect m_rcButton;
    Rect m_rcDropdown;
    Rect m_rcDropdownText;


    std::vector<ComboBoxItem*> m_Items;

private:
    // the elements used to render the Combobox
    // used to access m_elementsGFX vector and no other use
    enum ELEMENTS {MAIN, BUTTON, DROPDOWN, SELECTION };

    boost::signals2::signal<void (ComboBoxUI*)> m_selectionChangedSig;
};

#endif  //_CCOMBOBOXUI_H
