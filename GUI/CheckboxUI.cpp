#include "CheckboxUI.h"
#include "DialogUI.h"

//-----------------------------------------------------------------------------
// Name : CheckboxUI (Constructor)
//-----------------------------------------------------------------------------
CheckboxUI::CheckboxUI(DialogUI* pParentDialog, int ID, int x, int y, GLuint width, GLuint height, GLuint nHotkey)
:ButtonUI(pParentDialog, ID, "", x, y, width, height, nHotkey)
{
    m_type = CHECKBOX;
    m_bChecked = false;
}

//-----------------------------------------------------------------------------
// Name : CheckboxUI (constructor from InputFile)
//-----------------------------------------------------------------------------
CheckboxUI::CheckboxUI(std::istream& inputFile)
    :ButtonUI(inputFile)
{
    m_type = CHECKBOX;

    inputFile >> m_bChecked;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
}

//-----------------------------------------------------------------------------
// Name : CCheckboxUI (destructor)
//-----------------------------------------------------------------------------
CheckboxUI::~CheckboxUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : Render ()
//-----------------------------------------------------------------------------
void CheckboxUI::Render(Sprite& sprite, Sprite& textSprite, double timeStamp)
{
    if (m_bVisible)
    {
        //no texture was given abort rendering
        if (m_elementsGFX.size() < 2 || m_elementsGFX[BUTTON].iTexture == -1 || m_elementsGFX[MOUSEOVER].iTexture == -1)
            return;

        Point dialogPos = m_pParentDialog->getLocation();
        long  dialogCaptionHeihgt =  m_pParentDialog->getCaptionHeight();
        dialogPos.y += dialogCaptionHeihgt;

        //calculate the the button rendering rect
        Rect rcWindow(m_x, m_y, m_x +  m_width, m_y + m_height);

        if (!m_bMouseOver)
        {
            if (m_bEnabled)
            {
                renderRect(sprite, rcWindow, m_elementsGFX[BUTTON].iTexture, m_elementsGFX[BUTTON].rcTexture, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), dialogPos);
                if (m_bChecked)
                    renderRect(sprite,rcWindow, m_elementsGFX[MOUSEOVER].iTexture, m_elementsGFX[MOUSEOVER].rcTexture, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), dialogPos);
            }
            else
            {
                renderRect(sprite, rcWindow, m_elementsGFX[BUTTON].iTexture, m_elementsGFX[BUTTON].rcTexture, glm::vec4(0.8f, 0.8f, 0.8f,1.0f), dialogPos);
                if (m_bChecked)
                    renderRect(sprite, rcWindow, m_elementsGFX[MOUSEOVER].iTexture, m_elementsGFX[MOUSEOVER].rcTexture, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), dialogPos);
            }
        }
        else
        {
            // if the button is pressed and the cursor is on it darken it to showed it is pressed
            if (m_bMouseOver && m_bPressed)
            {
                renderRect(sprite, rcWindow, m_elementsGFX[BUTTON].iTexture, m_elementsGFX[BUTTON].rcTexture, glm::vec4(0.6f, 0.6f,0.6f,1.0f), dialogPos);
                if (m_bChecked)
                    renderRect(sprite, rcWindow, m_elementsGFX[MOUSEOVER].iTexture, m_elementsGFX[MOUSEOVER].rcTexture, glm::vec4(0.6f, 0.6f, 0.6f,1.0f), dialogPos);
            }
            else
                // if the button has the cursor on it high light
                if (m_bMouseOver)
                {
                    renderRect(sprite, rcWindow, m_elementsGFX[BUTTON].iTexture, m_elementsGFX[BUTTON].rcTexture, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), dialogPos);
                    if (m_bChecked)
                        renderRect(sprite, rcWindow ,m_elementsGFX[MOUSEOVER].iTexture, m_elementsGFX[MOUSEOVER].rcTexture, glm::vec4(1.0f, 1.0f, 1.0f,1.0f), dialogPos);
                }
        }
    }
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent ()
//-----------------------------------------------------------------------------
bool CheckboxUI::handleMouseEvent(MouseEvent event)
{
    if (!m_bEnabled || !m_bVisible)
        return false;

    switch (event.type)
    {
    case MouseEventType::LeftButton:
    {
        if (event.down)
        {
            if ( Pressed(event.cursorPos, INPUT_STATE(), event.timeStamp) )
                return true;
        }
        else
        {
            if (Released(event.cursorPos))
                    return true;
        }
    }break;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed ()
//-----------------------------------------------------------------------------
bool CheckboxUI::Pressed(Point pt, INPUT_STATE inputState, double timeStamp)
{
    if ( ContainsPoint( pt ) )
    {
        m_bPressed = true;

        if( !m_bHasFocus )
            m_pParentDialog->RequestFocus( this );

        return true;

    }
    return false;
}

//-----------------------------------------------------------------------------
// Name : Released ()
//-----------------------------------------------------------------------------
bool CheckboxUI::Released(Point pt)
{
    if (m_bPressed)
    {
        m_bPressed = false;

        if (ContainsPoint(pt))
        {
            m_bChecked = !m_bChecked;
            m_clickedSig(this);
        }

        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Name : SaveToFile ()
//-----------------------------------------------------------------------------
bool CheckboxUI::SaveToFile(std::ostream& SaveFile)
{
    ButtonUI::SaveToFile(SaveFile);
    SaveFile << m_bChecked << "| is CheckBox Checked" << "\n";

    return true;
}

//-----------------------------------------------------------------------------
// Name : getChecked ()
//-----------------------------------------------------------------------------
bool CheckboxUI::getChecked()
{
    return m_bChecked;
}
