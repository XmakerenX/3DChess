#ifndef  _ITEMBOXUI_H
#define  _ITEMBOXUI_H

#include "ControlUI.h"
#include "ScrollBarUI.h"

//-------------------------------------------------------------------------
//structs for This Class.
//-------------------------------------------------------------------------
template<class T>
struct Item
{
    Item(std::string text, T& newData) : strText(text), data(newData)
    {
        bVisible = false;
        bSelected = false;
    }

    Item(std::string text, T&& newData) : strText(text), data(newData)
    {
        bVisible = false;
        bSelected = false;
    }

    std::string strText;
    T data;

    Rect rcActive;
    bool bVisible;
    bool bSelected;
};

template<class T>
class ListBoxUI : public ControlUI
{
public:
    //-------------------------------------------------------------------------
    // Constructors & Destructors for This Class.
    //-------------------------------------------------------------------------
    ListBoxUI(DialogUI *pParentDialog, int ID, int x, int y, int width, int height, bool multiSelection);
    ListBoxUI(std::istream& inputFile);
    virtual ~ListBoxUI();

    virtual bool onInit();
    //-------------------------------------------------------------------------
    //functions that handle user Input to this control
    //-------------------------------------------------------------------------
    virtual bool    handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);

    virtual bool    Pressed	    (Point pt, const ModifierKeysStates &modifierStates, double timeStamp);
    virtual bool    Scrolled	(int nScrollAmount);
            bool    Highlight   (Point mousePoint);

    //-------------------------------------------------------------------------
    //functions that handle control Rendering
    //-------------------------------------------------------------------------
    virtual void    Render           (Sprite& sprite, Sprite& textSprite, double timeStamp);
    virtual void    UpdateRects	     ();

    virtual bool    CanHaveFocus     ();

    virtual bool    SaveToFile	     (std::ostream& SaveFile);
            void    CopyItemsFrom    (ListBoxUI& sourceItems);

    //-------------------------------------------------------------------------
    //functions that handle the items in the itembox
    //-------------------------------------------------------------------------
    bool            AddItem             (std::string strText, T& data);
    bool            AddItem             (std::string strText, T&& data);
    bool            InsertItem      	(int nIndex, std::string &strText, T& data );
    bool            InsertItem      	(int nIndex, std::string &strText, T&& data );
    void            RemoveItem          (GLuint index);
    void            RemoveItemByData	(T& data );
    void            RemoveAllItems      ();
    int             FindItem            (std::string strText, GLuint iStart = 0);
    bool            ContainsItem        (std::string strText, GLuint iStart = 0);
    Item<T>*        GetItem             (GLuint nIndex);
    T*              GetItemData         (std::string strText);
    T*              GetItemData         (int nIndex);
    const std::vector<int>& GetSelectedIndices() const;

    bool SelectItem(GLuint index, bool select);
    bool SelectItem(std::string strText, bool select);

    GLuint          GetNumItems         ();
    void            ShowItem            (int nIndex);

    int             GetScrollBarWidth   () const;
    void            SetScrollBarWidth   (int nWidth);

private:
    enum ELEMENTS {MAIN, SELECTION};

    bool m_isMultiSelection;
    std::vector<Item<T>> m_Items;
    int m_nSelected;    // the index from which shift drag will start
    std::vector<int> m_selectedItems;

    Rect m_rcItembox;
    Rect m_rcItemboxText;

    ScrollBarUI m_ScrollBar;

};

#endif  //_ITEMBOXUI_H
