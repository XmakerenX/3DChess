#include "ListBoxUI.h"
#include "DialogUI.h"
#include <algorithm>
//-----------------------------------------------------------------------------
// Name : ListBoxUI (constructor)
//-----------------------------------------------------------------------------
ListBoxUI::ListBoxUI(DialogUI *pParentDialog, int ID, int x, int y, int width, int height, GLuint dwStyle)
    :ControlUI(pParentDialog, ID, x, y, width, height)
{
    m_type = ControlUI::LISTBOX;

	m_dwStyle = dwStyle;
	m_nSBWidth = 16;
	m_nSelected = -1;
	m_nSelStart = 0;
	m_bDrag = false;
	m_nBorder = 6;
	m_nMargin = 5;
	m_nTextHeight = 0;
}

//-----------------------------------------------------------------------------
// Name : ListBoxUI (constructor from InputFile)
//-----------------------------------------------------------------------------
ListBoxUI::ListBoxUI(std::istream& inputFile)
    :ControlUI(inputFile)
{
    GLuint itemsSize = 0;
    m_type = ControlUI::LISTBOX;

	m_nSelected = -1;
	m_nSelStart = 0;
	m_bDrag = false;

	inputFile >> m_nSBWidth;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nBorder;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nMargin;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nTextHeight;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_dwStyle;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

	inputFile >> itemsSize;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

    for (GLuint i = 0; i < itemsSize; i++)
	{
		std::string strText;

		std::getline(inputFile, strText);
		strText = strText.substr(0, strText.find('|') );
        AddItem(strText, nullptr);

		inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	}

}

