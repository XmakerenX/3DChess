#include "ComboBoxUI.h"
#include "DialogUI.h"
#include <iostream>

//-----------------------------------------------------------------------------
// Name : ComboBoxUI(constructor)
//-----------------------------------------------------------------------------
ComboBoxUI::ComboBoxUI(DialogUI* pParentDialog, int ID, std::string strText, int x, int y, GLuint width, GLuint height, GLuint nHotkey)
    :ButtonUI(pParentDialog, ID, strText, x, y, width, height, nHotkey)
{
    m_type = ControlUI::COMBOBOX;

    m_nDropHeight = 100;

    m_ScrollBarWidth = 16;
    m_bOpened = false;
    m_iSelected = -1;
    m_iFocused = -1;
}

//-----------------------------------------------------------------------------
// Name : ComboBoxUI(constructor from InputFile)
//-----------------------------------------------------------------------------
ComboBoxUI::ComboBoxUI(std::istream& inputFile)
    :ButtonUI(inputFile)
{
    GLuint itemsSize = 0;;
    m_type = ControlUI::COMBOBOX;

    m_bOpened = false;
    m_iSelected = -1;
    m_iFocused = -1;

    inputFile >> m_nDropHeight;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_ScrollBarWidth;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_nFontHeight;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> itemsSize;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

    for (GLuint i = 0; i < itemsSize; i++)
    {
        std::string strText;

        std::getline(inputFile, strText);
        strText = strText.substr(0, strText.find('|') );
        AddItem(strText.c_str(), nullptr);

        inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    }
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
    return m_pParentDialog->initControl(&m_ScrollBar);
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
bool ComboBoxUI::handleMouseEvent(MouseEvent event)
{
    if (!m_bEnabled || !m_bVisible)
        return false;

    // Let the scroll bar handle it first.
    if( m_ScrollBar.handleMouseEvent(event))
        return true;

    switch(event.type)
    {
    case MouseEventType::MouseMoved:
    {
        if ( Highlight(event.cursorPos) )
            return true;
    }break;

    case MouseEventType::LeftButton:
    case MouseEventType::DoubleLeftButton:
    {
        if (event.down)
        {
            if ( Pressed(event.cursorPos, INPUT_STATE(), event.timeStamp) )
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
bool ComboBoxUI::Pressed(Point pt, INPUT_STATE inputState, double timeStamp)
{
    if( ContainsPoint( pt ) )
    {
        // Pressed while inside the control
        m_bPressed = true;

        if( !m_bHasFocus )
            m_pParentDialog->RequestFocus( this );

        // Toggle dropdown
        if( m_bHasFocus )
        {
            m_bOpened = !m_bOpened;

            if( !m_bOpened )
            {
                //TODO: check what this suppose to do ?
                // 						if( !m_pParentDialog->m_bKeyboardInput )
                // 							m_pDialog->ClearFocus();
            }
        }

        return true;
    }

    // Perhaps this click is within the dropdown
    if( m_bOpened && m_rcDropdown.isPointInRect(pt) )
    {
        // Determine which item has been selected
        for(GLuint i = m_ScrollBar.GetTrackPos(); i < m_Items.size(); i++ )
        {
            ComboBoxItem* pItem = m_Items[i];
            if( pItem->bVisible && pItem->rcActive.isPointInRect(pt) )
            {
                m_iFocused = m_iSelected = i;
                m_selectionChangedSig( this);
                //m_pParentDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, m_ID, hWnd );
                m_bOpened = false;

                m_bMouseOver = false;

                // 						if( !m_pDialog->m_bKeyboardInput )
                // 							m_pDialog->ClearFocus();

                break;
            }
        }

        return true;
    }

    // Mouse click not on main control or in dropdown, fire an event if needed
    if( m_bOpened )
    {
        m_iFocused = m_iSelected;
        m_selectionChangedSig( this );
        //m_pParentDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, m_ID, hWnd);
        m_bOpened = false;
    }

    // Make sure the control is no longer in a pressed state
    m_bPressed = false;

    // Release focus if appropriate
    // 			if( !m_pParentDialog->m_bKeyboardInput )
    // 			{
    // 				m_pParentDialog->ClearFocus();
    // 			}
    return false;
}

//-----------------------------------------------------------------------------
// Name : Released()
//-----------------------------------------------------------------------------
bool ComboBoxUI::Released(Point pt)
{
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
    if( m_bOpened )
    {
        m_ScrollBar.Scroll( -nScrollAmount /** uLines*/ );
    }
    else
    {
        if (m_bMouseOver)
        {
            if( nScrollAmount > 0 )
            {
                if( m_iFocused > 0 )
                {
                    m_iFocused--;
                    m_iSelected = m_iFocused;

                    if( !m_bOpened )
                        m_selectionChangedSig( this );
                }
            }
            else
            {
                if( m_iFocused + 1 < ( int )GetNumItems() )
                {
                    m_iFocused++;
                    m_iSelected = m_iFocused;

                    if( !m_bOpened )
                        m_selectionChangedSig( this );
                }
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// Name : Highlight()
//-----------------------------------------------------------------------------
bool ComboBoxUI::Highlight(Point mousePoint)
{
    if( m_bOpened && m_rcDropdown.isPointInRect(mousePoint) )
    {
        // Determine which item has been selected
        for( GLuint i = 0; i < m_Items.size(); i++ )
        {
            ComboBoxItem* pItem = m_Items[i];
            if( pItem->bVisible && pItem->rcActive.isPointInRect(mousePoint) )
            {
                m_iFocused = i;
            }
        }
        return true;
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
    if (m_bVisible)
    {
        // check that there is actual fonts
        if (m_elementsFonts.size() == 0)
            return;	// if the function returns here initDefalutElements wasn't called or Elements were not defined

        Point dialogPos = m_pParentDialog->getLocation();
        long  dialogCaptionHeihgt =  m_pParentDialog->getCaptionHeight();
        dialogPos.y += dialogCaptionHeihgt;

        //if Combobox is open render the scrollbar
        if (m_bOpened)
        {
            m_ScrollBar.Render(sprite, textSprite, timeStamp);

            // acquire a pointer for the dropdown texture
            renderRect(sprite, m_rcDropdown, m_elementsGFX[DROPDOWN].iTexture, m_elementsGFX[DROPDOWN].rcTexture, WHITE_COLOR, dialogPos);

        }

        int curY = m_rcDropdownText.top;
        int nRemainingHeight =  m_rcDropdownText.bottom - m_rcDropdownText.top;

        // render all item within the dropdown box;
        for (int i = m_ScrollBar.GetTrackPos(); i < m_Items.size(); i++)
        {
            ComboBoxItem* pItem = m_Items[i];

            Point strDim = m_elementsFonts[0].font->calcTextRect(pItem->strText);
            strDim.y += 4;
            // Make sure there's room left in the dropdown
            nRemainingHeight -= strDim.y;
            if( nRemainingHeight < 0 )
            {
                pItem->bVisible = false;
                continue;
            }

            pItem->rcActive = Rect(m_rcDropdownText.left, curY, m_rcDropdownText.right, curY + strDim.y);
            curY += strDim.y;

            pItem->bVisible = true;

            if( m_bOpened )
            {
                if( i == m_iFocused )
                {
                    //Rect rc(m_rcDropdown.left, pItem->rcActive.top - 2, m_rcDropdown.right, pItem->rcActive.bottom + 2);
                    Rect rc(m_rcDropdown.left, pItem->rcActive.top, m_rcDropdown.right, pItem->rcActive.bottom);

                    renderRect(sprite, rc, m_elementsGFX[SELECTION].iTexture, m_elementsGFX[SELECTION].rcTexture, WHITE_COLOR, dialogPos);
                    Rect rcTextRect(rc);
                    rcTextRect.offset(dialogPos.x, dialogPos.y);
                    rcTextRect.bottom = rcTextRect.bottom;
                    rcTextRect.top = rcTextRect.top;
                    m_elementsFonts[0].font->renderToRect(textSprite, pItem->strText, rcTextRect, WHITE_COLOR, mkFont::TextFormat::Center);
                }
                else
                {
                    Rect rcTextRect(pItem->rcActive);
                    rcTextRect.offset(dialogPos.x, dialogPos.y);
                    m_elementsFonts[0].font->renderToRect(textSprite, pItem->strText, rcTextRect, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), mkFont::TextFormat::Center);

                }
            }
        }
        // end of render dropDownButton

        //pTexture = assetManger.getTexturePtr(m_elementsGFX[BUTTON].iTexture);
        //LPDIRECT3DTEXTURE9 pTexture2 = assetManger.getTexturePtr(m_elementsGFX[MAIN].iTexture);

        //if the button is not pressed or doesn't have the cursor on it render it normally
        if (!m_bMouseOver && !m_bOpened)
        {
            if(m_bEnabled)
            {
                renderRect(sprite, m_rcButton, m_elementsGFX[BUTTON].iTexture, m_elementsGFX[BUTTON].rcTexture, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), dialogPos);
                renderRect(sprite, m_rcText, m_elementsGFX[MAIN].iTexture, m_elementsGFX[MAIN].rcTexture, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), dialogPos);
            }
            else
            {
                renderRect(sprite, m_rcButton, m_elementsGFX[BUTTON].iTexture, m_elementsGFX[BUTTON].rcTexture, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), dialogPos);
                renderRect(sprite, m_rcText, m_elementsGFX[MAIN].iTexture, m_elementsGFX[MAIN].rcTexture, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), dialogPos);
            }
        }
        else
        {
            // if the button is pressed and the cursor is on it darken it to showed it is pressed
            if (m_bMouseOver && m_bPressed)
            {
                renderRect(sprite, m_rcButton, m_elementsGFX[BUTTON].iTexture, m_elementsGFX[BUTTON].rcTexture, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), dialogPos);
                renderRect(sprite, m_rcText, m_elementsGFX[MAIN].iTexture, m_elementsGFX[MAIN].rcTexture, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), dialogPos);
            }
            else
                // if the button has the cursor on it high light
                if (m_bMouseOver || m_bOpened)
                {
                    renderRect(sprite, m_rcButton, m_elementsGFX[BUTTON].iTexture, m_elementsGFX[BUTTON].rcTexture, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), dialogPos);
                    renderRect(sprite, m_rcText, m_elementsGFX[MAIN].iTexture, m_elementsGFX[MAIN].rcTexture, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), dialogPos);
                }
        }

        if( m_iSelected >= 0 && m_iSelected < ( int )m_Items.size() )
        {
            ComboBoxItem* pItem = m_Items[m_iSelected];
            if( pItem != NULL )
            {
                if (!m_bMouseOver && !m_bOpened)
                {
                    if (m_elementsFonts.size() > 0)
                    {
                        Rect rcTextRect(m_rcText);
                        rcTextRect.offset(dialogPos.x, dialogPos.y);
                        m_elementsFonts[0].font->renderToRect(textSprite, pItem->strText, rcTextRect, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), mkFont::TextFormat::Center);
                    }
                }
                else
                {
                    if (m_elementsFonts.size() > 0)
                    {
                        Rect rcTextRect(m_rcText);
                        rcTextRect.offset(dialogPos.x, dialogPos.y);
                        m_elementsFonts[0].font->renderToRect(textSprite, pItem->strText, rcTextRect, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), mkFont::TextFormat::Center);
                    }
                }

            }
        }
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

    m_rcDropdown = m_rcText;
    m_rcDropdown.offset(0, ( int )( 0.90f * (m_rcText.bottom - m_rcText.top)));
    m_rcDropdown.bottom += m_nDropHeight;
    m_rcDropdown.right -= m_ScrollBarWidth;

    m_rcDropdownText = m_rcDropdown;
//    m_rcDropdownText.left += ( int )( 0.1f * (m_rcDropdown.right - m_rcDropdown.left) );
//    m_rcDropdownText.right -= ( int )( 0.1f * (m_rcDropdown.right - m_rcDropdown.left) );
    m_rcDropdownText.top += ( int )( 0.05f * (m_rcDropdown.getHeight()) );
    //m_rcDropdownText.bottom -= ( int )( 0.1f * (m_rcDropdown.getHeight()) );

    // Update the scrollbar's rects
    m_ScrollBar.setLocation( m_rcDropdown.right, m_rcDropdown.top + 2 );
    m_ScrollBar.setSize( m_ScrollBarWidth, m_rcDropdown.bottom - m_rcDropdown.top - 2 );


    m_ScrollBar.SetPageSize( (m_rcDropdownText.bottom - m_rcDropdownText.top)  / (m_elementsFonts[0].font->getFontSize() + 0.1875*m_elementsFonts[0].font->getFontSize() + 4));

    // The selected item may have been scrolled off the page.
    // Ensure that it is in page again.
    m_ScrollBar.ShowItem( m_iSelected );

}

