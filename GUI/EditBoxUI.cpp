#include "EditBoxUI.h"
#include "DialogUI.h"
#include "../GameWin.h"
#include <iostream>

bool EditBoxUI::s_bHideCaret;
int EditBoxUI::s_caretBlinkTime = 100;

//TODO: make scrolling and choosing a char work accurate
//TDOO: make m_nFirstVisible more accurate
//TODO: find a better and faster way to tell the text width

//-----------------------------------------------------------------------------
// Name : CEditBoxUI(constructor) 
//-----------------------------------------------------------------------------
EditBoxUI::EditBoxUI(DialogUI* pParentDialog, int ID, std::string strText, int x, int y, int width, int height)
    :ControlUI(pParentDialog, ID, x, y, width, height)
{
    m_type = ControlUI::EDITBOX;

    m_buffer = strText;

	m_nBorder = 5;  // Default border width
	m_nSpacing = 4;  // Default spacing
    m_nVisibleChars = m_buffer.size();
	m_nFirstVisible = 0;
	m_nBackwardChars = 0;

	m_bCaretOn = true;
    m_dfBlink = s_caretBlinkTime * 0.001f;
    m_dfLastBlink = 0;
	s_bHideCaret = false;

    m_TextColor = glm::vec4(0.07f, 0.07f, 0.07f, 1.0);
    m_SelTextColor = WHITE_COLOR;
    m_SelBkColor = glm::vec4(0.16f, 0.2f, 0.36f, 1.0);
    m_CaretColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_nCaret = m_nSelStart = 0;
	m_bInsertMode = true;

	m_bMouseDrag = false;

    //m_assetManger = false;
}

//-----------------------------------------------------------------------------
// Name : CEditBoxUI(constructor from InputFile)
//-----------------------------------------------------------------------------
EditBoxUI::EditBoxUI(std::istream& inputFile)
    :ControlUI(inputFile)
{
    m_type = ControlUI::EDITBOX;

    m_dfBlink = s_caretBlinkTime * 0.001f;
    m_dfLastBlink = 0;
	s_bHideCaret = false;
	m_nCaret = m_nSelStart = 0;
	m_bInsertMode = true;
	m_nFirstVisible = 0;
	m_nBackwardChars = 0;

	m_bMouseDrag = false;

	std::string bufferText;
	std::getline(inputFile, bufferText);
	bufferText = bufferText.substr(0, bufferText.find('|') );
	SetText(bufferText.c_str() );
    m_nVisibleChars = m_buffer.size();
	inputFile >> m_nBorder;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nSpacing;
	SetSpacing(m_nSpacing);
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_bCaretOn;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
//	inputFile >> m_TextColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
//	inputFile >> m_SelTextColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
//	inputFile >> m_SelBkColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
//	inputFile >> m_CaretColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
}

