#include "ScrollBarUI.h"
#include "DialogUI.h"

const int   ScrollBarUI::SCROLLBAR_MINTHUMBSIZE = 8;
const float ScrollBarUI::SCROLLBAR_ARROWCLICK_DELAY = 0.33f;
const float ScrollBarUI::SCROLLBAR_ARROWCLICK_REPEAT = 0.05f;
//-----------------------------------------------------------------------------
// Name : ScrollBarUI(constructor)
//-----------------------------------------------------------------------------
ScrollBarUI::ScrollBarUI(void)
{
    m_type = SCROLLBAR;
    m_bShowThumb = false;
    m_bDrag = false;

    m_rcElements[UPBUTTON] = Rect(0, 0 ,0 ,0);
    m_rcElements[DOWNBUTTON] = Rect(0, 0 ,0 ,0);
    m_rcElements[TRACK] = Rect(0, 0 ,0 ,0);
    m_rcElements[THUMB] = Rect(0, 0 ,0 ,0);

    m_nPosition = 0;
    m_nPageSize = 1;
    m_nStart = 0;
    m_nEnd = 1;

    m_LastMouse.x = 0;
    m_LastMouse.y = 0;

    m_Arrow = CLEAR;
    m_dArrowTS = 0.0;
}

//-----------------------------------------------------------------------------
// Name : CScrollBarUI(constructor)
//-----------------------------------------------------------------------------
ScrollBarUI::ScrollBarUI(std::istream& inputFile) : ControlUI(inputFile)
{
    m_type = SCROLLBAR;
    m_bShowThumb = false;
    m_bDrag = false;

    m_rcElements[UPBUTTON] = Rect(0, 0 ,0 ,0);
    m_rcElements[DOWNBUTTON] = Rect(0, 0 ,0 ,0);
    m_rcElements[TRACK] = Rect(0, 0 ,0 ,0);
    m_rcElements[THUMB] = Rect(0, 0 ,0 ,0);

    m_nPosition = 0;
    m_nPageSize = 1;
    m_nStart = 0;
    m_nEnd = 1;

    m_LastMouse.x = 0;
    m_LastMouse.y = 0;

    m_Arrow = CLEAR;
    m_dArrowTS = 0.0;
}

