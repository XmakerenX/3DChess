#include "EditBoxUI.h"
#include "DialogUI.h"
#include "../GameWin.h"
#include <iostream>

bool EditBoxUI::s_bHideCaret;
int EditBoxUI::s_caretBlinkTime = 100;


//-----------------------------------------------------------------------------
// Define << and >> operators for glm::vec4
//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const glm::vec4 v)
{
    os << v.x << " " << v.y << " " << v.z << " " << v.w;
    return os;
}

std::istream& operator>>(std::istream& is, glm::vec4 v)
{
    float x,y,z,w;
    is >> x;
    is >> y;
    is >> z;
    is >> w;
    v = glm::vec4(x,y,z,w);
    return is;
}

//TODO: remake editbox so when scrolling text will less glitchy
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
    m_dfBlink = s_caretBlinkTime * 0.001;
    m_dfLastBlink = 0;
	s_bHideCaret = false;

    m_TextColor = glm::vec4(0.07f, 0.07f, 0.07f, 1.0);
    m_SelTextColor = WHITE_COLOR;
    m_SelBkColor = glm::vec4(0.16f, 0.2f, 0.36f, 1.0);
    m_CaretColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_nCaret = m_nSelStart = 0;
	m_bInsertMode = true;

	m_bMouseDrag = false;
}