//-----------------------------------------------------------------------------
// Name : CEditBoxUI(destructor) 
//-----------------------------------------------------------------------------
EditBoxUI::~EditBoxUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : handleKeyEvent()
//-----------------------------------------------------------------------------
bool EditBoxUI::handleKeyEvent(unsigned char key , bool down)
{
    if( !m_bEnabled || !m_bVisible || !down )
        return false;

    switch(key)
    {

    case 8: // backsapce
    {
        // If there's a selection, treat this
        // like a delete key.
        if( m_nCaret != m_nSelStart )
        {
            DeleteSelectionText();
            m_editboxChangedSig(this);
        }
        else if( m_nCaret > 0 )
        {
            // Move the caret, then delete the char.
            PlaceCaret( m_nCaret - 1 );
            m_nSelStart = m_nCaret;
            m_buffer.erase(m_nCaret,1);

            if (m_nFirstVisible > 0)
                CalcFirstVisibleCharDown();

            m_nVisibleChars = m_buffer.size() - m_nFirstVisible;

            m_editboxChangedSig(this);
        }
        ResetCaretBlink();
        return true;
    }break;

    case 127: // delete
    {
        // Check if there is a text selection.
        if( m_nCaret != m_nSelStart )
        {
            DeleteSelectionText();
            m_editboxChangedSig(this);
        }
        else
        {
            // Deleting one character
            m_buffer.erase(m_nCaret,1);
            m_editboxChangedSig(this);
        }
        ResetCaretBlink();
        return true;
    }break;

    case 24: // Ctrl-X Cut
    case 3:  // Ctrl-C Copy
    {
        CopyToClipboard();

        // If the key is Ctrl-X, delete the selection too.
        if( key == 24 )
        {
            DeleteSelectionText();
            m_editboxChangedSig(this);
        }
        break;
    }

    // Ctrl-V Paste
    case 22:
    {
        PasteFromClipboard();
        m_editboxChangedSig(this);
        break;
    }

    // Junk characters we don't want in the string
    case 26:  // Ctrl Z
    case 2:   // Ctrl B
    case 14:  // Ctrl N
    case 19:  // Ctrl S
    case 4:   // Ctrl D
    case 6:   // Ctrl F
    case 7:   // Ctrl G
    case 10:  // Ctrl J
    case 11:  // Ctrl K
    case 12:  // Ctrl L
    case 17:  // Ctrl Q
    case 23:  // Ctrl W
    case 5:   // Ctrl E
    case 18:  // Ctrl R
    case 20:  // Ctrl T
    case 25:  // Ctrl Y
    case 21:  // Ctrl U
    case 9:   // Ctrl I
    case 15:  // Ctrl O
    case 16:  // Ctrl P
    case 27:  // Ctrl [
    case 29:  // Ctrl ]
    case 28:  // Ctrl \
        break;

    default:
    {
        // If there's a selection and the user
        // starts to type, the selection should
        // be deleted.
        if( m_nCaret != m_nSelStart )
            DeleteSelectionText();

        // If we are in overwrite mode and there is already
        // a char at the caret's position, simply replace it.
        // Otherwise, we insert the char as normal.
        if( !m_bInsertMode && m_nCaret < m_buffer.size() )
        {
            m_buffer[m_nCaret] = key;
            PlaceCaret( m_nCaret + 1 );
            m_nSelStart = m_nCaret;

            CalcFirstVisibleCharUp();
        }
        else
        {
            // Insert the char
            m_buffer.insert(m_nCaret, 1, key);

            PlaceCaret( m_nCaret + 1 );
            m_nSelStart = m_nCaret;

            CalcFirstVisibleCharUp();
        }

        ResetCaretBlink();
        m_editboxChangedSig(this);
        return true;
    }


    }

//    switch( uMsg )
//    {
//    // Make sure that while editing, the keyup and keydown messages associated with
//    // WM_CHAR messages don't go to any non-focused controls or cameras
//    // TODO: Fix the problem that this case flashes the delete button messages..
//    // possible solution is to just put the delete thingy here instead in Handlekeyboard
//    case WM_KEYUP:
//    case WM_KEYDOWN:
//        return false;

//    case WM_CHAR:
//    {
//        switch( ( char )wParam )
//        {
//        // Backspace
//        case VK_BACK:
//        {
//            // If there's a selection, treat this
//            // like a delete key.
//            if( m_nCaret != m_nSelStart )
//            {
//                DeleteSelectionText();
//                m_editboxChangedSig(this);
//                //m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
//            }
//            else if( m_nCaret > 0 )
//            {
//                // Move the caret, then delete the char.
//                PlaceCaret( m_nCaret - 1 );
//                m_nSelStart = m_nCaret;
//                m_Buffer.erase(m_nCaret,1);

//                if (m_nFirstVisible > 0)
//                    CalcFirstVisibleCharDown();

//                m_nVisibleChars = m_Buffer.size() - m_nFirstVisible;

//                m_editboxChangedSig(this);
//                //m_Buffer.RemoveChar( m_nCaret );
//                //m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
//            }
//            ResetCaretBlink();
//            break;
//        }

//        case 24:        // Ctrl-X Cut
//        case VK_CANCEL: // Ctrl-C Copy
//        {
//            CopyToClipboard();

//            // If the key is Ctrl-X, delete the selection too.
//            if( ( char )wParam == 24 )
//            {
//                DeleteSelectionText();
//                m_editboxChangedSig(this);
//                //m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
//            }

//            break;
//        }

//            // Ctrl-V Paste
//        case 22:
//        {
//            PasteFromClipboard();
//            m_editboxChangedSig(this);
//            //m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
//            break;
//        }

//            // Ctrl-A Select All
//        case 1:
//            if( m_nSelStart == m_nCaret )
//            {
//                m_nSelStart = 0;
//                PlaceCaret( m_Buffer.size() );
//            }
//            break;

//        case VK_RETURN:
//            // Invoke the callback when the user presses Enter.
//            //m_pDialog->SendEvent( EVENT_EDITBOX_STRING, true, this );
//            break;

//            // Junk characters we don't want in the string
//        case 26:  // Ctrl Z
//        case 2:   // Ctrl B
//        case 14:  // Ctrl N
//        case 19:  // Ctrl S
//        case 4:   // Ctrl D
//        case 6:   // Ctrl F
//        case 7:   // Ctrl G
//        case 10:  // Ctrl J
//        case 11:  // Ctrl K
//        case 12:  // Ctrl L
//        case 17:  // Ctrl Q
//        case 23:  // Ctrl W
//        case 5:   // Ctrl E
//        case 18:  // Ctrl R
//        case 20:  // Ctrl T
//        case 25:  // Ctrl Y
//        case 21:  // Ctrl U
//        case 9:   // Ctrl I
//        case 15:  // Ctrl O
//        case 16:  // Ctrl P
//        case 27:  // Ctrl [
//        case 29:  // Ctrl ]
//        case 28:  // Ctrl \
//            break;

//        default:
//        {
//            // If there's a selection and the user
//            // starts to type, the selection should
//            // be deleted.
//            if( m_nCaret != m_nSelStart )
//                DeleteSelectionText();

//            // If we are in overwrite mode and there is already
//            // a char at the caret's position, simply replace it.
//            // Otherwise, we insert the char as normal.
//            if( !m_bInsertMode && m_nCaret < m_Buffer.size() )
//            {
//                m_Buffer[m_nCaret] = ( char )wParam;
//                PlaceCaret( m_nCaret + 1 );
//                m_nSelStart = m_nCaret;

//                LPD3DXFONT pFont = m_assetManger->getFontPtr(2);
//                RECT rt = {0,0,0,0};

//                CalcFirstVisibleCharUp();
//            }
//            else
//            {
//                // Insert the char
//                m_Buffer.insert(m_nCaret, 1, (char)wParam);

//                PlaceCaret( m_nCaret + 1 );
//                m_nSelStart = m_nCaret;

//                LPD3DXFONT pFont = m_assetManger->getFontPtr(2);
//                RECT rt = {0,0,0,0};

//                CalcFirstVisibleCharUp();
//            }

//            ResetCaretBlink();
//            m_editboxChangedSig(this);
//        }
//        }
//        return true;
//    }
//    }
    return false;
}

