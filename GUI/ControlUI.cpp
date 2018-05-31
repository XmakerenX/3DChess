#include "ControlUI.h"
#include "DialogUI.h"

//-----------------------------------------------------------------------------
// Name : ControlUI(Default constructor)
//-----------------------------------------------------------------------------
ControlUI::ControlUI(void)
{
    m_pParentDialog = nullptr;

    m_x = 0;
    m_y = 0;

    m_width  = 0;
    m_height = 0;

    m_ID = -1;

    m_bEnabled = true;
    m_bVisible   = true;
    m_bMouseOver = false;
    m_bHasFocus  = false;
}

//-----------------------------------------------------------------------------
// Name : ControlUI(constructor)
//-----------------------------------------------------------------------------
ControlUI::ControlUI(DialogUI* pParentDialog,int ID,int x, int y, GLuint width, GLuint height)
{
    m_ID = ID;

    setParent(pParentDialog);
    setLocation(x ,y);
    setSize(width,height);

    m_bEnabled = true;
    m_bVisible   = true;
    m_bMouseOver = false;
    m_bHasFocus  = false;
}


//-----------------------------------------------------------------------------
// Name : ControlUI(constructor from InputFile)
//-----------------------------------------------------------------------------
ControlUI::ControlUI(std::istream& inputFile)
{
    inputFile >> m_ID;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

    inputFile >> m_x;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_y;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_width;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_height;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

    inputFile >> m_bVisible;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    inputFile >> m_bEnabled;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

    m_bMouseOver = false;
    m_bHasFocus  = false;
}

//-----------------------------------------------------------------------------
// Name : ~ControlUI(destructor)
//-----------------------------------------------------------------------------
ControlUI::~ControlUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : onInit()
// Desc : called after the control is created by the dialog
//		  help to initialize sub controls of the control like the scrollbar
//-----------------------------------------------------------------------------
bool ControlUI::onInit()
{
    return true;
}

//-----------------------------------------------------------------------------
// Name : handleKeyEvent()
// Desc : handles keyboard input
// Note : returns false by default for the basic control class which means
//        the control does nothing with the keyboard input
//-----------------------------------------------------------------------------
bool ControlUI::handleKeyEvent(unsigned char key, bool down)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : handleVirtualKey()
// Desc : handles Virtual keys input
// Note : returns false by default for the basic control class which means
//        the control does nothing with the Virtual keys input
//-----------------------------------------------------------------------------
bool ControlUI::handleVirtualKey(GK_VirtualKey virtualKey, bool down)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent()
//-----------------------------------------------------------------------------
bool ControlUI::handleMouseEvent(MouseEvent event)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : HandleMouse()
// Desc : handles mouse input
// Note : returns false by default for the basic control class which means
//        the control does nothing with the mouse input
//-----------------------------------------------------------------------------
//bool CControlUI::HandleMouse( HWND hWnd, UINT uMsg, POINT mousePoint, INPUT_STATE inputstate, CTimer* timer )
//{
//	return false;
//}

//-----------------------------------------------------------------------------
// Name : Pressed()
//-----------------------------------------------------------------------------
bool ControlUI::Pressed(Point pt, INPUT_STATE inputState, double timeStamp)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : Released()
//-----------------------------------------------------------------------------
bool ControlUI::Released( Point pt)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : Dragged()
//-----------------------------------------------------------------------------
bool ControlUI::Dragged( Point pt)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : Scrolled()
//-----------------------------------------------------------------------------
bool ControlUI::Scrolled( int nScrollAmount)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : MsgProc()
// Desc : handles windows messages while control is focused
// Note : returns false by default for the basic control class which means
//        the control does nothing with the windows messages
//-----------------------------------------------------------------------------
//bool CControlUI::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam )
//{
//	return false;
//}

//-----------------------------------------------------------------------------
// Name : ContainsPoint()
// Desc : check if the given point is inside the control
//-----------------------------------------------------------------------------
bool ControlUI::ContainsPoint(Point pt)
{
    return m_rcBoundingBox.isPointInRect(pt);
//    if(m_rcBoundingBox.isPointInRect(pt))
//    if ((pt.x >= m_x) && (pt.x <= m_x + m_width) && (pt.y >= m_y) && (pt.y <= m_y + m_height))
//        return true;
//    else
//        return false;
}

//-----------------------------------------------------------------------------
// Name : onMouseEnter()
//-----------------------------------------------------------------------------
void ControlUI::onMouseEnter()
{
    m_bMouseOver = true;
}

//-----------------------------------------------------------------------------
// Name : onMouseLeave()
//-----------------------------------------------------------------------------
void ControlUI::onMouseLeave()
{
    m_bMouseOver = false;
}

//-----------------------------------------------------------------------------
// Name : SetID ()
//-----------------------------------------------------------------------------
void ControlUI::SetID(int ID)
{
    m_ID = ID;
}