//-----------------------------------------------------------------------------
// Name : CEditBoxUI(constructor from InputFile)
//-----------------------------------------------------------------------------
EditBoxUI::EditBoxUI(std::istream& inputFile)
    :ControlUI(inputFile)
{
    m_type = ControlUI::EDITBOX;

    m_dfBlink = s_caretBlinkTime * 0.001;
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
    setText(bufferText.c_str() );
    m_nVisibleChars = m_buffer.size();
	inputFile >> m_nBorder;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_nSpacing;
    setSpacing(m_nSpacing);
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
	inputFile >> m_bCaretOn;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_TextColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_SelTextColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_SelBkColor;
	inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_CaretColor;
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
        // If there's a selection, treat this like a delete key.
        if( m_nCaret != m_nSelStart )
        {
            deleteSelectionText();
            m_editboxChangedSig(this);
        }
        else if( m_nCaret > 0 )
        {
            // Move the caret, then delete the char.
            placeCaret( m_nCaret - 1 );
            m_nSelStart = m_nCaret;
            m_buffer.erase(m_nCaret,1);

            if (m_nFirstVisible > 0)
                calcFirstVisibleCharDown();

            m_nVisibleChars = m_buffer.size() - m_nFirstVisible;

            m_editboxChangedSig(this);
        }
        resetCaretBlink();
        return true;
    }break;

    case 127: // delete
    {
        // Check if there is a text selection.
        if( m_nCaret != m_nSelStart )
        {
            deleteSelectionText();
            calcFirstVisibleCharDown();
            m_editboxChangedSig(this);
        }
        else
        {
            // Deleting one character
            m_buffer.erase(m_nCaret,1);
            calcFirstVisibleCharDown();
            m_editboxChangedSig(this);
        }
        resetCaretBlink();
        return true;
    }break;

    case 24: // Ctrl-X Cut
    case 3:  // Ctrl-C Copy
    {
        copyToClipboard();

        // If the key is Ctrl-X, delete the selection too.
        if( key == 24 )
        {
            deleteSelectionText();
            m_editboxChangedSig(this);
        }break;
    }

    // Ctrl-V Paste
    case 22:
    {
        pasteFromClipboard();
        m_editboxChangedSig(this);
    }break;

    // Ctrl-A Select All
    case 1:
    {
        if( m_nSelStart == m_nCaret )
        {
            m_nSelStart = 0;
            placeCaret( m_buffer.size() );
        }
    }break;

    // Return (enter)
    case 13:
        // Invoke signal when the user presses Enter
        //m_pDialog->SendEvent( EVENT_EDITBOX_STRING, true, this );
        break;

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
    case 28:  // Ctrl '\'
    case 31:  // Ctrl '/'
        break;

    default:
    {
        // If there's a selection and the user
        // starts to type, the selection should
        // be deleted.
        if( m_nCaret != m_nSelStart )
            deleteSelectionText();

        // If we are in overwrite mode and there is already
        // a char at the caret's position, simply replace it.
        // Otherwise, we insert the char as normal.
        if( !m_bInsertMode && m_nCaret < m_buffer.size())
        {
            m_buffer[m_nCaret] = key;
            placeCaret( m_nCaret + 1 );
            m_nSelStart = m_nCaret;

            if ( (m_nCaret  - ( m_nFirstVisible - m_nBackwardChars ) ) > m_buffer.size() - m_nFirstVisible )
            {
                if (m_nBackwardChars > 0)
                    m_nBackwardChars--;
            }
        }
        else
        {
            // Insert the char
            m_buffer.insert(m_nCaret, 1, key);

            placeCaret( m_nCaret + 1 );
            m_nSelStart = m_nCaret;
            calcFirstVisibleCharUp(false);
        }

        resetCaretBlink();
        m_editboxChangedSig(this);
        return true;
    }

    }

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
        placeCaret(0);
        m_nBackwardChars = m_nFirstVisible;
        if( !modifierStates.bShift )
        {
            // Shift is not pressed(down), Cancel selection
            m_nSelStart = m_nCaret;
        }
        resetCaretBlink();
        return true;
    }break;

    case GK_VirtualKey::GK_End:
    {
        placeCaret( m_buffer.size() );
        m_nBackwardChars = 0;
        if( !modifierStates.bShift )
        {
            // Shift is not pressed(down), Cancel selection
            m_nSelStart = m_nCaret;
        }
        resetCaretBlink();
        return true;
    }break;

    case GK_VirtualKey::GK_Insert:
    {
        // Toggle caret insert mode
        m_bInsertMode = !m_bInsertMode;
        return true;
    }break;

    case GK_VirtualKey::GK_Left:
    {
        if (m_nCaret > 0)
        {
            m_nCaret--;
            if ( m_nCaret < m_nFirstVisible - m_nBackwardChars)
            {
                if (m_nFirstVisible > 0 && m_nBackwardChars < m_nFirstVisible)
                    m_nBackwardChars++;
            }
            placeCaret( m_nCaret );
        }

        if( !modifierStates.bShift )
        {
            // Shift is not pressed(down), Cancel selection
            m_nSelStart = m_nCaret;
        }

        resetCaretBlink();
        return true;
    }break;

    case GK_VirtualKey::GK_Right:
    {
        if( m_nCaret < m_buffer.size() )
        {
            if ( (m_nCaret + 1 - ( m_nFirstVisible - m_nBackwardChars ) ) > m_buffer.size() - m_nFirstVisible )
            {
                if (m_nBackwardChars > 0)
                    m_nBackwardChars--;
            }

            placeCaret( m_nCaret + 1 );
        }

        if( !modifierStates.bShift )
        {
            // Shift is not pressed(down), Cancel selection
            m_nSelStart = m_nCaret;
        }

        resetCaretBlink();
        return true;
    }break;

    default:
        return false;

    }

}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent()
//-----------------------------------------------------------------------------
bool EditBoxUI::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch(event.type)
    {
    case MouseEventType::LeftButton:
    {
        if (event.down)
            return Pressed(event.cursorPos, modifierStates, event.timeStamp);
        else
            return Released(event.cursorPos);
    }break;

    case MouseEventType::MouseMoved:
    {
        if ( Dragged(event.cursorPos) )
            return true;
    }break;

    default:
        return false;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed() 
//-----------------------------------------------------------------------------
bool EditBoxUI::Pressed(Point pt, const ModifierKeysStates &modifierStates, double timeStamp)
{
	if( !ContainsPoint( pt ) )
		return false;

	if( !m_bHasFocus )
		m_pParentDialog->RequestFocus( this );

	m_bMouseDrag = true;
	// Determine the character corresponding to the coordinates.
 	int nCP;//, nTrail, nX1st;

    nCP = calcCaretPosByPoint(pt);

    if (nCP > m_buffer.size())
        nCP = m_buffer.size();

	// Cap at the NULL character.
    if(nCP >= 0 && nCP < (m_buffer.size() + 1) )
        placeCaret( nCP );
	else
        placeCaret( 0 );
	m_nSelStart = m_nCaret;
    resetCaretBlink();

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
        nCP = calcCaretPosByPoint(pt);

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
			
            placeCaret( nCP );

			if ( nCP > m_nCaret)
                calcFirstVisibleCharUp(true);
			
			if ( nCP < m_nCaret)
                calcFirstVisibleCharDown();
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

            placeCaret( nCP );

			if ( nCP > m_nCaret)
                calcFirstVisibleCharUp(true);

			if ( nCP < m_nCaret)
                calcFirstVisibleCharDown();
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
void EditBoxUI::Render(Sprite sprites[SPRITES_SIZE], Sprite topSprites[SPRITES_SIZE], double timeStamp)
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
        renderRect(sprites[NORMAL], m_rcRender[i], m_elementsGFX[i].iTexture, m_elementsGFX[i].rcTexture, WHITE_COLOR, dialogPos);
    }

    //
    // Compute the X coordinates of the first visible character.
    //
    int nXFirst = m_rcText.left + m_elementsFonts[0].fontInfo.fontSize;
    // compute the x coordinates of the caret
    nCaretX = m_rcText.left + m_elementsFonts[0].fontInfo.fontSize; //+
              //m_nCaret * m_elementsFonts[0].fontInfo.fontSize;

    if (nCaretX > m_rcText.right)
        ;//m_nFirstVisible = (nCaretX - m_rcText.right) / fontItem.avgWidth;

    nCaretX = m_rcText.right - m_elementsFonts[0].fontInfo.fontSize;

    // Render the text
    Rect rt = {0, 0, 0, 0};
    Rect rtFullText = {0, 0, 0, 0};

    std::string fullText = m_buffer.substr(0, m_buffer.size() - m_nBackwardChars);
    std::string textToRender;

    if (m_nBackwardChars == 0)
        textToRender = m_buffer.substr(m_nFirstVisible, m_buffer.size() - m_nFirstVisible );
    else
        //textToRender = m_buffer.substr(m_nFirstVisible - m_nBackwardChars, m_buffer.size() - m_nBackwardChars );
        textToRender = m_buffer.substr(m_nFirstVisible - m_nBackwardChars, m_buffer.size() - m_nFirstVisible );

    if (m_nFirstVisible - m_nBackwardChars == 0)
        renderText(sprites[TEXT], m_elementsFonts[0].font, textToRender, m_TextColor, m_rcText, dialogPos, mkFont::TextFormat::VerticalCenter);
    else
        renderText(sprites[TEXT], m_elementsFonts[0].font, textToRender, m_TextColor, m_rcText, dialogPos, mkFont::TextFormat::VerticalCenter);

    renderSelection(sprites[NORMAL], sprites[TEXT], dialogPos);

    // Render the caret if this control has the focus
    renderCaret(sprites[NORMAL], timeStamp, textToRender,dialogPos);
}

