#include "ComboBoxUI.h"
#include "DialogUI.h"
#include "ListBoxUI.cpp"
#include <iostream>

//-----------------------------------------------------------------------------
// Name : ComboBoxUI(constructor)
//-----------------------------------------------------------------------------
ComboBoxUI::ComboBoxUI(DialogUI* pParentDialog, int ID, std::string strText, int x, int y, GLuint width, GLuint height, GLuint nHotkey)
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
ComboBoxUI::ComboBoxUI(std::istream& inputFile)
    :ButtonUI(inputFile), m_dropDown(inputFile)
{
    m_type = ControlUI::COMBOBOX;

    m_bOpened = false;
    m_dropDown.setVisible(m_bOpened);
    m_iFocused = -1;
}

//-----------------------------------------------------------------------------
// Name : CComboBoxUI(destructor)
//-----------------------------------------------------------------------------
ComboBoxUI::~ComboBoxUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : onInit()
// Desc : initialize the scrollbar element
//-----------------------------------------------------------------------------
bool ComboBoxUI::onInit()
{
    return m_pParentDialog->initControl(&m_dropDown);
}

//-----------------------------------------------------------------------------
// Name : setHotKey ()
//-----------------------------------------------------------------------------
void ComboBoxUI::OnHotkey()
{
    ;
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent()
// Desc : Handles mouse input for the Combo box
//-----------------------------------------------------------------------------
bool ComboBoxUI::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
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
bool ComboBoxUI::Pressed(Point pt, const ModifierKeysStates &modifierStates, double timeStamp)
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
bool ComboBoxUI::Released(Point pt)
{
    if (m_dropDown.ContainsPoint(pt))
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
bool ComboBoxUI::Scrolled( int nScrollAmount)
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
void ComboBoxUI::ConnectToSelectChg( const signal_comboBox::slot_type& subscriber)
{
    m_selectionChangedSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : Render()
//-----------------------------------------------------------------------------
void ComboBoxUI::Render(Sprite& sprite, Sprite& textSprite, double timeStamp)
{    
    // check that there is actual fonts
    if (!m_bVisible ||  m_elementsFonts.size() == 0)
        return;

    Point dialogPos = calcPositionOffset();

    //if Combobox is open render the scrollbar
    if (m_bOpened)
    {
        m_dropDown.Render(sprite, textSprite, timeStamp);
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

    renderRect(sprite, m_rcButton, m_elementsGFX[BUTTON].iTexture, m_elementsGFX[BUTTON].rcTexture, tintColor, dialogPos);
    renderRect(sprite, m_rcText, m_elementsGFX[MAIN].iTexture, m_elementsGFX[MAIN].rcTexture, tintColor, dialogPos);

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
            renderText(textSprite, m_elementsFonts[0].font, m_dropDown.GetItem(m_iFocused)->strText
                    , textColor, m_rcText, dialogPos, mkFont::TextFormat::Center);
    }
}

//-----------------------------------------------------------------------------
// Name : UpdateRects()
//-----------------------------------------------------------------------------
void ComboBoxUI::UpdateRects()
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
bool ComboBoxUI::AddItem( std::string strText, int pData )
{
    bool ret = m_dropDown.AddItem(strText, 1);
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
void ComboBoxUI::RemoveItem( GLuint index )
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
void ComboBoxUI::RemoveAllItems()
{
    m_dropDown.RemoveAllItems();
    m_iFocused = -1;
}

//-----------------------------------------------------------------------------
// Name : FindItem()
// Desc : finds an item that match the given text and returns the item index
// Note : iStart can be given if there is need to start the check from a certain index
//-----------------------------------------------------------------------------
int ComboBoxUI::FindItem( std::string strText, GLuint iStart/* = 0 */)
{
    return m_dropDown.FindItem(strText, iStart);
}

//-----------------------------------------------------------------------------
// Name : ContainsItem()
// Desc : check is there is a Item that match the given text
// Note : iStart can be given if there is need to start the check from a certain index
//-----------------------------------------------------------------------------
bool ComboBoxUI::ContainsItem(std::string strText, GLuint iStart /*= 0 */)
{
    return m_dropDown.ContainsItem(strText, iStart);
}

//-----------------------------------------------------------------------------
// Name : GetItemData()
// Desc : returns the item data
// Note : the search for the item is being done based on the item text
//-----------------------------------------------------------------------------
void* ComboBoxUI::GetItemData( std::string strText )
{
    return (void*)m_dropDown.GetItemData(strText);
}

//-----------------------------------------------------------------------------
// Name : GetItemData()
// Desc : returns the item data
// Note : the search for the item is being done based on the item index
//-----------------------------------------------------------------------------
void* ComboBoxUI::GetItemData( int nIndex )
{
    return m_dropDown.GetItemData(nIndex);
}

//-----------------------------------------------------------------------------
// Name : SetDropHeight()
//-----------------------------------------------------------------------------
void ComboBoxUI::SetDropHeight( GLuint nHeight )
{
    m_dropDown.setSize(m_dropDown.getWidth(), nHeight);
}

//-----------------------------------------------------------------------------
// Name : GetScrollBarWidth()
//-----------------------------------------------------------------------------
int ComboBoxUI::GetScrollBarWidth() const
{
    return m_dropDown.GetScrollBarWidth();
}

//-----------------------------------------------------------------------------
// Name : SetScrollBarWidth()
//-----------------------------------------------------------------------------
void ComboBoxUI::SetScrollBarWidth( int nWidth )
{
    m_dropDown.SetScrollBarWidth(nWidth);
    UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : GetSelectedIndex()
//-----------------------------------------------------------------------------
int ComboBoxUI::GetSelectedIndex() const
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
int* ComboBoxUI::GetSelectedData()
{
    const std::vector<int> selIndices = m_dropDown.GetSelectedIndices();
    if (selIndices.size() > 0)
        return m_dropDown.GetItemData(selIndices.back());
    else
        return nullptr;
}

//-----------------------------------------------------------------------------
// Name : GetSelectedItem()
// Desc : returns the current selected Item
//-----------------------------------------------------------------------------
Item<int>* ComboBoxUI::GetSelectedItem()
{
    const std::vector<int> selIndices = m_dropDown.GetSelectedIndices();
    if (selIndices.size() > 0)
        return m_dropDown.GetItem(selIndices.back());
    else
        return nullptr;
}

//-----------------------------------------------------------------------------
// Name : GetNumItems()
//-----------------------------------------------------------------------------
GLuint ComboBoxUI::GetNumItems()
{
    return m_dropDown.GetNumItems();
}

//-----------------------------------------------------------------------------
// Name : GetItem()
//-----------------------------------------------------------------------------
Item<int>* ComboBoxUI::GetItem( GLuint index )
{
    return m_dropDown.GetItem(index);
}

//-----------------------------------------------------------------------------
// Name : SetSelectedByIndex()
//-----------------------------------------------------------------------------
bool ComboBoxUI::SetSelectedByIndex( GLuint index )
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
bool ComboBoxUI::SetSelectedByText( std::string strText )
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
bool ComboBoxUI::SetSelectedByData( void* pData )
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
bool ComboBoxUI::CanHaveFocus()
{
    return ( m_bVisible && m_bEnabled );
}

//-----------------------------------------------------------------------------
// Name : OnFocusOut()
//-----------------------------------------------------------------------------
void ComboBoxUI::OnFocusOut()
{
    ButtonUI::OnFocusOut();
    m_bOpened = false;
}

//-----------------------------------------------------------------------------
// Name : SaveToFile()
//-----------------------------------------------------------------------------
bool ComboBoxUI::SaveToFile(std::ostream& SaveFile)
{
    ButtonUI::SaveToFile(SaveFile);
    m_dropDown.SaveToFile(SaveFile);

    return true;
}

//-----------------------------------------------------------------------------
// Name : CopyItemsFrom
//-----------------------------------------------------------------------------
void ComboBoxUI::CopyItemsFrom(ComboBoxUI* sourceComboBox)
{
    //m_dropDown.CopyItemsFrom();
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
bool ComboBoxUI::ContainsPoint(Point pt)
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
void ComboBoxUI::onMouseEnter()
{
    m_bMouseOver = true;
    m_dropDown.onMouseEnter();
}

//-----------------------------------------------------------------------------
// Name : onMouseLeave
//-----------------------------------------------------------------------------
void ComboBoxUI::onMouseLeave()
{
    m_bMouseOver = false;
    m_dropDown.onMouseLeave();
}
