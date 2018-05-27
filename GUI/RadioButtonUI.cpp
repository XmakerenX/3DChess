#include "RadioButtonUI.h"
#include "DialogUI.h"

//-----------------------------------------------------------------------------
// Name : RadioButtonUI (Constructor)
//-----------------------------------------------------------------------------
RadioButtonUI::RadioButtonUI(DialogUI* pParentDialog, int ID, int x, int y, GLuint width, GLuint height, GLuint nHotkey, GLuint nButtonGruop)
    :CheckboxUI(pParentDialog, ID, x, y, width, height, nHotkey)
{
    m_type = RADIOBUTTON;
    m_nButtonGroup = nButtonGruop;
}

//-----------------------------------------------------------------------------
// Name : CRadioButtonUI (constructor from InputFile)
//-----------------------------------------------------------------------------
RadioButtonUI::RadioButtonUI(std::istream& inputFile)
    :CheckboxUI(inputFile)
{
    m_type = RADIOBUTTON;

    inputFile >> m_nButtonGroup;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
}
//-----------------------------------------------------------------------------
// Name : CRadioButtonUI (destructor)
//-----------------------------------------------------------------------------
RadioButtonUI::~RadioButtonUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent ()
//-----------------------------------------------------------------------------
bool RadioButtonUI::handleMouseEvent(MouseEvent event)
{
    if (!m_bEnabled || !m_bVisible)
        return false;

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
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed ()
//-----------------------------------------------------------------------------
bool RadioButtonUI::Pressed(Point pt, INPUT_STATE inputState, double timeStamp)
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
bool RadioButtonUI::Released(Point pt)
{
    if (m_bPressed)
    {
        m_bPressed = false;

        if (ContainsPoint(pt))
        {
            m_pParentDialog->ClearRadioButtonGruop(m_nButtonGroup);
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
bool RadioButtonUI::SaveToFile(std::ostream& SaveFile)
{
    CheckboxUI::SaveToFile(SaveFile);

    SaveFile << m_nButtonGroup << "| RadioButton Button Group" << "\n";

    return true;
}

//-----------------------------------------------------------------------------
// Name : getButtonGroup ()
//-----------------------------------------------------------------------------
GLuint RadioButtonUI::getButtonGroup()
{
    return m_nButtonGroup;
}

//-----------------------------------------------------------------------------
// Name : setChecked ()
//-----------------------------------------------------------------------------
void RadioButtonUI::setChecked(bool bChecked)
{
    m_bChecked = bChecked;
}

//-----------------------------------------------------------------------------
// Name : setButtonGroup ()
//-----------------------------------------------------------------------------
void RadioButtonUI::setButtonGroup(GLuint buttonGroup)
{
    m_nButtonGroup = buttonGroup;
}
