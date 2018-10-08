#include "StaticUI.h"
#include "DialogUI.h"
#include <type_traits>
//-----------------------------------------------------------------------------
// Name : StaticUI (Constructor)
//-----------------------------------------------------------------------------
StaticUI::StaticUI(DialogUI* pParentDialog, int ID, const std::string& strText, int x, int y, GLuint width, GLuint height)
    :ControlUI(pParentDialog, ID, x, y, width, height)
{
    //sets the Control type
    m_type = ControlUI::STATIC;
    m_strText = strText;
    //sets the color by Default to white
    m_textColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_textOrientation = mkFont::TextFormat::VerticalCenter;
}

//-----------------------------------------------------------------------------
// Name : StaticUI (constructor from InputFile)
//-----------------------------------------------------------------------------
StaticUI::StaticUI(std::istream& inputFile)
    :ControlUI(inputFile)
{
    m_type = STATIC;

    std::string controlText;

    std::getline(inputFile, controlText);
    controlText = controlText.substr(0, controlText.find('|') );
    m_strText = controlText;
    inputFile >> m_textColor.r;
    inputFile >> m_textColor.g;
    inputFile >> m_textColor.b;
    inputFile >> m_textColor.a;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    
    int textOrientation;
    inputFile >> textOrientation;
    m_textOrientation = static_cast<mkFont::TextFormat>(textOrientation);
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    
    //m_textOrientation = mkFont::TextFormat::VerticalCenter;
}

//-----------------------------------------------------------------------------
// Name : StaticUI (Destructor)
//-----------------------------------------------------------------------------
StaticUI::~StaticUI(void)
{
}

//-----------------------------------------------------------------------------
// Name : setTextColor ()
//-----------------------------------------------------------------------------
void StaticUI::setTextColor(glm::vec4 textColor)
{
    m_textColor = textColor;
}

//-----------------------------------------------------------------------------
// Name : setText ()
//-----------------------------------------------------------------------------
void StaticUI::setText(const std::string& text)
{
    m_strText = text;
}

//-----------------------------------------------------------------------------
// Name : setTextOrientation ()
//-----------------------------------------------------------------------------
void StaticUI::setTextOrientation(mkFont::TextFormat textOrientation)
{
    m_textOrientation = textOrientation;
}

//-----------------------------------------------------------------------------
// Name : getText ()
//-----------------------------------------------------------------------------
const std::string StaticUI::getText() const
{
    return m_strText;
}

//-----------------------------------------------------------------------------
// Name : Render ()
// Desc : Render the text store in the static to the screen
//-----------------------------------------------------------------------------
void StaticUI::Render(Sprite sprites[SPRITES_SIZE], Sprite topSprites[SPRITES_SIZE], double timeStamp)
{
    if (m_bVisible)
    {
        // if there is no text in the static no reason to render it
        if (m_strText[0] == '\0')
            return;

        //calculate the the static rendering rect
        Rect rcWindow(m_x, m_y, m_x + m_width, m_y + m_height);

        Point dialogPos = calcPositionOffset();

        if (m_elementsFonts.size() > 0)
            renderText(sprites[TEXT], m_elementsFonts[0].font, m_strText, m_textColor, rcWindow, dialogPos, m_textOrientation);
    }
}

//-----------------------------------------------------------------------------
// Name : SaveToFile ()
//-----------------------------------------------------------------------------
bool StaticUI::SaveToFile( std::ostream& SaveFile )
{
    ControlUI::SaveToFile(SaveFile);
    SaveFile << m_strText << "| Control Text" << "\n";
    SaveFile << m_textColor.r << " "<< m_textColor.g << " "<< m_textColor.b << " " << m_textColor.a << "| Control Text Color"
        << "\n";
    SaveFile << static_cast<std::underlying_type<mkFont::TextFormat>::type>(m_textOrientation) << "| Control text orientation\n";

    return true;
}
