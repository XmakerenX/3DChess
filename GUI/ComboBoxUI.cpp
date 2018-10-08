#ifndef  _COMBOBOXUI_CPP
#define  _COMBOBOXUI_CPP

#include "ComboBoxUI.h"
#include "DialogUI.h"
#include "ListBoxUI.cpp"
#include <iostream>

//-----------------------------------------------------------------------------
// Name : ComboBoxUI(constructor)
//-----------------------------------------------------------------------------
template<class T>
ComboBoxUI<T>::ComboBoxUI(DialogUI* pParentDialog, int ID, std::string strText, int x, int y, GLuint width, GLuint height, GLuint nHotkey)
    :ButtonUI(pParentDialog, ID, strText, x, y, width, height, nHotkey),
    m_dropDown(pParentDialog, ID, x, y + height, width - height, 100, false)
{
    m_type = ControlUI::COMBOBOX;

    m_bOpened = false;
    m_dropDown.setVisible(m_bOpened);
    m_iFocused = -1;
}

//-----------------------------------------------------------------------------
// Name : ComboBoxUI(constructor from InputFile)
//-----------------------------------------------------------------------------
template<class T>
ComboBoxUI<T>::ComboBoxUI(std::istream& inputFile)
    :ButtonUI(inputFile), m_dropDown(inputFile)
{
    m_type = ControlUI::COMBOBOX;

    m_bOpened = false;
    m_dropDown.setVisible(m_bOpened);
    if (m_dropDown.GetNumItems() > 0)
    {
        m_iFocused = 0;
        m_dropDown.SelectItem(0, true);
    }
    else
        m_iFocused = -1;
}

//-----------------------------------------------------------------------------
// Name : CComboBoxUI(destructor)
//-----------------------------------------------------------------------------
template<class T>
ComboBoxUI<T>::~ComboBoxUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : onInit()
// Desc : initialize the scrollbar element
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::onInit()
{
    return m_pParentDialog->initControl(&m_dropDown);
}