//-----------------------------------------------------------------------------
// Name : handleVirtualKey()
//-----------------------------------------------------------------------------
bool EditBoxUI::handleVirtualKey(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates)
{
    if( !m_bEnabled || !m_bVisible || !down )
        return false;

    switch(virtualKey)
    {

    case GK_VirtualKey::GK_Home:
    {
        PlaceCaret( 0 );
        if( !modifierStates.bShift )
        {
            // Shift is not down. Update selection
            // start along with the caret.
            m_nSelStart = m_nCaret;
        }
        ResetCaretBlink();
        return true;
    }break;

    case GK_VirtualKey::GK_End:
    {
        PlaceCaret( m_buffer.size() );
        if( !modifierStates.bShift )
        {
            // Shift is not down. Update selection
            // start along with the caret.
            m_nSelStart = m_nCaret;
        }
        ResetCaretBlink();
        return true;
    }break;

    case GK_VirtualKey::GK_Insert:
    {
        if( modifierStates.bCtrl )
        {
            // Control Insert. Copy to clipboard
            //CopyToClipboard();
        }
        else if( modifierStates.bShift )
        {
            // Shift Insert. Paste from clipboard
            //PasteFromClipboard();
        }
        else
        {
            // Toggle caret insert mode
            m_bInsertMode = !m_bInsertMode;
        }
        return true;
    }break;

    case GK_VirtualKey::GK_Delete:
    {
        // Check if there is a text selection.
        if( m_nCaret != m_nSelStart )
        {
            DeleteSelectionText();
            m_editboxChangedSig(this);
        }
        else
        {
            // Deleting one character
            m_buffer.erase(m_nCaret,1);
            m_editboxChangedSig(this);
        }
        ResetCaretBlink();
        return true;
    }break;

    case GK_VirtualKey::GK_Left:
    {
        if( !modifierStates.bCtrl )
        {
            // Control is down. Move the caret to a new item
            // instead of a character.
            if (m_nCaret > 0)
            {
                m_nCaret--;
                if ( m_nCaret < m_nFirstVisible)
                {
                    if (m_nFirstVisible > 0 && m_nBackwardChars < m_nFirstVisible)
                        m_nBackwardChars++;
                }
                PlaceCaret( m_nCaret );
            }
        }
        else if( m_nCaret > 0 )
        {
            if ( m_nCaret - 1 < m_nFirstVisible)
            {
                if (m_nFirstVisible > 0 && m_nBackwardChars < m_nFirstVisible)
                    m_nBackwardChars++;
            }
            PlaceCaret( m_nCaret - 1 );
        }
        if( !modifierStates.bShift )
        {
            // Shift is not down. Update selection
            // start along with the caret.
            m_nSelStart = m_nCaret;
        }

        ResetCaretBlink();
        return true;
    }break;

    case GK_VirtualKey::GK_Right:
    {
        if( !modifierStates.bCtrl )
        {
            // Control is down. Move the caret to a new item
            // instead of a character.
            if( m_nCaret < m_buffer.size())
            {
                m_nCaret++;

                if ( (m_nCaret - (m_nFirstVisible - m_nBackwardChars) ) < m_nVisibleChars )
                {
                    if (m_nBackwardChars > 0)
                        m_nBackwardChars--;
                }
                PlaceCaret( m_nCaret );
            }
        }
        else if( m_nCaret < m_buffer.size() )
        {
            if ( (m_nCaret + 1 - ( m_nFirstVisible - m_nBackwardChars ) ) > m_nVisibleChars )
            {
                if (m_nBackwardChars > 0)
                    m_nBackwardChars--;
                PlaceCaret(m_nCaret + 1);
            }
            else
                PlaceCaret( m_nCaret + 1 );
        }
        if( !modifierStates.bShift )
        {
            // Shift is not down. Update selection
            // start along with the caret.
            m_nSelStart = m_nCaret;
        }

        ResetCaretBlink();
        return true;
    }break;

    default:
        return false;

    }

}

//-----------------------------------------------------------------------------
// Name : HandleKeyboard() 
//-----------------------------------------------------------------------------
//bool CEditBoxUI::HandleKeyboard( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
//{
//	if( !m_bEnabled || !m_bVisible )
//		return false;

//	bool bHandled = false;

//	switch( uMsg )
//	{
//	case WM_KEYDOWN:
//		{
//			switch( wParam )
//			{
//			case VK_TAB:
//				// We don't process Tab in case keyboard input is enabled and the user
//				// wishes to Tab to other controls.
//				break;

//			case VK_HOME:
//				PlaceCaret( 0 );
//				if( GetKeyState( VK_SHIFT ) >= 0 )
//					// Shift is not down. Update selection
//					// start along with the caret.
//					m_nSelStart = m_nCaret;
//				ResetCaretBlink();
//				bHandled = true;
//				break;

//			case VK_END:
//				PlaceCaret( m_Buffer.size() );
//				if( GetKeyState( VK_SHIFT ) >= 0 )
//					// Shift is not down. Update selection
//					// start along with the caret.
//					m_nSelStart = m_nCaret;
//				ResetCaretBlink();
//				bHandled = true;
//				break;

//			case VK_INSERT:
//				if( GetKeyState( VK_CONTROL ) < 0 )
//				{
//					// Control Insert. Copy to clipboard
//					CopyToClipboard();
//				}
//				else if( GetKeyState( VK_SHIFT ) < 0 )
//				{
//					// Shift Insert. Paste from clipboard
//					PasteFromClipboard();
//				}
//				else
//				{
//					// Toggle caret insert mode
//					m_bInsertMode = !m_bInsertMode;
//				}
//				break;

//			case VK_DELETE:
//				// Check if there is a text selection.
//				if( m_nCaret != m_nSelStart )
//				{
//					DeleteSelectionText();
//					m_editboxChangedSig(this);
//					//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
//				}
//				else
//				{
//					// Deleting one character
//					m_Buffer.erase(m_nCaret,1);
//					m_editboxChangedSig(this);
//					//if( m_Buffer.RemoveChar( m_nCaret ) )
//					//	m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
//				}
//				ResetCaretBlink();
//				bHandled = true;
//				break;

//			case VK_LEFT:
//				{
//					if( GetKeyState( VK_CONTROL ) < 0 )
//					{
//						// Control is down. Move the caret to a new item
//						// instead of a character.
//						m_nCaret--;
//						//m_Buffer.GetPriorItemPos( m_nCaret, &m_nCaret );
//						if ( m_nCaret < m_nFirstVisible)
//						{
//							if (m_nFirstVisible > 0 && m_nBackwardChars < m_nFirstVisible)
//								m_nBackwardChars++;
//							//m_nFirstVisible = m_nCaret;
//						}
//						PlaceCaret( m_nCaret );
//					}
//					else if( m_nCaret > 0 )
//					{
//						if ( m_nCaret - 1 < m_nFirstVisible)
//						{
//							if (m_nFirstVisible > 0 && m_nBackwardChars < m_nFirstVisible)
//								m_nBackwardChars++;
//							//m_nFirstVisible = m_nCaret - 1;
//						}
//						PlaceCaret( m_nCaret - 1 );
//					}
//					if( GetKeyState( VK_SHIFT ) >= 0 )
//						// Shift is not down. Update selection
//						// start along with the caret.
//						m_nSelStart = m_nCaret;
//					ResetCaretBlink();
//					bHandled = true;
//				}
//				break;

//			case VK_RIGHT:
//				if( GetKeyState( VK_CONTROL ) < 0 )
//				{
//					// Control is down. Move the caret to a new item
//					// instead of a character.
//					m_nCaret++;