//-----------------------------------------------------------------------------
// Name : renderSelection()
//-----------------------------------------------------------------------------
void EditBoxUI::renderSelection(Sprite& sprite, Sprite& textSprite, Point dialogPos)
{
    // Make this available for rendering selected text
    Rect rcSelection;

    if( m_nCaret != m_nSelStart )
    {
        int nXFirst = m_rcText.left + m_elementsFonts[0].fontInfo.fontSize;
        int nCaretX = m_rcText.right - m_elementsFonts[0].fontInfo.fontSize;
        int nSelStartX = nXFirst + m_nSelStart * m_elementsFonts[0].fontInfo.fontSize;

        int nSelLeftX = nCaretX, nSelRightX = nSelStartX;
        // Swap if left is bigger than right
        if( nSelLeftX > nSelRightX )
        {
            int nTemp = nSelLeftX;
            nSelLeftX = nSelRightX;
            nSelRightX = nTemp;
        }

        int nFirstToRender = std::max( m_nFirstVisible - m_nBackwardChars, std::min( m_nSelStart, m_nCaret ) );
        int nLastToRender = m_buffer.size() - m_nBackwardChars;
        int nNumChatToRender = std::min(std::max( m_nSelStart, m_nCaret ), nLastToRender) - nFirstToRender;

        std::string temp = m_buffer.substr( nFirstToRender, nNumChatToRender);

        Rect rcSelectionBound = {0, 0, 0, 0};
        Rect rcSelectX = {0, 0, 0, 0};

        Point textSize = m_elementsFonts[0].font->calcTextRect(temp);

        temp = m_buffer.substr(m_nFirstVisible - m_nBackwardChars, nFirstToRender - (m_nFirstVisible - m_nBackwardChars));

        Point textSelectSize = m_elementsFonts[0].font->calcTextRect(temp);

        rcSelection = Rect(nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom);
        rcSelection.offset(m_rcText.left - nXFirst, 0);

        rcSelection = rcSelection.intersectRect(rcSelection, m_rcText);

        Rect rtFullText = {0, 0, 0, 0};
        Point fullTextSize = m_elementsFonts[0].font->calcTextRect(m_buffer);

        nSelLeftX = m_rcText.left + textSelectSize.x + 0;
        nSelRightX = m_rcText.left + textSelectSize.x + textSize.x;

        int fontSize = m_elementsFonts[0].font->getFontSize();
        int nSelTop = m_rcText.top + m_rcText.getHeight()/2 - fontSize;
        int nSelBottom = m_rcText.top + m_rcText.getHeight()/2 + fontSize;
        rcSelection = Rect(nSelLeftX, std::max(m_rcText.top, nSelTop), nSelRightX, std::min(m_rcText.bottom, nSelBottom));

        renderRect(sprite, rcSelection, 0, Rect(), m_SelBkColor, dialogPos);

        //temp = m_buffer.substr( nFirstToRender, nNumChatToRender - (m_nFirstVisible - m_nBackwardChars));
        temp = m_buffer.substr( nFirstToRender, nNumChatToRender);
        renderText(textSprite, m_elementsFonts[0].font, temp, m_SelTextColor, rcSelection, dialogPos, mkFont::TextFormat::VerticalCenter);
    }
}