//-----------------------------------------------------------------------------
// Name : setHotKey ()
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::OnHotkey()
{
    ;
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent()
// Desc : Handles mouse input for the Combo box
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    if (!m_bEnabled || !m_bVisible)
        return false;

    if (m_bOpened)
        if(m_dropDown.handleMouseEvent(event, ModifierKeysStates(false, false, false)))
            return true;

    switch(event.type)
    {
    case MouseEventType::MouseMoved:
    {
        if(m_bMouseOver && m_bOpened)
            if(m_dropDown.Highlight(event.cursorPos))
                return true;
    }break;

    case MouseEventType::LeftButton:
    case MouseEventType::DoubleLeftButton:
    {
        if (event.down)
        {
            if ( Pressed(event.cursorPos, modifierStates, event.timeStamp) )
                return true;
        }
        else
        {
            if ( Released(event.cursorPos) )
                return true;
        }
    }break;

    case MouseEventType::ScrollVert:
    {
        return Scrolled(event.nLinesToScroll);
    }break;

    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed()
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::Pressed(Point pt, const ModifierKeysStates &modifierStates, double timeStamp)
{
    if( ContainsPoint(pt))
    {
        // Pressed while inside the control
        m_bPressed = true;

        if( !m_bHasFocus )
            m_pParentDialog->RequestFocus(this);

        // Toggle dropdown
        if( m_bHasFocus )
        {
            m_bOpened = !m_bOpened;
            m_dropDown.setVisible(m_bOpened);
        }

        return true;
    }

    // Make sure the control is no longer in a pressed state
    m_bPressed = false;

    return false;
}

//-----------------------------------------------------------------------------
// Name : Released()
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::Released(Point pt)
{
    if (m_bOpened && m_dropDown.ContainsPoint(pt) && m_dropDown.GetSelectedIndices().size() > 0)
    {
        m_iFocused = m_dropDown.GetSelectedIndices().back();
        m_bOpened = false;
        m_dropDown.setVisible(m_bOpened);
        m_selectionChangedSig( this );
        return true;
    }
    else
        if (m_bOpened && !m_bPressed)
        {
            m_dropDown.setVisible(false);
            m_bOpened = false;
        }

    if( m_bPressed && ContainsPoint( pt ) )
    {
        // Button click
        m_bPressed = false;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Name : Scrolled()
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::Scrolled( int nScrollAmount)
{
    const std::vector<int>& selectedIndices =  m_dropDown.GetSelectedIndices();
    if (selectedIndices.size() > 0)
        m_iFocused = selectedIndices[0];
    else
        m_iFocused = 0;

    if (m_bMouseOver)
    {
        if( nScrollAmount > 0 )
        {
            if( m_iFocused > 0 )
            {
                m_dropDown.SelectItem(m_iFocused, false);
                m_iFocused--;
                m_dropDown.SelectItem(m_iFocused, true);

                if( !m_bOpened )
                    m_selectionChangedSig( this );
            }
        }
        else
        {
            if( m_iFocused + 1 < ( int )GetNumItems() )
            {
                m_dropDown.SelectItem(m_iFocused, false);
                m_iFocused++;
                m_dropDown.SelectItem(m_iFocused, true);

                if( !m_bOpened )
                    m_selectionChangedSig( this );
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : ConnectToSelectChg()
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::ConnectToSelectChg( const typename signal_comboBox::slot_type& subscriber)
{
    m_selectionChangedSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : Render()
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::Render(Sprite sprites[SPRITES_SIZE], Sprite topSprites[SPRITES_SIZE], double timeStamp)
{    
    // check that there is actual fonts
    if (!m_bVisible ||  m_elementsFonts.size() == 0)
        return;

    Point dialogPos = calcPositionOffset();

    //if Combobox is open render the scrollbar
    if (m_bOpened)
    {
        //m_dropDown.Render(topSprites, topSprites, timeStamp);
        m_dropDown.Render(topSprites, sprites, timeStamp);
    }

    // Render the main combobox elements
    glm::vec4 tintColor;
    if (!m_bEnabled)
        tintColor = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    else
        if (m_bPressed)
            tintColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
        else
            if (m_bMouseOver ||  m_bOpened)
                tintColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            else
                tintColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);

    renderRect(sprites[NORMAL], m_rcButton, m_elementsGFX[BUTTON].iTexture, m_elementsGFX[BUTTON].rcTexture, tintColor, dialogPos);
    renderRect(sprites[NORMAL], m_rcText, m_elementsGFX[MAIN].iTexture, m_elementsGFX[MAIN].rcTexture, tintColor, dialogPos);

    // Render combobox text
    const std::vector<int>& selectedIndices =  m_dropDown.GetSelectedIndices();
    if (selectedIndices.size() > 0)
    {
        glm::vec4 textColor;
        if (!m_bMouseOver && !m_bOpened)
            textColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        else
            textColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        if (m_iFocused != -1)
            renderText(sprites[TEXT], m_elementsFonts[0].font, m_dropDown.GetItem(m_iFocused)->strText
                    , textColor, m_rcText, dialogPos, mkFont::TextFormat::Center);
    }
}

//-----------------------------------------------------------------------------
// Name : UpdateRects()
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::UpdateRects()
{
    ButtonUI::UpdateRects();

    m_rcButton = m_rcBoundingBox;
    m_rcButton.left = m_rcButton.right - (m_rcButton.bottom - m_rcButton.top);

    m_rcText = m_rcBoundingBox;
    m_rcText.right = m_rcButton.left;

    m_dropDown.setLocation(m_x, m_rcBoundingBox.top + m_rcBoundingBox.getHeight());
    m_dropDown.setSize(m_rcText.getWidth(), m_dropDown.getHeight() );
}

//-----------------------------------------------------------------------------
// Name : AddItem()
// Desc : add an item to the combobox
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::AddItem( std::string strText, T data )
{
    bool ret = m_dropDown.AddItem(strText, data);
    if (m_dropDown.GetNumItems() == 1)
    {
        m_iFocused = 0;
        m_dropDown.SelectItem(0, true);
        m_selectionChangedSig(this);
    }

    return ret;
}

//-----------------------------------------------------------------------------
// Name : RemoveItem()
// Desc : remove an item at the given index
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::RemoveItem( GLuint index )
{
    m_dropDown.RemoveItem(index);
    if (m_dropDown.GetSelectedIndices().size() == 0)
        if (m_dropDown.GetNumItems() > 0)
        {
            m_dropDown.SelectItem(m_dropDown.GetNumItems() - 1, true);
            m_iFocused = m_dropDown.GetNumItems() - 1;
        }
}

//-----------------------------------------------------------------------------
// Name : RemoveAllItems()
// Desc : removes all the items from the combobox
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::RemoveAllItems()
{
    m_dropDown.RemoveAllItems();
    m_iFocused = -1;
}

//-----------------------------------------------------------------------------
// Name : FindItem()
// Desc : finds an item that match the given text and returns the item index
// Note : iStart can be given if there is need to start the check from a certain index
//-----------------------------------------------------------------------------
template<class T>
int ComboBoxUI<T>::FindItem( std::string strText, GLuint iStart/* = 0 */)
{
    return m_dropDown.FindItem(strText, iStart);
}

//-----------------------------------------------------------------------------
// Name : ContainsItem()
// Desc : check is there is a Item that match the given text
// Note : iStart can be given if there is need to start the check from a certain index
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::ContainsItem(std::string strText, GLuint iStart /*= 0 */)
{
    return m_dropDown.ContainsItem(strText, iStart);
}

//-----------------------------------------------------------------------------
// Name : GetItemData()
// Desc : returns the item data
// Note : the search for the item is being done based on the item text
//-----------------------------------------------------------------------------
template<class T>
void* ComboBoxUI<T>::GetItemData( std::string strText )
{
    return (void*)m_dropDown.GetItemData(strText);
}

//-----------------------------------------------------------------------------
// Name : GetItemData()
// Desc : returns the item data
// Note : the search for the item is being done based on the item index
//-----------------------------------------------------------------------------
template<class T>
void* ComboBoxUI<T>::GetItemData( int nIndex )
{
    return m_dropDown.GetItemData(nIndex);
}

//-----------------------------------------------------------------------------
// Name : SetDropHeight()
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::SetDropHeight( GLuint nHeight )
{
    m_dropDown.setSize(m_dropDown.getWidth(), nHeight);
}

//-----------------------------------------------------------------------------
// Name : GetScrollBarWidth()
//-----------------------------------------------------------------------------
template<class T>
int ComboBoxUI<T>::GetScrollBarWidth() const
{
    return m_dropDown.GetScrollBarWidth();
}

//-----------------------------------------------------------------------------
// Name : SetScrollBarWidth()
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::SetScrollBarWidth( int nWidth )
{
    m_dropDown.SetScrollBarWidth(nWidth);
    UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : GetSelectedIndex()
//-----------------------------------------------------------------------------
template<class T>
int ComboBoxUI<T>::GetSelectedIndex() const
{
    const std::vector<int> selIndices = m_dropDown.GetSelectedIndices();
    if (selIndices.size() > 0)
        return selIndices.back();
    else
        return -1;
}

//-----------------------------------------------------------------------------
// Name : GetSelectedData()
// Desc : returns the data of the current selected Item
//-----------------------------------------------------------------------------
template<class T>
T* ComboBoxUI<T>::GetSelectedData()
{
    if (m_iFocused != -1)
        return m_dropDown.GetItemData(m_iFocused);
    else
        return nullptr;
}

//-----------------------------------------------------------------------------
// Name : GetSelectedItem()
// Desc : returns the current selected Item
//-----------------------------------------------------------------------------
template<class T>
Item<T>* ComboBoxUI<T>::GetSelectedItem()
{
    if (m_iFocused != -1)
        return m_dropDown.GetItem(m_iFocused);
    else
        return nullptr;
}

//-----------------------------------------------------------------------------
// Name : GetNumItems()
//-----------------------------------------------------------------------------
template<class T>
GLuint ComboBoxUI<T>::GetNumItems()
{
    return m_dropDown.GetNumItems();
}

//-----------------------------------------------------------------------------
// Name : GetItem()
//-----------------------------------------------------------------------------
template<class T>
Item<T>* ComboBoxUI<T>::GetItem( GLuint index )
{
    return m_dropDown.GetItem(index);
}

//-----------------------------------------------------------------------------
// Name : SetSelectedByIndex()
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::SetSelectedByIndex( GLuint index )
{
    const std::vector<int>& selectedIndices =  m_dropDown.GetSelectedIndices();
    if (selectedIndices.size() > 0)
        m_dropDown.SelectItem(selectedIndices[0], false);

    if (m_dropDown.SelectItem(index, true))
    {
        m_iFocused = index;
        m_selectionChangedSig(this);
        return true;
    }
    else
        return false;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedByText()
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::SetSelectedByText( std::string strText )
{
    const std::vector<int>& selectedIndices =  m_dropDown.GetSelectedIndices();
    if (selectedIndices.size() > 0)
        m_dropDown.SelectItem(selectedIndices[0], false);

    bool ret = m_dropDown.SelectItem(strText, true);
    if (m_dropDown.SelectItem(strText, true))
    {
        m_iFocused = m_dropDown.GetSelectedIndices().back();
        m_selectionChangedSig(this);
        return true;
    }
    else
        return false;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedByData()
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::SetSelectedByData( void* pData )
{
//    for( GLuint i = 0; i < m_Items.size(); i++ )
//    {
//        ComboBoxItem* pItem = m_Items[i];

//        if( pItem->pData == pData )
//        {
//            m_iFocused = m_iSelected = i;
//            m_selectionChangedSig(this);
//            return true;
//        }
//    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : CanHaveFocus()
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::CanHaveFocus()
{
    return ( m_bVisible && m_bEnabled );
}

//-----------------------------------------------------------------------------
// Name : OnFocusOut()
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::OnFocusOut()
{
    ButtonUI::OnFocusOut();
    m_bOpened = false;
}

//-----------------------------------------------------------------------------
// Name : SaveToFile()
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::SaveToFile(std::ostream& SaveFile)
{
    ButtonUI::SaveToFile(SaveFile);
    m_dropDown.SaveToFile(SaveFile);

    return true;
}

//-----------------------------------------------------------------------------
// Name : CopyItemsFrom
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::CopyItemsFrom(ComboBoxUI* sourceComboBox)
{
    // clears the items vector
    RemoveAllItems();

    for (GLuint i = 0; i < sourceComboBox->GetNumItems(); i++)
    {
        AddItem( sourceComboBox->GetItem(i)->strText, sourceComboBox->GetItem(i)->data );
    }
}

//-----------------------------------------------------------------------------
// Name : ContainsPoint
//-----------------------------------------------------------------------------
template<class T>
bool ComboBoxUI<T>::ContainsPoint(Point pt)
{
    if (m_rcBoundingBox.isPointInRect(pt))
        return true;
    else
        if (m_bOpened && m_dropDown.ContainsPoint(pt))
            return true;
        else
            return false;
}

//-----------------------------------------------------------------------------
// Name : onMouseEnter
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::onMouseEnter()
{
    m_bMouseOver = true;
    m_dropDown.onMouseEnter();
}

//-----------------------------------------------------------------------------
// Name : onMouseLeave
//-----------------------------------------------------------------------------
template<class T>
void ComboBoxUI<T>::onMouseLeave()
{
    m_bMouseOver = false;
    m_dropDown.onMouseLeave();
}

#endif  //_COMBOBOXUI_CPP
