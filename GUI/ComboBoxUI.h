#ifndef  _COMBOBOXUI_H
#define  _COMBOBOXUI_H

#include "ButtonUI.h"
#include "ScrollBarUI.h"
#include "ListBoxUI.h"
#include <string>

template<class T>
class ComboBoxUI : public ButtonUI
{
public:
    typedef boost::signals2::signal<void (ComboBoxUI<T>*)>  signal_comboBox;

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
    virtual bool    handleMouseEvent    (MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual void    OnHotkey            ();

    virtual bool    Pressed             (Point pt, const ModifierKeysStates &modifierStates, double timeStamp);
    virtual bool    Released            (Point pt);
    virtual bool    Scrolled            (int nScrollAmount);

    void            ConnectToSelectChg  (const typename signal_comboBox::slot_type& subscriber);

    //-------------------------------------------------------------------------
    //functions that handle control Rendering
    //-------------------------------------------------------------------------
    virtual void    Render              (Sprite sprites[SPRITES_SIZE], Sprite topSprites[SPRITES_SIZE], double timeStamp);
    virtual void    UpdateRects         ();

    virtual bool    CanHaveFocus        ();
    virtual void    OnFocusOut          ();

    virtual bool    SaveToFile          (std::ostream& SaveFile);
            void    CopyItemsFrom       (ComboBoxUI<T>* sourceComboBox);


    virtual bool    ContainsPoint       (Point pt);

    virtual void    onMouseEnter        ();
    virtual void    onMouseLeave        ();

    //-------------------------------------------------------------------------
    //functions that handle checkBox specific properties
    //-------------------------------------------------------------------------
    bool            AddItem             (std::string strText, T data);
    void            RemoveItem          (GLuint index);
    void            RemoveAllItems      ();
    int             FindItem            (std::string strText, GLuint iStart = 0);
    bool            ContainsItem        (std::string strText, GLuint iStart = 0);
    void *          GetItemData         (std::string strText);
    void *          GetItemData         (int nIndex);

    void            SetDropHeight       (GLuint nHeight);

    int             GetScrollBarWidth   () const;
    void            SetScrollBarWidth   (int nWidth);

    int             GetSelectedIndex    () const;

    T *             GetSelectedData     ();
    Item<T> *       GetSelectedItem     ();

    GLuint          GetNumItems         ();
    Item<T>  *      GetItem             (GLuint index);

    bool            SetSelectedByIndex  (GLuint index);
    bool            SetSelectedByText   (std::string strText);
    bool            SetSelectedByData   (void* pData);

protected:
    int m_iFocused;
    bool m_bOpened;
    ListBoxUI<T> m_dropDown;

    Rect m_rcText;
    Rect m_rcButton;

private:
    // the elements used to render the Combobox
    // used to access m_elementsGFX vector and no other use
    enum ELEMENTS {MAIN, BUTTON};

   signal_comboBox m_selectionChangedSig;
};

#endif  //_COMBOBOXUI_H