//-----------------------------------------------------------------------------
// Name : setLocation ()
//-----------------------------------------------------------------------------
void ControlUI::setLocation(int x, int y)
{
    m_x = x;
    m_y = y;
    UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : setSize ()
//-----------------------------------------------------------------------------
void ControlUI::setSize(GLuint width, GLuint height)
{
    m_width = width;
    m_height = height;
    UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : setParent ()
//-----------------------------------------------------------------------------
void ControlUI::setParent(DialogUI* pParentDialog)
{
    m_pParentDialog = pParentDialog;
}

//-----------------------------------------------------------------------------
// Name : setControlGFX ()
//-----------------------------------------------------------------------------
void ControlUI::setControlGFX(std::vector<ELEMENT_GFX>& elementsGFX)
{
    m_elementsGFX = elementsGFX;
}

//-----------------------------------------------------------------------------
// Name : setControlFonts ()
//-----------------------------------------------------------------------------
void ControlUI::setControlFonts(std::vector<ELEMENT_FONT>& elementsFonts)
{
    m_elementsFonts = elementsFonts;
}

//-----------------------------------------------------------------------------
// Name : getType ()
//-----------------------------------------------------------------------------
GLuint ControlUI::getType()
{
    return m_type;
}

//-----------------------------------------------------------------------------
// Name : renderRect ()
// Desc : render a Rect using the given texture and texture Rect
// Note : the function also scale the texture to fit the Rect being rendered
//-----------------------------------------------------------------------------
void ControlUI::renderRect(Sprite& sprite, Rect& rcWindow, GLuint textureName, Rect& rcTexture ,glm::vec4 color, Point offset)
{
    Rect offsetedWindowRect(rcWindow);
    offsetedWindowRect.offset(offset);

    if (textureName != NO_TEXTURE)
        sprite.AddTintedTexturedQuad(offsetedWindowRect, color, textureName, rcTexture);
    else
        sprite.AddTintedQuad(offsetedWindowRect, color);
}

//-----------------------------------------------------------------------------
// Name : renderText ()
//-----------------------------------------------------------------------------
void ControlUI::renderText(Sprite &textSprite, mkFont *font, std::string text, glm::vec4 color, Rect& rcText, Point dialogPos, mkFont::TextFormat format/* = mkFont::TextFormat::Center*/)
{
    Rect rcTextRect(rcText);
    rcTextRect.offset(dialogPos.x, dialogPos.y);
    font->renderToRect(textSprite, text, rcTextRect, color, format);
}

//-----------------------------------------------------------------------------
// Name : RenderText ()
// Desc : Render the text store in the static to the screen
//-----------------------------------------------------------------------------
//void ControlUI::RenderText(const char strText[], RECT rcDest, LPD3DXFONT pFont, DWORD format, LPD3DXSPRITE pSprite, D3DCOLOR textColor, POINT offset)
//{
//    // checks the parameters are valid
//    if (!strText || !pFont ||  !pSprite || strText[0] == '\0')
//        return;

//    OffsetRect(&rcDest, offset.x, offset.y);

//    //pFont->DrawTextA(pSprite, strText, -1, &rcDest,/* DT_NOCLIP |*/ DT_CENTER | DT_VCENTER ,textColor);
//    pFont->DrawTextA(pSprite, strText, -1, &rcDest, format ,textColor);

//}

//-----------------------------------------------------------------------------
// Name : UpdateRects
//-----------------------------------------------------------------------------
void ControlUI::UpdateRects()
{
    m_rcBoundingBox = Rect(m_x, m_y, m_x + m_width, m_y + m_height);
}

//-----------------------------------------------------------------------------
// Name : CanHaveFocus
//-----------------------------------------------------------------------------
bool ControlUI::CanHaveFocus()
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : OnFocusIn
//-----------------------------------------------------------------------------
void ControlUI::OnFocusIn()
{
    m_bHasFocus = true;
}

//-----------------------------------------------------------------------------
// Name : OnFocusOut
//-----------------------------------------------------------------------------
void ControlUI::OnFocusOut()
{
    m_bHasFocus = false;
}

//-----------------------------------------------------------------------------
// Name : setEnabled
//-----------------------------------------------------------------------------
void ControlUI::setEnabled(bool bEnabled)
{
    m_bEnabled = bEnabled;
}

//-----------------------------------------------------------------------------
// Name : setVisible
//-----------------------------------------------------------------------------
void ControlUI::setVisible(bool bVisible)
{
    m_bVisible = bVisible;
}

//-----------------------------------------------------------------------------
// Name : getEnabled
//-----------------------------------------------------------------------------
bool ControlUI::getEnabled()
{
    return m_bEnabled;
}

//-----------------------------------------------------------------------------
// Name : getVisible
//-----------------------------------------------------------------------------
bool ControlUI::getVisible()
{
    return m_bVisible;
}

//-----------------------------------------------------------------------------
// Name : getParentDialog
//-----------------------------------------------------------------------------
DialogUI* ControlUI::getParentDialog()
{
    return m_pParentDialog;
}

//-----------------------------------------------------------------------------
// Name : getID
//-----------------------------------------------------------------------------
int ControlUI::getID()
{
    return m_ID;
}

//-----------------------------------------------------------------------------
// Name : getX
//-----------------------------------------------------------------------------
int ControlUI::getX()
{
    return m_x;
}

//-----------------------------------------------------------------------------
// Name : getY
//-----------------------------------------------------------------------------
int ControlUI::getY()
{
    return m_y;
}

//-----------------------------------------------------------------------------
// Name : getWidth
//-----------------------------------------------------------------------------
int ControlUI::getWidth()
{
    return m_width;
}

//-----------------------------------------------------------------------------
// Name : getHeight
//-----------------------------------------------------------------------------
int ControlUI::getHeight()
{
    return m_height;
}

//-----------------------------------------------------------------------------
// Name : SaveToFile
//-----------------------------------------------------------------------------
bool ControlUI::SaveToFile(std::ostream& SaveFile)
{
    SaveFile << m_type	  << "| Control Type"   << "\n";
    SaveFile << m_ID	  << "| Control ID"     << "\n";

    SaveFile << m_x       << "| Control X"	   << "\n";
    SaveFile << m_y	      << "| Control Y"	   << "\n";
    SaveFile << m_width   << "| Control Width"  <<  "\n";
    SaveFile << m_height  << "| Control Height" << "\n";

    SaveFile << m_bVisible << "| is Control Visible" << "\n";
    SaveFile << m_bEnabled << "| is Control Enabled" << "\n";

    return true;
}