//-----------------------------------------------------------------------------
// Name : ~CScrollBarUI(destructor)
//-----------------------------------------------------------------------------
ScrollBarUI::~ScrollBarUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent()
// Desc : handles mouse input
//-----------------------------------------------------------------------------
bool ScrollBarUI::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    if (!m_bVisible || !m_bEnabled)
        return false;
    
    static int ThumbOffsetY;

    m_LastMouse = event.cursorPos;

    switch (event.type)
    {
    case MouseEventType::LeftButton:
    case MouseEventType::DoubleLeftButton:
    {
        if (event.down)
        {
            if (Pressed(event.cursorPos, modifierStates, event.timeStamp))
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
        if ( Dragged(event.cursorPos))
            return true;
    }break;

    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed()
//-----------------------------------------------------------------------------
bool ScrollBarUI::Pressed( Point pt, const ModifierKeysStates &modifierStates, double timeStamp)
{
    // Check for click on up button
    if (m_rcElements[UPBUTTON].isPointInRect(pt))
    {
        if( m_nPosition > m_nStart )
            --m_nPosition;

        UpdateThumbRect();
        m_Arrow = CLICKED_UP;
        // save time stamp for arrow repeat
        m_dArrowTS = timeStamp;
        return true;
    }

    // Check for click on down button
    if (m_rcElements[DOWNBUTTON].isPointInRect(pt))
    {
        if( m_nPosition + m_nPageSize <= m_nEnd - 1 )
            ++m_nPosition;

        UpdateThumbRect();
        m_Arrow = CLICKED_DOWN;
        m_dArrowTS = timeStamp;
        return true;
    }

    // Check for click on thumb
    if (m_rcElements[THUMB].isPointInRect(pt))
    {
        m_bDrag = true;
        m_thumbOffsetY = pt.y - m_rcElements[THUMB].top;
        return true;
    }

    // Check for click on track
    if( m_rcElements[THUMB].left <= pt.x && m_rcElements[THUMB].right > pt.x )
    {
        if( m_rcElements[THUMB].top > pt.y && m_rcElements[TRACK].top <= pt.y )
        {
            Scroll( -( m_nPageSize - 1 ) );
            return true;
        }
        else if( m_rcElements[THUMB].bottom <= pt.y && m_rcElements[TRACK].bottom > pt.y )
        {
            Scroll( m_nPageSize - 1 );
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Released()
//-----------------------------------------------------------------------------
bool ScrollBarUI::Released( Point pt)
{
    m_bDrag = false;
    UpdateThumbRect();
    m_Arrow = CLEAR;

    return ContainsPoint(pt);
}

//-----------------------------------------------------------------------------
// Name : Dragged()
//-----------------------------------------------------------------------------
bool ScrollBarUI::Dragged( Point pt)
{
    if( m_bDrag )
    {
        m_rcElements[THUMB].bottom += pt.y - m_thumbOffsetY - m_rcElements[THUMB].top;
        m_rcElements[THUMB].top = pt.y - m_thumbOffsetY;

        if( m_rcElements[THUMB].top < m_rcElements[TRACK].top )
            m_rcElements[THUMB].offset(0, m_rcElements[TRACK].top - m_rcElements[THUMB].top);

        else if( m_rcElements[THUMB].bottom > m_rcElements[TRACK].bottom )
            m_rcElements[THUMB].offset(0, m_rcElements[TRACK].bottom - m_rcElements[THUMB].bottom);

        // Compute first item index based on thumb position
        int rcTrackHeight = m_rcElements[TRACK].bottom - m_rcElements[TRACK].top;
        int rcThumbHeight = m_rcElements[THUMB].bottom - m_rcElements[THUMB].top;

        int nMaxFirstItem = m_nEnd - m_nStart - m_nPageSize;  // Largest possible index for first item
        int nMaxThumb = rcTrackHeight - rcThumbHeight ;  // Largest possible thumb position from the top

        m_nPosition = m_nStart + ( m_rcElements[THUMB].top - m_rcElements[TRACK].top +
            nMaxThumb / ( nMaxFirstItem * 2 ) ) * // Shift by half a row to avoid last row covered by only one pixel
            nMaxFirstItem / nMaxThumb;

        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Name : SaveToFile()
// NOTE : seems to be no need to save the Scrollbar as it is automatically generated
//-----------------------------------------------------------------------------
bool ScrollBarUI::SaveToFile(std::ostream& SaveFile)
{
    ControlUI::SaveToFile(SaveFile);
    return true;
}

//-----------------------------------------------------------------------------
// Name : Render()
// Desc : renders the scroll bar to the screen
//-----------------------------------------------------------------------------
void ScrollBarUI::Render(Sprite sprites[SPRITES_SIZE], Sprite topSprites[SPRITES_SIZE], double timeStamp)
{
    if (m_bVisible)
    {
        // checks if one of the arrows buttons is being held or was clicked
        if( m_Arrow != CLEAR )
        {

            //TODO: find some miracle way to bring to this function the time....
            //double dCurrTime = timer->getCurrentTime();

            // check if the cursor on one of the arrow buttons if it is scrolls according to the defined delay and repeat times
            if (m_rcElements[UPBUTTON].isPointInRect(m_LastMouse))
            {
                switch( m_Arrow )
                {
                case CLICKED_UP:
                    if( SCROLLBAR_ARROWCLICK_DELAY < timeStamp - m_dArrowTS )
                    {
                        Scroll( -1 );
                        m_Arrow = HELD_UP;
                        m_dArrowTS = timeStamp;
                    }
                    break;
                case HELD_UP:
                    if( SCROLLBAR_ARROWCLICK_REPEAT < timeStamp - m_dArrowTS )
                    {
                        Scroll( -1 );
                        m_dArrowTS = timeStamp;
                    }
                    break;
                }
            }
            else if (m_rcElements[DOWNBUTTON].isPointInRect(m_LastMouse))
            {
                switch( m_Arrow )
                {
                case CLICKED_DOWN:
                    if( SCROLLBAR_ARROWCLICK_DELAY < timeStamp - m_dArrowTS )
                    {
                        Scroll( 1 );
                        m_Arrow = HELD_DOWN;
                        m_dArrowTS = timeStamp;
                    }
                    break;
                case HELD_DOWN:
                    if( SCROLLBAR_ARROWCLICK_REPEAT < timeStamp - m_dArrowTS )
                    {
                        Scroll( 1 );
                        m_dArrowTS = timeStamp;
                    }
                    break;
                }
            }
        } // ends check if arrow buttons were held or

        // prepare to render all the elements of the scrollbar and gets a pointer to the sprite
        if (m_elementsGFX.size() < 4)
            return;

        // render all the scrollbar elements
        for (GLuint i = 0; i < 4; i++)
        {
            //no texture was given abort rendering
            if (m_elementsGFX[i].texture.name == 0)
                return;

            Point dialogPos = m_pParentDialog->getLocation();
            long  dialogCaptionHeihgt =  m_pParentDialog->getCaptionHeight();
            dialogPos.y += dialogCaptionHeihgt;

            renderRect(sprites[NORMAL], m_rcElements[i], m_elementsGFX[i].texture, m_elementsGFX[i].rcTexture, WHITE_COLOR, dialogPos);
        }
    }
}

//-----------------------------------------------------------------------------
// Name : SetPageSize()
// Desc : handles mouse input
//-----------------------------------------------------------------------------
void ScrollBarUI::SetPageSize( int nPageSize )
{
    m_nPageSize = nPageSize;
    Cap();
    UpdateThumbRect();
}

//-----------------------------------------------------------------------------
// Name : SetTrackRange()
// Desc :
//-----------------------------------------------------------------------------
void ScrollBarUI::SetTrackRange( int nStart, int nEnd )
{
    m_nStart = nStart;
    m_nEnd   = nEnd;
    Cap();
    UpdateThumbRect();
}

//-----------------------------------------------------------------------------
// Name : SetTrackPos()
// Desc :
//-----------------------------------------------------------------------------
void ScrollBarUI::SetTrackPos( int nPosition )
{
    m_nPosition = nPosition;
    Cap();
    UpdateThumbRect();
}

//-----------------------------------------------------------------------------
// Name : GetTrackPos()
//-----------------------------------------------------------------------------
int ScrollBarUI::GetTrackPos()
{
    return m_nPosition;
}

//-----------------------------------------------------------------------------
// Name : GetPageSize()
//-----------------------------------------------------------------------------
int ScrollBarUI::GetPageSize()
{
    return m_nPageSize;
}

//-----------------------------------------------------------------------------
// Name : UpdateRects()
// Desc :
//-----------------------------------------------------------------------------
void ScrollBarUI::UpdateRects()
{
    ControlUI::UpdateRects();

    // sets the size of the up/down buttons and makes them a square
    m_rcElements[UPBUTTON] = Rect(m_rcBoundingBox.left, m_rcBoundingBox.top,
                                  m_rcBoundingBox.right, m_rcBoundingBox.top + ( m_rcBoundingBox.right - m_rcBoundingBox.left ));
    m_rcElements[DOWNBUTTON] = Rect(m_rcBoundingBox.left,
                                    m_rcBoundingBox.bottom - ( m_rcBoundingBox.right -m_rcBoundingBox.left ),
                                    m_rcBoundingBox.right, m_rcBoundingBox.bottom);
    m_rcElements[TRACK] = Rect(m_rcElements[UPBUTTON].left, m_rcElements[UPBUTTON].bottom,
                               m_rcElements[DOWNBUTTON].right, m_rcElements[DOWNBUTTON].top);

    m_rcElements[THUMB].left = m_rcElements[UPBUTTON].left;
    m_rcElements[THUMB].right = m_rcElements[UPBUTTON].right;

    UpdateThumbRect();
}

//-----------------------------------------------------------------------------
// Name : UpdateThumbRect()
// Desc :
//-----------------------------------------------------------------------------
void ScrollBarUI::UpdateThumbRect()
{
    if( m_nEnd - m_nStart > m_nPageSize )
    {
        int rcTrackHeight = m_rcElements[TRACK].bottom - m_rcElements[TRACK].top;

        int nThumbHeight = std::max(rcTrackHeight * m_nPageSize / ( m_nEnd - m_nStart ), SCROLLBAR_MINTHUMBSIZE );

        int nMaxPosition = m_nEnd - m_nStart - m_nPageSize;
        m_rcElements[THUMB].top = m_rcElements[TRACK].top + ( m_nPosition - m_nStart ) *
                ( rcTrackHeight - nThumbHeight ) / nMaxPosition;
        m_rcElements[THUMB].bottom = m_rcElements[THUMB].top + nThumbHeight;
        m_bShowThumb = true;

    }
    else
    {
        // No content to scroll
        m_rcElements[THUMB].bottom = m_rcElements[THUMB].top;
        m_bShowThumb = false;
    }
}

//-----------------------------------------------------------------------------
// Name : Cap()
// Desc :
//-----------------------------------------------------------------------------
void ScrollBarUI::Cap()
{
    if( m_nPosition < m_nStart || m_nEnd - m_nStart <= m_nPageSize )
    {
        m_nPosition = m_nStart;
    }
    else if( m_nPosition + m_nPageSize > m_nEnd )
        m_nPosition = m_nEnd - m_nPageSize;
}

//-----------------------------------------------------------------------------
// Name : Scroll
// desc : Scroll() scrolls by nDelta items.  A positive value scrolls down, while a negative
//		  value scrolls up.
//-----------------------------------------------------------------------------
void ScrollBarUI::Scroll( int nDelta )
{
    // Perform scroll
    m_nPosition += nDelta;

    // Cap position
    Cap();

    // Update thumb position
    UpdateThumbRect();
}

//-----------------------------------------------------------------------------
// Name : ShowItem
//-----------------------------------------------------------------------------
void ScrollBarUI::ShowItem( int nIndex )
{
    // Cap the index

    if( nIndex < 0 )
        nIndex = 0;

    if( nIndex >= m_nEnd )
        nIndex = m_nEnd - 1;

    // Adjust position

    if( m_nPosition > nIndex )
        m_nPosition = nIndex;
    else if( m_nPosition + m_nPageSize <= nIndex )
        m_nPosition = nIndex - m_nPageSize + 1;

    UpdateThumbRect();
}