//-----------------------------------------------------------------------------
// Name : renderCaret()
//-----------------------------------------------------------------------------
void EditBoxUI::renderCaret(Sprite& sprite, double timeStamp, std::string& textTorender, Point dialogPos)
{
    int nXFirst = m_rcText.left + m_elementsFonts[0].fontInfo.fontSize;
    std::string temp = m_buffer.substr(m_nFirstVisible -  m_nBackwardChars, m_nCaret - (m_nFirstVisible -  m_nBackwardChars));

    Point rtSize =  m_elementsFonts[0].font->calcTextRect(temp);
    Point fullTextSize = m_elementsFonts[0].font->calcTextRect(textTorender);

    // Blink the caret
    if( timeStamp - m_dfLastBlink >= m_dfBlink )
    {
        m_bCaretOn = !m_bCaretOn;
        m_dfLastBlink = timeStamp;
    }

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
void EditBoxUI::setText( std::string strText, bool bSelected /* = false */ )
{
    m_buffer = strText;
	m_nFirstVisible = 0;
	m_nBackwardChars = 0;
    m_nVisibleChars = m_buffer.size();
	// Move the caret to the end of the text
    placeCaret( m_buffer.size() );

	if (bSelected)
		m_nSelStart = 0;
	else
		m_nSelStart = m_nCaret;
}

//-----------------------------------------------------------------------------
// Name : GetText() 
//-----------------------------------------------------------------------------
const std::string& EditBoxUI::getText()
{
    return m_buffer;
}

//-----------------------------------------------------------------------------
// Name : GetTextLength() 
//-----------------------------------------------------------------------------
int EditBoxUI::getTextLength()
{
    return m_buffer.size();
}

//-----------------------------------------------------------------------------
// Name : GetTextCopy() 
//-----------------------------------------------------------------------------
std::string EditBoxUI::getTextCopy()
{
    return m_buffer;
}

//-----------------------------------------------------------------------------
// Name : ClearText() 
//-----------------------------------------------------------------------------
void EditBoxUI::clearText()
{
    m_buffer.clear();
	m_nFirstVisible = 0;
	m_nBackwardChars = 0;
    m_nVisibleChars = m_buffer.size();
    placeCaret( 0 );
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
void EditBoxUI::setSelectedTextColor(glm::vec4 Color)
{
	m_SelTextColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetSelectedBackColor() 
//-----------------------------------------------------------------------------
void EditBoxUI::setSelectedBackColor(glm::vec4 Color)
{
	m_SelBkColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetCaretColor() 
//-----------------------------------------------------------------------------
void EditBoxUI::setCaretColor(glm::vec4 Color)
{
	m_CaretColor = Color;
}

//-----------------------------------------------------------------------------
// Name : SetBorderWidth() 
//-----------------------------------------------------------------------------
void EditBoxUI::setBorderWidth(int nBorder)
{
	m_nBorder = nBorder;
}

//-----------------------------------------------------------------------------
// Name : SetSpacing() 
//-----------------------------------------------------------------------------
void EditBoxUI::setSpacing(int nSpacing)
{
	m_nSpacing = nSpacing;
	UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : CalcCaretPosByPoint() 
//-----------------------------------------------------------------------------
int EditBoxUI::calcCaretPosByPoint( Point pt )
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
void EditBoxUI::placeCaret( int nCP )
{
    assert( nCP >= 0 && nCP <= m_buffer.size() );
    m_nCaret = nCP;
}

//-----------------------------------------------------------------------------
// Name : DeleteSelectionText() 
//-----------------------------------------------------------------------------
void EditBoxUI::deleteSelectionText()
{
    int nFirst = std::min( m_nCaret, m_nSelStart );
    int nLast = std::max( m_nCaret, m_nSelStart );

    // Remove the characters
    m_buffer.erase(nFirst, nLast - nFirst);

	// Update caret and selection
    placeCaret( nFirst );
	m_nSelStart = m_nCaret;
    m_nFirstVisible = 0;
    calcFirstVisibleCharDown();
    m_nVisibleChars = m_buffer.size() - m_nFirstVisible;
}

//-----------------------------------------------------------------------------
// Name : ResetCaretBlink() 
//-----------------------------------------------------------------------------
void EditBoxUI::resetCaretBlink()
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
void EditBoxUI::copyToClipboard()
{
    if (m_nCaret != m_nSelStart)
    {
        GameWin::copyToClipboard(m_buffer.substr(std::min(m_nCaret, m_nSelStart), std::abs(m_nCaret - m_nSelStart)));
    }
}

//-----------------------------------------------------------------------------
// Name : PasteFromClipboard() 
//-----------------------------------------------------------------------------
void EditBoxUI::pasteFromClipboard()
{
    deleteSelectionText();
    std::string pasteString = GameWin::PasteClipboard();
    if (pasteString != "")
    {
        m_buffer.insert(m_nCaret, pasteString);
        placeCaret(m_nCaret + pasteString.size());
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
    resetCaretBlink();
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
    SaveFile << m_TextColor << "| EditBox Text Color" << "\n";
    SaveFile << m_SelTextColor << "| EditBox Selection Color" << "\n";
    SaveFile << m_SelBkColor << "| EditBox Background Color" << "\n";
    SaveFile << m_CaretColor << "| EditBox Caret Color" << "\n";

	return true;
}

//-----------------------------------------------------------------------------
// Name : CalcFirstVisibleCharUp 
//-----------------------------------------------------------------------------
int EditBoxUI::calcFirstVisibleCharUp(bool insertMode)
{
    std::string visibleText = m_buffer.substr(m_nFirstVisible, m_buffer.size() - (m_nFirstVisible - m_nBackwardChars));
    m_nVisibleChars = visibleText.size();

    Point rtSize = m_elementsFonts[0].font->calcTextRect(visibleText);

    int textEdge =  m_rcText.left + rtSize.x;

    bool changed = false;
    while (textEdge > m_rcText.right)
    {
        if (m_nCaret != m_buffer.size() - m_nBackwardChars)
            m_nBackwardChars++;
        m_nFirstVisible++;
        changed = true;

        visibleText = m_buffer.substr(m_nFirstVisible, m_buffer.size() - (m_nFirstVisible - m_nBackwardChars));
        m_nVisibleChars = visibleText.size();
        rtSize = m_elementsFonts[0].font->calcTextRect(visibleText);
        textEdge =  m_rcText.left + rtSize.x;
    }

    if (!changed && m_nFirstVisible > 0 && !insertMode)
    {
        if (m_nCaret != m_buffer.size() - m_nBackwardChars)
            m_nBackwardChars++;
        m_nFirstVisible++;
    }

    return m_nFirstVisible;
}

//-----------------------------------------------------------------------------
// Name : CalcFirstVisibleCharDown 
//-----------------------------------------------------------------------------
int EditBoxUI::calcFirstVisibleCharDown()
{
    if (m_elementsFonts.size() == 0)
        return 0;

    if (m_nFirstVisible > 0)
        m_nFirstVisible--;

    if (m_nBackwardChars > m_nFirstVisible)
        m_nBackwardChars = m_nFirstVisible;

	return 0;
} 
