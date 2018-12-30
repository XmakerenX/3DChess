#include "SliderUI.h"
#include "DialogUI.h"

//-----------------------------------------------------------------------------
// Name : SliderUI(constructor)
//-----------------------------------------------------------------------------
SliderUI::SliderUI(DialogUI* pParentDialog, int ID, int x, int y, int width, int height, int min, int max, int nValue )
    :ControlUI(pParentDialog, ID, x, y, width ,height)
{
    m_type = ControlUI::SLIDER;
    m_nMin = min;
    m_nMax = max;
    m_nValue = nValue;
    m_bPressed = false;
}

//-----------------------------------------------------------------------------
// Name : SliderUI(constructor from InputFile)
//-----------------------------------------------------------------------------
SliderUI::SliderUI(std::istream& inputFile)
    :ControlUI(inputFile)
{
    m_type = ControlUI::SLIDER;

    inputFile >> m_nMin;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_nMax;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

    m_nValue = (m_nMax - m_nMin) / 2;

    m_bPressed = false;
}

//-----------------------------------------------------------------------------
// Name : SliderUI(destructor)
//-----------------------------------------------------------------------------
SliderUI::~SliderUI(void)
{}

//-----------------------------------------------------------------------------
// Name : ContainsPoint() 
//-----------------------------------------------------------------------------
bool SliderUI::ContainsPoint( Point pt )
{
    return (m_rcBoundingBox.isPointInRect(pt) || m_rcButton.isPointInRect(pt));
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent()
//-----------------------------------------------------------------------------
bool SliderUI::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch(event.type)
    {
    case MouseEventType::LeftButton:
    {
        if(event.down)
        {
            if ( Pressed(event.cursorPos, modifierStates, event.timeStamp))
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

    case MouseEventType::ScrollVert:
    {
        if ( Scrolled( event.nLinesToScroll ) )
            return true;
    }break;

    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed() 
//-----------------------------------------------------------------------------
bool SliderUI::Pressed( Point pt, const ModifierKeysStates &modifierStates, double timeStamp)
{
    if (m_rcButton.isPointInRect(pt))
    {
        // Pressed while inside the control
        m_bPressed = true;

        m_nDragX = pt.x;
        m_nDragOffset = m_nButtonX - m_nDragX;

        if( !m_bHasFocus )
            m_pParentDialog->RequestFocus( this );

        return true;
    }

    if (m_rcBoundingBox.isPointInRect(pt))
    {
        m_nDragX = pt.x;
        m_nDragOffset = 0;
        m_bPressed = true;
        
        if( !m_bHasFocus )
            m_pParentDialog->RequestFocus( this );

        if( pt.x > m_nButtonX + m_x )
        {
            SetValueInternal( m_nValue + 1, true );
            return true;
        }

        if( pt.x < m_nButtonX + m_x )
        {
            SetValueInternal( m_nValue - 1, true );
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Name : Released() 
//-----------------------------------------------------------------------------
bool SliderUI::Released( Point pt)
{
    if( m_bPressed )
    {
        m_bPressed = false;
        m_sliderChangedSig(this);
        return true;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : Dragged() 
//-----------------------------------------------------------------------------
bool SliderUI::Dragged( Point pt)
{
    if( m_bPressed )
    {
        SetValueInternal( ValueFromPos( m_x + pt.x + m_nDragOffset ), true );
        return true;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : Scrolled() 
//-----------------------------------------------------------------------------
bool SliderUI::Scrolled( int nScrollAmount)
{
    if (m_bMouseOver)
    {
        SetValueInternal( m_nValue - nScrollAmount, true );
        return true;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : connectToSliderChg()
//-----------------------------------------------------------------------------
void SliderUI::connectToSliderChg( const signal_slider::slot_type& subscriber)
{
    m_sliderChangedSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : UpdateRects() 
//-----------------------------------------------------------------------------
void SliderUI::UpdateRects()
{
    ControlUI::UpdateRects();

    m_rcButton = m_rcBoundingBox;
    m_rcButton.right = m_rcButton.left + ( m_rcButton.bottom - m_rcButton.top );
    m_rcButton.offset( -( m_rcButton.right - m_rcButton.left ) / 2, 0);

    m_nButtonX = ( int )( ( m_nValue - m_nMin ) * ( float )( m_rcBoundingBox.right - m_rcBoundingBox.left ) / ( m_nMax - m_nMin ) );
    m_rcButton.offset(m_nButtonX, 0);
}

//-----------------------------------------------------------------------------
// Name : SaveToFile() 
//-----------------------------------------------------------------------------
bool SliderUI::SaveToFile(std::ostream& SaveFile)
{
    ControlUI::SaveToFile(SaveFile);

    SaveFile << m_nMin << "| Slider Minimum Value" << "\n";
    SaveFile << m_nMax << "| Slider Maximum Value" << "\n";

    return true;
}

//-----------------------------------------------------------------------------
// Name : Render() 
//-----------------------------------------------------------------------------
void SliderUI::Render(Sprite sprites[SPRITES_SIZE], Sprite topSprites[SPRITES_SIZE], double timeStamp)
{
    if (!m_bVisible)
        return;

    //no texture was given abort rendering
    if (m_elementsGFX.size() < 2 ||m_elementsGFX[TRACK].texture.name == 0 || m_elementsGFX[BUTTON].texture.name == 0)
        return;

    Point dialogPos = m_pParentDialog->getLocation();

    if (m_bMouseOver)
    {
        renderRect(sprites[NORMAL], m_rcBoundingBox, m_elementsGFX[TRACK].texture, m_elementsGFX[TRACK].rcTexture, WHITE_COLOR, dialogPos);
        renderRect(sprites[NORMAL], m_rcButton, m_elementsGFX[BUTTON].texture, m_elementsGFX[BUTTON].rcTexture, WHITE_COLOR, dialogPos);
    }
    else
    {
        renderRect(sprites[NORMAL], m_rcBoundingBox, m_elementsGFX[TRACK].texture, m_elementsGFX[TRACK].rcTexture, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), dialogPos);
        renderRect(sprites[NORMAL], m_rcButton, m_elementsGFX[BUTTON].texture, m_elementsGFX[BUTTON].rcTexture, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), dialogPos);
    }
}

//-----------------------------------------------------------------------------
// Name : SetValue() 
//-----------------------------------------------------------------------------
void SliderUI::SetValue( int nValue )
{
    SetValueInternal( nValue, false );
}

//-----------------------------------------------------------------------------
// Name : GetValue() 
//-----------------------------------------------------------------------------
int SliderUI::GetValue() const
{
    return m_nValue;
}

//-----------------------------------------------------------------------------
// Name : GetRange() 
//-----------------------------------------------------------------------------
void SliderUI::GetRange( int& nMin, int& nMax ) const
{
    nMin = m_nMin;
    nMax = m_nMax;
}

//-----------------------------------------------------------------------------
// Name : SetRange() 
//-----------------------------------------------------------------------------
void SliderUI::SetRange( int nMin, int nMax )
{
    m_nMin = nMin;
    m_nMax = nMax;

    SetValueInternal( m_nValue, false );
}

//-----------------------------------------------------------------------------
// Name : SetValueInternal()
//-----------------------------------------------------------------------------
void SliderUI::SetValueInternal( int nValue, bool bFromInput )
{
    // Clamp to range
    nValue = std::max( m_nMin, nValue );
    nValue = std::min( m_nMax, nValue );

    if( nValue == m_nValue )
        return;

    m_nValue = nValue;
    UpdateRects();

    m_sliderChangedSig(this);
}

//-----------------------------------------------------------------------------
// Name : ValueFromPos()
//-----------------------------------------------------------------------------
int SliderUI::ValueFromPos( int x )
{
    float fValuePerPixel = ( float )( m_nMax - m_nMin ) / ( m_rcBoundingBox.right - m_rcBoundingBox.left );
    return ( int )( 0.5f + m_nMin + fValuePerPixel * ( x - m_rcBoundingBox.left ) );
}

//-----------------------------------------------------------------------------
// Name : CanHaveFocus()
//-----------------------------------------------------------------------------
bool SliderUI::CanHaveFocus()
{
    return ( m_bVisible && m_bEnabled );
}