//-----------------------------------------------------------------------------
// Name : ListBoxUI (destructor)
//-----------------------------------------------------------------------------
ListBoxUI::~ListBoxUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : onInit()
//-----------------------------------------------------------------------------
bool ListBoxUI::onInit()
{
	return m_pParentDialog->initControl( &m_ScrollBar );
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent
//-----------------------------------------------------------------------------
bool ListBoxUI::handleMouseEvent(MouseEvent event)
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    // Let the scroll bar handle it first.
    if( m_ScrollBar.handleMouseEvent(event))
        return true;

    switch(event.type)
    {
    case MouseEventType::LeftButton:
    {
        if (event.down)
        {
            if ( Pressed(event.cursorPos, INPUT_STATE(), event.timeStamp))
                return true;
        }
        else
        {
            if ( Released(event.cursorPos))
                return true;
        }
    }break;

    case MouseEventType::MouseMoved:
    {
        if (Dragged(event.cursorPos))
            return true;
    }break;

    case MouseEventType::ScrollVert:
    {
        if ( Scrolled( event.nLinesToScroll ))
            return true;
    }break;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed
//-----------------------------------------------------------------------------
bool ListBoxUI::Pressed( Point pt, INPUT_STATE inputState, double timeStamp)
{
    if (!m_bHasFocus &&  ContainsPoint(pt))
            m_pParentDialog->RequestFocus( this );

	// Check for clicks in the text area
    if( m_Items.size() > 0 && m_rcSelection.isPointInRect(pt))
	{
		// Compute the index of the clicked item

		int nClicked = -1;
		int temp = m_rcBoundingBox.top;

		if( m_nTextHeight )
		{
			nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
		}
		else
			nClicked = -1;

		// Only proceed if the click falls on top of an item.
		if( nClicked >= m_ScrollBar.GetTrackPos() &&
			nClicked < ( int )m_Items.size() &&
			nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
		{
			m_bDrag = true;

            // If this is a double click, fire off an event and exit since
            // the first click would have taken care of the selection  updating
			//if( uMsg == WM_LBUTTONDBLCLK )
//			if ( inputState.bDoubleClick )
//			{
//				return true;
//			}

			m_nSelected = nClicked;
			if( !( inputState.bShift ) )
				m_nSelStart = m_nSelected;

			// If this is a multi-selection listbox, update per-item
			// selection data.
            if( m_dwStyle == MULTISELECTION )
			{
				// Determine behavior based on the state of Shift and Ctrl
				ListBoxItemUI* pSelItem = m_Items[m_nSelected];
				if( inputState.bCtrl && !inputState.bShift )
				{
					// Control click. Reverse the selection of this item.
					pSelItem->bSelected = !pSelItem->bSelected;
				}
				else if( !inputState.bCtrl && inputState.bShift)
				{
					// Shift click. Set the selection for all items
					// from last selected item to the current item.
					// Clear everything else.

                    int nBegin = std::min( m_nSelStart, m_nSelected );
                    int nEnd   = std::max( m_nSelStart, m_nSelected );

					for( int i = 0; i < nBegin; ++i )
					{
						ListBoxItemUI* pItem = m_Items[i];
						pItem->bSelected = false;
					}

					for( int i = nEnd + 1; i < ( int )m_Items.size(); ++i )
					{
						ListBoxItemUI* pItem = m_Items[i];
						pItem->bSelected = false;
					}

					for( int i = nBegin; i <= nEnd; ++i )
					{
						ListBoxItemUI* pItem = m_Items[i];
						pItem->bSelected = true;
					}
				}
				else if( inputState.bCtrl && inputState.bShift )
				{
					// Control-Shift-click.
					// The behavior is:
					//   Set all items from m_nSelStart to m_nSelected to
					//     the same state as m_nSelStart, not including m_nSelected.
					//   Set m_nSelected to selected.

                    int nBegin = std::min( m_nSelStart, m_nSelected );
                    int nEnd   = std::max( m_nSelStart, m_nSelected );

					// The two ends do not need to be set here.

					bool bLastSelected = m_Items[m_nSelStart]->bSelected;
					for( int i = nBegin + 1; i < nEnd; ++i )
					{
						ListBoxItemUI* pItem = m_Items[ i ];
						pItem->bSelected = bLastSelected;
					}

					pSelItem->bSelected = true;

					// Restore m_nSelected to the previous value
					// This matches the Windows behavior
					m_nSelected = m_nSelStart;
				}
				else
				{
					// Simple click.  Clear all items and select the clicked
					// item.
					for( int i = 0; i < ( int )m_Items.size(); ++i )
					{
						ListBoxItemUI* pItem = m_Items[i];
						pItem->bSelected = false;
					}

					pSelItem->bSelected = true;
				}
			}  // End of multi-selection case

		}

		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Released
//-----------------------------------------------------------------------------
bool ListBoxUI::Released(Point pt)
{
	m_bDrag = false;

	if( m_nSelected != -1 )
	{
		// Set all items between m_nSelStart and m_nSelected to
		// the same state as m_nSelStart
        int nEnd = std::max( m_nSelStart, m_nSelected );

        for( int n = std::min( m_nSelStart, m_nSelected ) + 1; n < nEnd; ++n )
			m_Items[n]->bSelected = m_Items[m_nSelStart]->bSelected;
		m_Items[m_nSelected]->bSelected = m_Items[m_nSelStart]->bSelected;

		// If m_nSelStart and m_nSelected are not the same,
		// the user has dragged the mouse to make a selection.
		// Notify the application of this.
		//if( m_nSelStart != m_nSelected );
		//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );

		//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION_END, true, this );
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Dragged
//-----------------------------------------------------------------------------
bool ListBoxUI::Dragged(Point pt)
{
	if( m_bDrag )
	{
		// Compute the index of the item below cursor

		int nItem;
		if( m_nTextHeight )
			nItem = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
		else
			nItem = -1;

		// Only proceed if the cursor is on top of an item.

		if( nItem >= ( int )m_ScrollBar.GetTrackPos() &&
			nItem < ( int )m_Items.size() &&
			nItem < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
		{
			m_nSelected = nItem;
			//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
		}
		else if( nItem < ( int )m_ScrollBar.GetTrackPos() )
		{
			// User drags the mouse above window top
			m_ScrollBar.Scroll( -1 );
			m_nSelected = m_ScrollBar.GetTrackPos();
			//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
		}
		else if( nItem >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
		{
			// User drags the mouse below window bottom
			m_ScrollBar.Scroll( 1 );
            m_nSelected = std::min( ( int )m_Items.size(), m_ScrollBar.GetTrackPos() +
				m_ScrollBar.GetPageSize() ) - 1;
			//m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : Scrolled
//-----------------------------------------------------------------------------
bool ListBoxUI::Scrolled( int nScrollAmount)
{
	if (m_bMouseOver)
	{	
		m_ScrollBar.Scroll( -nScrollAmount );
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Name : ConnectToItemDBLCK
//-----------------------------------------------------------------------------
void ListBoxUI::ConnectToItemDBLCK(const signal_listbox::slot_type& subscriber)
{
	m_itemDBLCLKSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : ConnectToListboxSel
//-----------------------------------------------------------------------------
void ListBoxUI::ConnectToListboxSel(const signal_listbox::slot_type& subscriber)
{
	m_listboxSelSig.connect(subscriber);
}


//-----------------------------------------------------------------------------
// Name : Render
//-----------------------------------------------------------------------------
void ListBoxUI::Render(Sprite& sprite, Sprite& textSprite, double timeStamp)
{
	if( m_bVisible == false )
		return;
	
    Point dialogPos = m_pParentDialog->getLocation();
    long  dialogCaptionHeihgt =  m_pParentDialog->getCaptionHeight();
	dialogPos.y += dialogCaptionHeihgt;

    renderRect(sprite, m_rcBoundingBox, m_elementsGFX[0].iTexture, m_elementsGFX[0].rcTexture, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), dialogPos);

	// Render the text
	if( m_Items.size() > 0 )
	{
		// Find out the height of a single line of text
        Rect rc = m_rcText;
        Rect rcSel = m_rcSelection;
        rc.bottom = rc.top + m_elementsFonts[0].font->getFontSize();

		// Update the line height formation
		m_nTextHeight = rc.bottom - rc.top;

		rc.right = m_rcText.right;
		for( int i = m_ScrollBar.GetTrackPos(); i < ( int )m_Items.size(); ++i )
		{
			if( rc.bottom > m_rcText.bottom )
				break;

			ListBoxItemUI* pItem = m_Items[i];
            Point strDim = m_elementsFonts[0].font->calcTextRect(pItem->strText);
            rc.bottom = rc.top + strDim.y;

			// Determine if we need to render this item with the
			// selected element.
			bool bSelectedStyle = false;

            if( !( m_dwStyle == MULTISELECTION ) && i == m_nSelected )
				bSelectedStyle = true;
            else if( m_dwStyle == MULTISELECTION )
			{
				if( m_bDrag &&
					( ( i >= m_nSelected && i < m_nSelStart ) ||
					( i <= m_nSelected && i > m_nSelStart ) ) )
					bSelectedStyle = m_Items[m_nSelStart]->bSelected;
				else if( pItem->bSelected )
					bSelectedStyle = true;
			}

			if( bSelectedStyle )
			{
				rcSel.top = rc.top; rcSel.bottom = rc.bottom;
                renderRect(sprite, rcSel, m_elementsGFX[1].iTexture, m_elementsGFX[1].rcTexture, glm::vec4(1.0f, 1.0f, 1.0f,1.0f), dialogPos);
                if (m_elementsFonts.size() > 0)
                    renderText(textSprite, m_elementsFonts[0].font, pItem->strText, WHITE_COLOR, rc, dialogPos);
			}
			else
                if (m_elementsFonts.size() > 0)
                    renderText(textSprite, m_elementsFonts[0].font, pItem->strText, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), rc, dialogPos);

            rc.offset(0, m_nTextHeight);
		}
	}

	// Render the scroll bar
    m_ScrollBar.Render(sprite, textSprite, timeStamp);
}

//-----------------------------------------------------------------------------
// Name : UpdateRects
//-----------------------------------------------------------------------------
void ListBoxUI::UpdateRects()
{
    ControlUI::UpdateRects();

	m_rcSelection = m_rcBoundingBox;
	m_rcSelection.right -= m_nSBWidth;
    m_rcSelection.inflate(-m_nBorder, m_nBorder);
	m_rcText = m_rcSelection;
    m_rcText.inflate(-m_nMargin, 0);

	// Update the scrollbar's rects
	m_ScrollBar.setLocation( m_rcBoundingBox.right - m_nSBWidth, m_rcBoundingBox.top );
	m_ScrollBar.setSize( m_nSBWidth, m_height );

	if(m_elementsFonts.size() > 0 )
	{
        m_ScrollBar.SetPageSize( ( m_rcText.bottom - m_rcText.top ) /  m_elementsFonts[0].font->getFontSize() );

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem( m_nSelected );
	}
}

//-----------------------------------------------------------------------------
// Name : GetStyle
//-----------------------------------------------------------------------------
GLuint ListBoxUI::GetStyle() const
{
	return m_dwStyle;
}

//-----------------------------------------------------------------------------
// Name : SetStyle
//-----------------------------------------------------------------------------
void ListBoxUI::SetStyle( GLuint dwStyle )
{
	m_dwStyle = dwStyle;
}

//-----------------------------------------------------------------------------
// Name : GetSize
//-----------------------------------------------------------------------------
int ListBoxUI::GetSize() const
{
	return m_Items.size();
}

//-----------------------------------------------------------------------------
// Name : GetScrollBarWidth
//-----------------------------------------------------------------------------
int ListBoxUI::GetScrollBarWidth() const
{
	return m_nSBWidth;
}

//-----------------------------------------------------------------------------
// Name : SetScrollBarWidth
//-----------------------------------------------------------------------------
void ListBoxUI::SetScrollBarWidth( int nWidth )
{
	m_nSBWidth = nWidth;
	UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : SetBorder
//-----------------------------------------------------------------------------
void ListBoxUI::SetBorder( int nBorder, int nMargin )
{
	m_nBorder = nBorder;
	m_nMargin = nMargin;
}

//-----------------------------------------------------------------------------
// Name : AddItem
//-----------------------------------------------------------------------------
bool ListBoxUI::AddItem(std::string strText, void* pData)
{
	ListBoxItemUI* pNewItem = new ListBoxItemUI();
	if (!pNewItem)
        return false;

    pNewItem->strText = strText;
	pNewItem->pData = pData;
    pNewItem->rcActive = Rect(0,0,0,0);
	pNewItem->bSelected = false;

    //TODO: add a check that will catch the exception in a case push_back fails!
	m_Items.push_back(pNewItem);

	m_ScrollBar.SetTrackRange( 0, m_Items.size());

    return true;
}

//-----------------------------------------------------------------------------
// Name : InsertItem
//-----------------------------------------------------------------------------
bool ListBoxUI::InsertItem( int nIndex, std::string& strText, void* pData )
{
	ListBoxItemUI* pNewItem = new ListBoxItemUI();
	if (!pNewItem)
        return false;

    pNewItem->strText = strText;
	pNewItem->pData = pData;
    pNewItem->rcActive = Rect(0,0,0,0);
	pNewItem->bSelected = false;

	//TODO: add something that will catch the exception
	m_Items.insert(m_Items.begin() + nIndex, pNewItem );

	m_ScrollBar.SetTrackRange( 0, m_Items.size() );

    return true;
}

//-----------------------------------------------------------------------------
// Name : RemoveItem
//-----------------------------------------------------------------------------
void ListBoxUI::RemoveItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= ( int )m_Items.size() )
		return;

	m_Items.erase(m_Items.begin() + nIndex);

	m_ScrollBar.SetTrackRange( 0, m_Items.size() );
	if( m_nSelected >= ( int )m_Items.size() )
		m_nSelected = m_Items.size() - 1;

    //m_pParentDialog->SendEvent(5, false, m_ID, NULL);
}

//TODO: decide if I really need this function as I don't really use data here....
//-----------------------------------------------------------------------------
// Name : RemoveItemByData
//-----------------------------------------------------------------------------
void ListBoxUI::RemoveItemByData( void* pData )
{
}

//-----------------------------------------------------------------------------
// Name : RemoveAllItems
//-----------------------------------------------------------------------------
void ListBoxUI::RemoveAllItems()
{
	m_Items.clear();
	m_ScrollBar.SetTrackRange( 0, 1 );
}

//-----------------------------------------------------------------------------
// Name : GetItem
//-----------------------------------------------------------------------------
ListBoxItemUI* ListBoxUI::GetItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= ( int )m_Items.size() )
		return NULL;

	return m_Items[nIndex];
}

//-----------------------------------------------------------------------------
// Name : GetSelectedIndex
// Desc : For single-selection listbox, returns the index of the selected item.
// For multi-selection, returns the first selected item after the nPreviousSelected position.
// To search for the first selected item, the app passes -1 for nPreviousSelected.  For
// subsequent searches, the app passes the returned index back to GetSelectedIndex as.
// nPreviousSelected.
// Returns -1 on error or if no item is selected.
//-----------------------------------------------------------------------------
int ListBoxUI::GetSelectedIndex( int nPreviousSelected/*  = -1 */)
{
	if( nPreviousSelected < -1 )
		return -1;

    if( m_dwStyle == MULTISELECTION )
	{
		// Multiple selection enabled. Search for the next item with the selected flag.
		for( int i = nPreviousSelected + 1; i < ( int )m_Items.size(); ++i )
		{
			ListBoxItemUI* pItem = m_Items[i];

			if( pItem->bSelected )
				return i;
		}

		return -1;
	}
	else
	{
		// Single selection
		return m_nSelected;
	}
}

//-----------------------------------------------------------------------------
// Name : GetSelectedItem
//-----------------------------------------------------------------------------
ListBoxItemUI* ListBoxUI::GetSelectedItem( int nPreviousSelected/* = -1 */)
{
	return GetItem( GetSelectedIndex( nPreviousSelected ) );
}

//-----------------------------------------------------------------------------
// Name : SelectItem
//-----------------------------------------------------------------------------
void ListBoxUI::SelectItem( int nNewIndex )
{
	// If no item exists, do nothing.
	if( m_Items.size() == 0 )
		return;

	int nOldSelected = m_nSelected;

	// Adjust m_nSelected
	m_nSelected = nNewIndex;

	// Perform capping
	if( m_nSelected < 0 )
		m_nSelected = 0;
	if( m_nSelected >= ( int )m_Items.size() )
		m_nSelected = m_Items.size() - 1;

	if( nOldSelected != m_nSelected )
	{
        if( m_dwStyle == MULTISELECTION )
		{
			m_Items[m_nSelected]->bSelected = true;
		}

		// Update selection start
		m_nSelStart = m_nSelected;

		// Adjust scroll bar
		m_ScrollBar.ShowItem( m_nSelected );
	}

//	m_pParentDialog->SendEvent(5, false, m_ID, false);
}

//-----------------------------------------------------------------------------
// Name : CanHaveFocus
//-----------------------------------------------------------------------------
bool ListBoxUI::CanHaveFocus()
{
	return ( m_bVisible && m_bEnabled );
}

//-----------------------------------------------------------------------------
// Name : SaveToFile
//-----------------------------------------------------------------------------
bool ListBoxUI::SaveToFile(std::ostream& SaveFile)
{
    ControlUI::SaveToFile(SaveFile);

	SaveFile << m_nSBWidth << "| ListBox SBWidth" << "\n";
	SaveFile << m_nBorder << "| ListBox Border" << "\n";
	SaveFile << m_nMargin << "| ListBox Margin" << "\n";
	SaveFile << m_nTextHeight << "| ListBox Text Height" << "\n";
	SaveFile << m_dwStyle << "| ListBox Style" << "\n";

	SaveFile << m_Items.size() << "| ListBox Item size" << "\n";

    for (GLuint i = 0; i < m_Items.size(); i++)
	{
		SaveFile << m_Items[i]->strText << "| ListBox Item " << i << " Text" << "\n";
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name : CopyItemsFrom
//-----------------------------------------------------------------------------
void ListBoxUI::CopyItemsFrom(ListBoxUI *sourceListBox)
{
	// clears the items vector
	RemoveAllItems();

	for (int i = 0; i < sourceListBox->GetSize(); i++)
	{
		AddItem( sourceListBox->GetItem(i)->strText, sourceListBox->GetItem(i)->pData );
	}
} 