//					if ( (m_nCaret - (m_nFirstVisible - m_nBackwardChars) ) < m_nVisibleChars )
//					{
//						if (m_nBackwardChars > 0)
//							m_nBackwardChars--;
//						//m_nCaret = m_nVisibleChars + m_nFirstVisible;
//					}
//					//m_Buffer.GetNextItemPos( m_nCaret, &m_nCaret );
//					PlaceCaret( m_nCaret );
//				}
//				else if( m_nCaret < m_Buffer.size() )
//				{
//					if ( (m_nCaret + 1 - ( m_nFirstVisible - m_nBackwardChars ) ) > m_nVisibleChars )
//					{
//						if (m_nBackwardChars > 0)
//							m_nBackwardChars--;
//						//m_nCaret = m_nVisibleChars + m_nFirstVisible;
//						PlaceCaret(m_nCaret + 1);
//					}
//					else
//						PlaceCaret( m_nCaret + 1 );
//				}
//				if( GetKeyState( VK_SHIFT ) >= 0 )
//					// Shift is not down. Update selection
//					// start along with the caret.
//					m_nSelStart = m_nCaret;
//				ResetCaretBlink();
//				bHandled = true;
//				break;

//			case VK_UP:
//			case VK_DOWN:
//				// Trap up and down arrows so that the dialog
//				// does not switch focus to another control.
//				bHandled = true;
//				break;