//-----------------------------------------------------------------------------
// Name : AddItem()
// Desc : add an item to the combobox
//-----------------------------------------------------------------------------
//TODO: sort the objects by something else than pData or change it from void* to some kind of ENUM
bool ComboBoxUI::AddItem( std::string strText, void* pData )
{
    // Validate parameters
    if( strText == "" )
    {
        return false;
    }

    // Create a new item and set the data
    ComboBoxItem* newItem = new ComboBoxItem(strText);
    newItem->pData = pData;

    //TODO: add something that will catch the exception thrown by push_back in case of lack of memory
    m_Items.push_back(newItem);

    // Update the scroll bar with new range
    m_ScrollBar.SetTrackRange( 0, m_Items.size() );

    // If this is the only item in the list, it's selected
    if( GetNumItems() == 1 )
    {
        m_iSelected = 0;
        m_iFocused = 0;
        m_selectionChangedSig(this);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Name : RemoveItem()
// Desc : remove an item at the given index
//-----------------------------------------------------------------------------
void ComboBoxUI::RemoveItem( GLuint index )
{
    ComboBoxItem* pItem = m_Items[index];

    if (pItem)
    {
        delete pItem;
        pItem = NULL;
    }

    m_Items.erase(m_Items.begin() + index);

    m_ScrollBar.SetTrackRange( 0, m_Items.size() );
    if( m_iSelected >= m_Items.size() )
        m_iSelected = m_Items.size() - 1;
}

//-----------------------------------------------------------------------------
// Name : RemoveAllItems()
// Desc : removes all the items from the combobox
//-----------------------------------------------------------------------------
void ComboBoxUI::RemoveAllItems()
{
    for( GLuint i = 0; i < m_Items.size(); i++ )
    {
        ComboBoxItem* pItem = m_Items[i];
        if (pItem)
        {
            delete pItem;
            pItem = NULL;
        }
    }

    m_Items.clear();
    m_ScrollBar.SetTrackRange( 0, 1 );
    m_iFocused = m_iSelected = -1;
}

//-----------------------------------------------------------------------------
// Name : FindItem()
// Desc : finds an item that match the given text and returns the item index
// Note : iStart can be given if there is need to start the check from a certain index
//-----------------------------------------------------------------------------
int ComboBoxUI::FindItem( std::string strText, GLuint iStart/* = 0 */)
{
    if( strText == "" )
        return -1;

    for( GLuint i = iStart; i < m_Items.size(); i++ )
    {
        ComboBoxItem* pItem = m_Items[i];

        if (pItem->strText == strText)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Name : ContainsItem()
// Desc : check is there is a Item that match the given text
// Note : iStart can be given if there is need to start the check from a certain index
//-----------------------------------------------------------------------------
bool ComboBoxUI::ContainsItem(std::string strText, GLuint iStart /*= 0 */)
{
        return ( FindItem( strText, iStart ) != -1 );
}

//-----------------------------------------------------------------------------
// Name : GetItemData()
// Desc : returns the item data
// Note : the search for the item is being done based on the item text
//-----------------------------------------------------------------------------
void* ComboBoxUI::GetItemData( std::string strText )
{
    int index = FindItem( strText );
    if( index == -1 )
    {
        return nullptr;
    }

    ComboBoxItem* pItem = m_Items[index];
    if( pItem == nullptr )
    {
        //TODO: add some way to pop up errors Messages
        //DXTRACE_ERR( L"CGrowableArray::GetAt", E_FAIL );
        return nullptr;
    }

    return pItem->pData;
}

//-----------------------------------------------------------------------------
// Name : GetItemData()
// Desc : returns the item data
// Note : the search for the item is being done based on the item index
//-----------------------------------------------------------------------------
void* ComboBoxUI::GetItemData( int nIndex )
{
    if( nIndex < 0 || nIndex >= m_Items.size() )
        return NULL;

    return m_Items[nIndex]->pData;
}

//-----------------------------------------------------------------------------
// Name : SetDropHeight()
//-----------------------------------------------------------------------------
void ComboBoxUI::SetDropHeight( GLuint nHeight )
{
    m_nDropHeight = nHeight;
    UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : GetScrollBarWidth()
//-----------------------------------------------------------------------------
int ComboBoxUI::GetScrollBarWidth() const
{
    return m_ScrollBarWidth;
}

//-----------------------------------------------------------------------------
// Name : SetScrollBarWidth()
//-----------------------------------------------------------------------------
void ComboBoxUI::SetScrollBarWidth( int nWidth )
{
    m_ScrollBarWidth = nWidth;
    UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : GetSelectedIndex()
//-----------------------------------------------------------------------------
int ComboBoxUI::GetSelectedIndex() const
{
    return m_iSelected;
}

//-----------------------------------------------------------------------------
// Name : GetSelectedData()
// Desc : returns the data of the current selected Item
//-----------------------------------------------------------------------------
void* ComboBoxUI::GetSelectedData()
{
    if( m_iSelected < 0 )
        return NULL;

    ComboBoxItem* pItem = m_Items[m_iSelected];
    return pItem->pData;
}

//-----------------------------------------------------------------------------
// Name : GetSelectedItem()
// Desc : returns the current selected Item
//-----------------------------------------------------------------------------
ComboBoxItem* ComboBoxUI::GetSelectedItem()
{
    if( m_iSelected < 0 )
        return NULL;

    return m_Items[m_iSelected];
}

//-----------------------------------------------------------------------------
// Name : GetNumItems()
//-----------------------------------------------------------------------------
GLuint ComboBoxUI::GetNumItems()
{
    return m_Items.size();
}

//-----------------------------------------------------------------------------
// Name : GetItem()
//-----------------------------------------------------------------------------
ComboBoxItem* ComboBoxUI::GetItem( GLuint index )
{
    return m_Items[index];
}

//-----------------------------------------------------------------------------
// Name : SetSelectedByIndex()
//-----------------------------------------------------------------------------
bool ComboBoxUI::SetSelectedByIndex( GLuint index )
{
    if( index >= GetNumItems() )
        return false;

    m_iFocused = m_iSelected = index;
    // TODO: make a signal for this
    //m_pParentDialog->SendEvent( 9, false, NULL );

    return true;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedByText()
//-----------------------------------------------------------------------------
bool ComboBoxUI::SetSelectedByText( std::string strText )
{
    if( strText == "" )
        return false;

    int index = FindItem( strText );
    if( index == -1 )
        return false;

    m_iFocused = m_iSelected = index;
    // TODO: make a signal for this
    //m_pParentDialog->SendEvent( 9, false, NULL );

    return true;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedByData()
//-----------------------------------------------------------------------------
bool ComboBoxUI::SetSelectedByData( void* pData )
{
    for( GLuint i = 0; i < m_Items.size(); i++ )
    {
        ComboBoxItem* pItem = m_Items[i];

        if( pItem->pData == pData )
        {
            m_iFocused = m_iSelected = i;
            // TODO: make a signal for this
            //m_pParentDialog->SendEvent( 9, false, NULL );
            return true;
        }
    }

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

    SaveFile << m_nDropHeight << "| ComboBox Drop Height" << "\n";
    SaveFile << m_ScrollBarWidth << "| ComboBox SBWidth" << "\n";
    SaveFile << m_nFontHeight << "| ComboBox Font Height" << "\n";

    SaveFile << m_Items.size() << "| ComboBox Items Size" << "\n";

    for (GLuint i = 0; i < m_Items.size(); i++)
    {
        SaveFile << m_Items[i]->strText << "| ComboBox Item "<< i <<" Text" << "\n";
    }

    return true;
}

//-----------------------------------------------------------------------------
// Name : CopyItemsFrom
//-----------------------------------------------------------------------------
void ComboBoxUI::CopyItemsFrom(ComboBoxUI* sourceComboBox)
{
    // clears the items vector
    RemoveAllItems();

    for (GLuint i = 0; i < sourceComboBox->GetNumItems(); i++)
    {
        AddItem( sourceComboBox->GetItem(i)->strText, sourceComboBox->GetItem(i)->pData );
    }
}