//			default:
//				bHandled = wParam != VK_ESCAPE;  // Let the application handle Esc.
//			}
//		}
//	}
//	return bHandled;
//}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent()
//-----------------------------------------------------------------------------
bool EditBoxUI::handleMouseEvent(MouseEvent event)
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch(event.type)
    {
    case MouseEventType::LeftButton:
    {
        if (event.down)
            return Pressed(event.cursorPos, INPUT_STATE(), event.timeStamp);
        else
            return Released(event.cursorPos);
    }break;

    case MouseEventType::MouseMoved:
    {
        if ( Dragged(event.cursorPos) )
            return true;
    }break;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed() 
//-----------------------------------------------------------------------------
bool EditBoxUI::Pressed(Point pt, INPUT_STATE inputState, double timeStamp)
{
	if( !ContainsPoint( pt ) )
		return false;

	if( !m_bHasFocus )
		m_pParentDialog->RequestFocus( this );

	m_bMouseDrag = true;
	// Determine the character corresponding to the coordinates.
 	int nCP;//, nTrail, nX1st;

	nCP = CalcCaretPosByPoint(pt);

    if (nCP > m_buffer.size())
        nCP = m_buffer.size();

	// Cap at the NULL character.
    if(nCP >= 0 && nCP < (m_buffer.size() + 1) )
		PlaceCaret( nCP );
	else
		PlaceCaret( 0 );
	m_nSelStart = m_nCaret;
	ResetCaretBlink();

	return true;
}

//-----------------------------------------------------------------------------
// Name : Released() 
//-----------------------------------------------------------------------------
bool EditBoxUI::Released( Point pt)
{
	m_bMouseDrag = false;

	return false;
}

//-----------------------------------------------------------------------------
// Name : Dragged() 
//-----------------------------------------------------------------------------
bool EditBoxUI::Dragged( Point pt)
{
	if( m_bMouseDrag )
	{
		// Determine the character corresponding to the coordinates.
		int nCP;//, nTrail, nX1st;
		nCP = CalcCaretPosByPoint(pt);

		// Cap at the NULL character.
        if(nCP >= 0 && nCP < m_buffer.size() )
		{

			if (m_nCaret < m_nFirstVisible)
				if (m_nFirstVisible > 0 && m_nBackwardChars < m_nFirstVisible)
				{
					m_nBackwardChars += m_nFirstVisible - m_nCaret;

					if (m_nBackwardChars > m_nFirstVisible)
						m_nBackwardChars = m_nFirstVisible;
				}

			if ( (m_nCaret + 1 - ( m_nFirstVisible - m_nBackwardChars ) ) > m_nVisibleChars )
				if (m_nBackwardChars > 0)
				{
					m_nBackwardChars -= m_nCaret - m_nVisibleChars;

					if (m_nBackwardChars < 0)
						m_nBackwardChars = 0;
				}
			
			PlaceCaret( nCP );

			if ( nCP > m_nCaret)
				CalcFirstVisibleCharUp();
			
			if ( nCP < m_nCaret)
				CalcFirstVisibleCharDown();
		}
		else
		{
            if (nCP < 0 || m_buffer.size() == 0)
			{
				nCP = 0;
			}

            if (nCP > m_buffer.size())
			{
                nCP = m_buffer.size();
				m_nBackwardChars = 0;
			}

			PlaceCaret( nCP );

			if ( nCP > m_nCaret)
				CalcFirstVisibleCharUp();

			if ( nCP < m_nCaret)
				CalcFirstVisibleCharDown();
		}
		return true;

	}

	return false;
}

//-----------------------------------------------------------------------------
// Name : connectToEditboxChg() 
//-----------------------------------------------------------------------------
void EditBoxUI::connectToEditboxChg(const signal_editbox::slot_type& subscriber)
{
	m_editboxChangedSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : MsgProc() 
//-----------------------------------------------------------------------------
bool EditBoxUI::MsgProc()
{
//	if( !m_bEnabled || !m_bVisible )
//		return false;

//	switch( uMsg )
//	{
//		// Make sure that while editing, the keyup and keydown messages associated with
//		// WM_CHAR messages don't go to any non-focused controls or cameras
//		// TODO: Fix the problem that this case flashes the delete button messages..
//		// possible solution is to just put the delete thingy here instead in Handlekeyboard
//	case WM_KEYUP:
//	case WM_KEYDOWN:
//		return false;

//	case WM_CHAR:
//		{
//			switch( ( char )wParam )
//			{
//				// Backspace
//			case VK_BACK:
//				{
//					// If there's a selection, treat this
//					// like a delete key.
//					if( m_nCaret != m_nSelStart )
//					{
//						DeleteSelectionText();
//						m_editboxChangedSig(this);
//						//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
//					}
//					else if( m_nCaret > 0 )
//					{
//						// Move the caret, then delete the char.
//						PlaceCaret( m_nCaret - 1 );
//						m_nSelStart = m_nCaret;
//						m_Buffer.erase(m_nCaret,1);

//						if (m_nFirstVisible > 0)
//							CalcFirstVisibleCharDown();

//						m_nVisibleChars = m_Buffer.size() - m_nFirstVisible;

//						m_editboxChangedSig(this);
//						//m_Buffer.RemoveChar( m_nCaret );
//						//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
//					}
//					ResetCaretBlink();
//					break;
//				}

//			case 24:        // Ctrl-X Cut
//			case VK_CANCEL: // Ctrl-C Copy
//				{
//					CopyToClipboard();

//					// If the key is Ctrl-X, delete the selection too.
//					if( ( char )wParam == 24 )
//					{
//						DeleteSelectionText();
//						m_editboxChangedSig(this);
//						//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
//					}

//					break;
//				}

//				// Ctrl-V Paste
//			case 22:
//				{
//					PasteFromClipboard();
//					m_editboxChangedSig(this);
//					//m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
//					break;
//				}

//				// Ctrl-A Select All
//			case 1:
//				if( m_nSelStart == m_nCaret )
//				{
//					m_nSelStart = 0;
//					PlaceCaret( m_Buffer.size() );
//				}
//				break;

//			case VK_RETURN:
//				// Invoke the callback when the user presses Enter.
//				//m_pDialog->SendEvent( EVENT_EDITBOX_STRING, true, this );
//				break;

//				// Junk characters we don't want in the string
//			case 26:  // Ctrl Z
//			case 2:   // Ctrl B
//			case 14:  // Ctrl N
//			case 19:  // Ctrl S
//			case 4:   // Ctrl D
//			case 6:   // Ctrl F
//			case 7:   // Ctrl G
//			case 10:  // Ctrl J
//			case 11:  // Ctrl K
//			case 12:  // Ctrl L
//			case 17:  // Ctrl Q
//			case 23:  // Ctrl W
//			case 5:   // Ctrl E
//			case 18:  // Ctrl R
//			case 20:  // Ctrl T
//			case 25:  // Ctrl Y
//			case 21:  // Ctrl U
//			case 9:   // Ctrl I
//			case 15:  // Ctrl O
//			case 16:  // Ctrl P
//			case 27:  // Ctrl [
//			case 29:  // Ctrl ]
//			case 28:  // Ctrl \
//				break;

//			default:
//				{
//					// If there's a selection and the user
//					// starts to type, the selection should
//					// be deleted.
//					if( m_nCaret != m_nSelStart )
//						DeleteSelectionText();

//					// If we are in overwrite mode and there is already
//					// a char at the caret's position, simply replace it.
//					// Otherwise, we insert the char as normal.
//					if( !m_bInsertMode && m_nCaret < m_Buffer.size() )
//					{
//						m_Buffer[m_nCaret] = ( char )wParam;
//						PlaceCaret( m_nCaret + 1 );
//						m_nSelStart = m_nCaret;

//						LPD3DXFONT pFont = m_assetManger->getFontPtr(2);
//						RECT rt = {0,0,0,0};
						
//						CalcFirstVisibleCharUp();
//					}
//					else
//					{
//						// Insert the char
//						m_Buffer.insert(m_nCaret, 1, (char)wParam);

//                        PlaceCaret( m_nCaret + 1 );
//                        m_nSelStart = m_nCaret;

//                        LPD3DXFONT pFont = m_assetManger->getFontPtr(2);
//                        RECT rt = {0,0,0,0};

//                        CalcFirstVisibleCharUp();
//					}

//					ResetCaretBlink();
//					m_editboxChangedSig(this);
//				}
//			}
//			return true;
//		}
//	}
	return false;
}

//-----------------------------------------------------------------------------
// Name : UpdateRects() 
//-----------------------------------------------------------------------------
void EditBoxUI::UpdateRects()
{
    ControlUI::UpdateRects();

	// Update the text rectangle
	m_rcText = m_rcBoundingBox;
	// First inflate by m_nBorder to compute render rects
    m_rcText.inflate(-m_nBorder, -m_nBorder);

	// Update the render rectangles
    m_rcRender[0] = m_rcText; // text area texture rect
    m_rcRender[1] = Rect( m_rcBoundingBox.left, m_rcBoundingBox.top, m_rcText.left, m_rcText.top);        // top left border texture
    m_rcRender[2] = Rect(m_rcText.left, m_rcBoundingBox.top, m_rcText.right, m_rcText.top);               // top border
    m_rcRender[3] = Rect(m_rcText.right, m_rcBoundingBox.top, m_rcBoundingBox.right, m_rcText.top);       // top right border
    m_rcRender[4] = Rect(m_rcBoundingBox.left, m_rcText.top, m_rcText.left, m_rcText.bottom);             // left border
    m_rcRender[5] = Rect(m_rcText.right, m_rcText.top, m_rcBoundingBox.right, m_rcText.bottom);           // right border
    m_rcRender[6] = Rect(m_rcBoundingBox.left, m_rcText.bottom, m_rcText.left, m_rcBoundingBox.bottom);   // lower left border
    m_rcRender[7] = Rect(m_rcText.left, m_rcText.bottom, m_rcText.right, m_rcBoundingBox.bottom);         // lower border
    m_rcRender[8] = Rect(m_rcText.right, m_rcText.bottom, m_rcBoundingBox.right, m_rcBoundingBox.bottom); // lower right border

	// Inflate further by m_nSpacing
    m_rcText.inflate(-m_nSpacing, -m_nSpacing);
}

//-----------------------------------------------------------------------------
// Name : Render() 
//-----------------------------------------------------------------------------
void EditBoxUI::Render(Sprite& sprite, Sprite& textSprite, double timeStamp)
{
    if( m_bVisible == false || m_elementsFonts.size() == 0)
		return;

    Point dialogPos =  m_pParentDialog->getLocation();
    long  dialogCaptionHeihgt =  m_pParentDialog->getCaptionHeight();
	dialogPos.y += dialogCaptionHeihgt;

	int nSelStartX = 0, nCaretX = 0;  // Left and right X cordinates of the selection region

	// Render the control graphics
    for(GLuint i = 0; i < m_elementsGFX.size(); ++i)
	{
        renderRect(sprite, m_rcRender[i], m_elementsGFX[i].iTexture, m_elementsGFX[i].rcTexture, WHITE_COLOR, dialogPos);
	}

	//
	// Compute the X coordinates of the first visible character.
	//
    int nXFirst = m_rcText.left + m_elementsFonts[0].fontInfo.fontSize;;
	// compute the x coordinates of the caret
    nCaretX = m_rcText.left + m_elementsFonts[0].fontInfo.fontSize; +
              m_nCaret * m_elementsFonts[0].fontInfo.fontSize;;

	if (nCaretX > m_rcText.right)
		      ;//m_nFirstVisible = (nCaretX - m_rcText.right) / fontItem.avgWidth;

    nCaretX = m_rcText.right - m_elementsFonts[0].fontInfo.fontSize;;

	//
	// Compute the X coordinates of the selection rectangle
	//

	if (m_nCaret != m_nSelStart)
        nSelStartX = nXFirst + m_nSelStart * m_elementsFonts[0].fontInfo.fontSize;
	else
		nSelStartX = nCaretX;

	//
	// Render the selection rectangle
	//
    Rect rcSelection;  // Make this available for rendering selected text
	if( m_nCaret != m_nSelStart )
	{
		int nSelLeftX = nCaretX, nSelRightX = nSelStartX;
		// Swap if left is bigger than right
		if( nSelLeftX > nSelRightX )
		{
			int nTemp = nSelLeftX;
			nSelLeftX = nSelRightX;
			nSelRightX = nTemp;
		}
		
        int nFirstToRender = std::max( m_nFirstVisible - m_nBackwardChars, std::min( m_nSelStart, m_nCaret ) );
        int nNumChatToRender = std::max( m_nSelStart, m_nCaret ) - nFirstToRender;

        std::string temp = m_buffer.substr( nFirstToRender, nNumChatToRender);
		if (temp.size() > 0 && temp[0] == ' ')
			temp[0] = ';';
		if (temp.size() > 0 && temp[temp.size() -1] == ' ')
			temp [temp.size() - 1] = ';';
		
        Rect rcSelectionBound = {0, 0, 0, 0};
        Rect rcSelectX = {0, 0, 0, 0};
        Point textSize = m_elementsFonts[0].font->calcTextRect(temp);

        temp = m_buffer.substr(0, nFirstToRender);
		if (temp.size() > 0 && temp[0] == ' ')
			temp[0] = ';';
		if (temp.size() > 0 && temp[temp.size() -1] == ' ')
			temp [temp.size() - 1] = ';';

        Point textSelectSize = m_elementsFonts[0].font->calcTextRect(temp);

        rcSelection = Rect(nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom);
        rcSelection.offset(m_rcText.left - nXFirst, 0);

        rcSelection = rcSelection.intersectRect(rcSelection, m_rcText);

        Rect rtFullText = {0, 0, 0, 0};
        Point fullTextSize = m_elementsFonts[0].font->calcTextRect(m_buffer);
 
 		if (m_nFirstVisible - m_nBackwardChars != 0 )
 		{
            int rtExtra = fullTextSize.x - rcSelectX.right;
 			rcSelectX.right = (m_rcText.right - m_rcText.left) - rtExtra;
 		}

        nSelLeftX = m_rcText.left + textSelectSize.x + 0;
        nSelRightX = m_rcText.left + textSelectSize.x + textSize.x;

        int fontSize = m_elementsFonts[0].font->getFontSize();
        int nSelTop = m_rcText.top + m_rcText.getHeight()/2 - fontSize;
        int nSelBottom = m_rcText.top + m_rcText.getHeight()/2 + fontSize;
        rcSelection = Rect(nSelLeftX, std::max(m_rcText.top, nSelTop), nSelRightX, std::min(m_rcText.bottom, nSelBottom));

        Rect rc;
        renderRect(sprite, rcSelection, 0, rc, m_SelBkColor, dialogPos);
	}

	//
	// Render the text
	//
	// Element 0 for text
    Rect rt = {0, 0, 0, 0};
    Rect rtFullText = {0, 0, 0, 0};

    std::string temp = m_buffer.substr(0, m_nCaret);
    std::string fullText = m_buffer.substr(0, m_buffer.size() - m_nBackwardChars);


	std::string textToRender;

	if (m_nBackwardChars == 0)
        textToRender = m_buffer.substr(m_nFirstVisible, m_buffer.size() - m_nFirstVisible );
	else
        textToRender = m_buffer.substr(m_nFirstVisible - m_nBackwardChars, m_buffer.size() - (m_nFirstVisible /*- m_nBackwardChars * 2*/) );

	if (m_nFirstVisible - m_nBackwardChars == 0)
        renderText(textSprite, m_elementsFonts[0].font, textToRender, m_TextColor, m_rcText, dialogPos, mkFont::TextFormat::VerticalCenter);
	else
        renderText(textSprite, m_elementsFonts[0].font, textToRender, m_TextColor, m_rcText, dialogPos, mkFont::TextFormat::RightVerticalCenter);

	if (temp.size() > 0 && temp[temp.size() - 1] == ' ')
		temp[temp.size() - 1] = ';';

    Point rtSize =  m_elementsFonts[0].font->calcTextRect(temp);
    Point fullTextSize = m_elementsFonts[0].font->calcTextRect(fullText);

	if (m_nFirstVisible - m_nBackwardChars != 0)
	{
        int rtExtra = fullTextSize.x - rtSize.x;
        rtSize.x = (m_rcText.right - m_rcText.left) - rtExtra;
	}

	// Render the selected text
	if( m_nCaret != m_nSelStart )
	{
        int nFirstToRender = std::min(m_nSelStart, m_nCaret);
        int nNumChatToRender = std::max( m_nSelStart, m_nCaret ) - nFirstToRender;

        temp = m_buffer.substr( nFirstToRender, nNumChatToRender  + m_nBackwardChars);
        renderText(textSprite, m_elementsFonts[0].font, temp, m_SelTextColor, rcSelection, dialogPos, mkFont::TextFormat::VerticalCenter);
	}

	// Blink the caret
    if( timeStamp - m_dfLastBlink >= m_dfBlink )
	{
		m_bCaretOn = !m_bCaretOn;
        m_dfLastBlink = timeStamp;
	}

	// Render the caret if this control has the focus
	if( m_bHasFocus && m_bCaretOn && !s_bHideCaret )
	{
		// Start the rectangle with insert mode caret
        int fontSize = m_elementsFonts[0].font->getFontSize();
        int caretTop = m_rcText.top + m_rcText.getHeight()/2 - fontSize;
        int caretBottom = m_rcText.top + m_rcText.getHeight()/2 + fontSize;
        Rect rcCaret = Rect(m_rcText.left + rtSize.x - 1, std::max(m_rcText.top, caretTop),
                            m_rcText.left + rtSize.x + 1, std::min(m_rcText.bottom, caretBottom));

		// If we are in overwrite mode, adjust the caret rectangle
		// to fill the entire character.
		if( !m_bInsertMode )
		{
			// Obtain the right edge X coord of the current character
			int nRightEdgeX;
            nRightEdgeX = nXFirst + m_elementsFonts[0].font->getFontSize() * ( m_nCaret + 1 );
			rcCaret.right = m_rcText.left - nXFirst + nRightEdgeX;
            std::string latterAfterCaret;
            latterAfterCaret.push_back(m_buffer[m_nCaret]);
            Point letterSize = m_elementsFonts[0].font->calcTextRect(latterAfterCaret);
            rcCaret.right = rcCaret.left + letterSize.x;
		}

        renderRect(sprite, rcCaret, NO_TEXTURE, Rect(), m_CaretColor, dialogPos);
	}
}

//-----------------------------------------------------------------------------
// Name : SetText() 
//-----------------------------------------------------------------------------
void EditBoxUI::SetText( std::string strText, bool bSelected /* = false */ )
{
    m_buffer = strText;
	m_nFirstVisible = 0;
	m_nBackwardChars = 0;
    m_nVisibleChars = m_buffer.size();
	// Move the caret to the end of the text
    PlaceCaret( m_buffer.size() );

	if (bSelected)
		m_nSelStart = 0;
	else
		m_nSelStart = m_nCaret;
}

//-----------------------------------------------------------------------------
// Name : GetText() 
//-----------------------------------------------------------------------------
std::string EditBoxUI::GetText()
{
    return m_buffer;
}

//-----------------------------------------------------------------------------
// Name : GetTextLength() 
//-----------------------------------------------------------------------------
int EditBoxUI::GetTextLength()
{
    return m_buffer.size();
}

//-----------------------------------------------------------------------------
// Name : GetTextCopy() 
//-----------------------------------------------------------------------------
bool EditBoxUI::GetTextCopy(std::string strDest, GLuint bufferCount)
{
    return false;
//	assert( strDest );

//	strcpy_s( strDest, bufferCount, m_Buffer.c_str() );

//	return S_OK;
}

//-----------------------------------------------------------------------------
// Name : ClearText() 
//-----------------------------------------------------------------------------
void EditBoxUI::ClearText()
{
    m_buffer.clear();
	m_nFirstVisible = 0;
	m_nBackwardChars = 0;
    m_nVisibleChars = m_buffer.size();
	PlaceCaret( 0 );
	m_nSelStart = 0;
}

//-----------------------------------------------------------------------------
// Name : SetTextColor() 
//-----------------------------------------------------------------------------
void EditBoxUI::SetTextColor(glm::vec4 Color)
{
	m_TextColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedTextColor() 
//-----------------------------------------------------------------------------
void EditBoxUI::SetSelectedTextColor(glm::vec4 Color)
{
	m_SelTextColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedBackColor() 
//-----------------------------------------------------------------------------
void EditBoxUI::SetSelectedBackColor(glm::vec4 Color)
{
	m_SelBkColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetCaretColor() 
//-----------------------------------------------------------------------------
void EditBoxUI::SetCaretColor(glm::vec4 Color)
{
	m_CaretColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetBorderWidth() 
//-----------------------------------------------------------------------------
void EditBoxUI::SetBorderWidth(int nBorder)
{
	m_nBorder = nBorder;
}

//-----------------------------------------------------------------------------
// Name : SetSpacing() 
//-----------------------------------------------------------------------------
void EditBoxUI::SetSpacing(int nSpacing)
{
	m_nSpacing = nSpacing;
	UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : ParseFloatArray() 
//-----------------------------------------------------------------------------
// void CEditBoxUI::ParseFloatArray( float* pNumbers, int nCount )
// {
// 	int nWritten = 0;  // Number of floats written
// 	const char* pToken, *pEnd;
// 	char strToken[60];
// 
// 	pToken = m_Buffer.GetBuffer();
// 	while( nWritten < nCount && *pToken != L'\0' )
// 	{
// 		// Skip leading spaces
// 		while( *pToken == L' ' )
// 			++pToken;
// 
// 		if( *pToken == L'\0' )
// 			break;
// 
// 		// Locate the end of number
// 		pEnd = pToken;
// 		//TODO: replace this crap with something that is logical and elegant 
// 		while( IN_FLOAT_CHARSET( *pEnd ) )
// 			++pEnd;
// 
// 		// Copy the token to our buffer
// 		int nTokenLen = __min( sizeof( wszToken ) / sizeof( wszToken[0] ) - 1, int( pEnd - pToken ) );
// 		strcpy_s( wszToken, nTokenLen, pToken );
// 		*pNumbers = ( float )strtod( wszToken, NULL );
// 		++nWritten;
// 		++pNumbers;
// 		pToken = pEnd;
// 	}
//}

//-----------------------------------------------------------------------------
// Name : SetTextFloatArray() 
//-----------------------------------------------------------------------------
void EditBoxUI::SetTextFloatArray( const float* pNumbers, int nCount )
{
//	char strBuffer[512] = {0};
//	char strTmp[64];

//	if( pNumbers == NULL )
//		return;

//	for( int i = 0; i < nCount; ++i )
//	{
//		sprintf_s( strTmp, 64, "%.4f ", pNumbers[i] );
//		strcat_s( strBuffer, 512, strTmp );
//	}

//	// Don't want the last space
//	if( nCount > 0 && strlen( strBuffer ) > 0 )
//		strBuffer[strlen( strBuffer ) - 1] = 0;

//	SetText( strBuffer );
}

//-----------------------------------------------------------------------------
// Name : CalcCaretPosByPoint() 
//-----------------------------------------------------------------------------
int EditBoxUI::CalcCaretPosByPoint( Point pt )
{
	// Determine the character corresponding to the coordinates.
	int nCP;

    Rect rcFullText = {0, 0, 0, 0};

    Point textSize = m_elementsFonts[0].font->calcTextRect(m_buffer);

    for (nCP = m_nFirstVisible - m_nBackwardChars; nCP <= m_buffer.size() ; nCP++)
	{
        std::string curCursorText = m_buffer.substr(0, nCP);
        Point CursorTextSize = m_elementsFonts[0].font->calcTextRect(curCursorText);

		if (m_nFirstVisible - m_nBackwardChars != 0 )
		{
            int rtExtra = textSize.x - CursorTextSize.x;
            CursorTextSize.x = (m_rcText.right - m_rcText.left) - rtExtra;
		}

        if (CursorTextSize.x < (pt.x - m_rcText.left) )
			continue;
		else 
			break;
	}

    if ( ( pt.x - m_rcText.left ) < 0)
		nCP--; 

	return nCP;
}

//-----------------------------------------------------------------------------
// Name : PlaceCaret() 
//-----------------------------------------------------------------------------
void EditBoxUI::PlaceCaret( int nCP )
{
    assert( nCP >= 0 && nCP <= m_buffer.size() );
    m_nCaret = nCP;
}

//-----------------------------------------------------------------------------
// Name : DeleteSelectionText() 
//-----------------------------------------------------------------------------
void EditBoxUI::DeleteSelectionText()
{
    int nFirst = std::min( m_nCaret, m_nSelStart );
    int nLast = std::max( m_nCaret, m_nSelStart );
	// Update caret and selection
	PlaceCaret( nFirst );
	m_nSelStart = m_nCaret;
	CalcFirstVisibleCharDown();
    m_nVisibleChars = m_buffer.size() - m_nFirstVisible;

	// Remove the characters
    m_buffer.erase(nFirst, nLast - nFirst);
}

//-----------------------------------------------------------------------------
// Name : ResetCaretBlink() 
//-----------------------------------------------------------------------------
void EditBoxUI::ResetCaretBlink()
{
	//TODO: try to use the timer here instead of this ugly thing
//	__int64 currTime;
//	__int64 prefFreq;
//	float   timeScale;

//	m_bCaretOn = true;

//	QueryPerformanceCounter((LARGE_INTEGER *)&currTime);
//	QueryPerformanceFrequency((LARGE_INTEGER *)&prefFreq);

//	timeScale			= 1.0f / prefFreq;

//	m_dfLastBlink = currTime * timeScale;
}

//-----------------------------------------------------------------------------
// Name : CopyToClipboard() 
//-----------------------------------------------------------------------------
void EditBoxUI::CopyToClipboard()
{
    if (m_nCaret != m_nSelStart)
    {
        GameWin::copyToClipboard(m_buffer.substr(std::min(m_nCaret, m_nSelStart), std::abs(m_nCaret - m_nSelStart)));
    }
}

//-----------------------------------------------------------------------------
// Name : PasteFromClipboard() 
//-----------------------------------------------------------------------------
void EditBoxUI::PasteFromClipboard()
{
    DeleteSelectionText();
    std::string pasteString = GameWin::PasteClipboard();
    if (pasteString != "")
    {
        m_buffer.insert(m_nCaret, pasteString);
        PlaceCaret(m_nCaret + pasteString.size());
        m_nSelStart = m_nCaret;
    }
}

//-----------------------------------------------------------------------------
// Name : CanHaveFocus() 
//-----------------------------------------------------------------------------
bool EditBoxUI::CanHaveFocus()
{
	return ( m_bVisible && m_bEnabled );
}

//-----------------------------------------------------------------------------
// Name : OnFocusIn 
//-----------------------------------------------------------------------------
void EditBoxUI::OnFocusIn()
{
    ControlUI::OnFocusIn();
	ResetCaretBlink();
}

//-----------------------------------------------------------------------------
// Name : OnFocusOut 
//-----------------------------------------------------------------------------
void EditBoxUI::OnFocusOut()
{
    ControlUI::OnFocusOut();
	m_nSelStart = m_nCaret;
}

//-----------------------------------------------------------------------------
// Name : SaveToFile 
//-----------------------------------------------------------------------------
bool EditBoxUI::SaveToFile(std::ostream& SaveFile)
{
    ControlUI::SaveToFile(SaveFile);
	
	//TODO: make all the editbox options saveable
    SaveFile << m_buffer << "| EditBox text " << "\n";
	SaveFile << m_nBorder << "| EditBox Border Number" << "\n";
	SaveFile << m_nSpacing << "| EditBox Spacing" << "\n";
	SaveFile << m_bCaretOn << "| is EditBox Caret On" << "\n";
// 	SaveFile << m_TextColor << "| EditBox Text Color" << "\n";
// 	SaveFile << m_SelTextColor << "| EditBox Selection Color" << "\n";
// 	SaveFile << m_SelBkColor << "| EditBox Background Color" << "\n";
//	SaveFile << m_CaretColor << "| EditBox Caret Color" << "\n";

	return true;
}

//-----------------------------------------------------------------------------
// Name : CalcFirstVisibleCharUp 
//-----------------------------------------------------------------------------
int EditBoxUI::CalcFirstVisibleCharUp()
{
    std::string visibleText = m_buffer.substr(m_nFirstVisible, m_buffer.size() - m_nFirstVisible);
	m_nVisibleChars = visibleText.size();

    Point rtSize = m_elementsFonts[0].font->calcTextRect(visibleText);

    int textEdge =  m_rcText.left + rtSize.x;

	if (textEdge > m_rcText.right )
	{
		m_nFirstVisible++;
		CalcFirstVisibleCharUp();
	}
	else
		return m_nFirstVisible;
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : CalcFirstVisibleCharDown 
//-----------------------------------------------------------------------------
int EditBoxUI::CalcFirstVisibleCharDown()
{
    if (m_elementsFonts.size() == 0)
        return 0;

    Rect rt = Rect(0,0,0,0);

    std::string visibleText = m_buffer.substr(m_nFirstVisible, m_buffer.size() - m_nFirstVisible);
	m_nVisibleChars = visibleText.size();

    m_elementsFonts[0].font->calcTextRect(visibleText);

	int textEdge =  m_rcText.left + rt.right;

	if (textEdge < m_rcText.right  && m_nFirstVisible > 0)
	{
		m_nFirstVisible--;
		//return m_nFirstVisible;
		CalcFirstVisibleCharDown();
	}
	else
	{
		if (m_nBackwardChars > m_nFirstVisible)
			m_nBackwardChars = m_nFirstVisible;

		return m_nFirstVisible;
	}

	return 0;
} 
