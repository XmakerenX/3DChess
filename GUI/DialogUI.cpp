#ifndef  _DIALOGUI_CPP
#define  _DIALOGUI_CPP

#include "DialogUI.h"
#include "ListBoxUI.cpp"
#include "ComboBoxUI.cpp"
#include "../BaseGame.h"
#include <iostream>
#include <limits>

//-----------------------------------------------------------------------------
// Name : DialogUI (constructor)
//-----------------------------------------------------------------------------
DialogUI::DialogUI(void)
    :m_clipboardCopyFunc([](std::string x){}), m_clipboardPasteFunc([](){return std::string();})
{
    m_nCaptionHeight = 18;

    m_bVisible = true;
    m_bCaption = true;
    m_bMinimized = false;
    m_bDrag = false;

    m_x = 0;
    m_y = 0;

    m_width = 0;
    m_height = 0;
    m_texWidth = 0;
    m_texHeight = 0;

    m_texturePath[0] = '\0';
    m_captionText[0] = '\0';

    m_captionFont = nullptr;
    m_pMouseOverControl = nullptr;

    m_pControlFocus = nullptr;
    m_curControlID = 200;
}


//-----------------------------------------------------------------------------
// Name : DialogUI (destructor)
//-----------------------------------------------------------------------------
DialogUI::~DialogUI(void)
{
    for (GLuint i = 0; i < m_Controls.size(); i++)
    {
        if (m_Controls[i])
        {
            delete m_Controls[i];
            m_Controls[i] = nullptr; // just to be safe
        }
    }

    m_Controls.clear();
}

//-----------------------------------------------------------------------------
// Name : init ()
// Desc : initialize the dialog size , texture and caption
//-----------------------------------------------------------------------------
bool DialogUI::init(GLuint width, GLuint height, int nCaptionHeight, std::string captionText, std::string newTexturePath, glm::vec4 dialogColor, AssetManager& assetManger)
{
    GLuint textureName;

    setSize(width,height);
    m_rcBoundingBox = Rect(m_x, m_y, m_x + m_width, m_y + m_height);

    m_nCaptionHeight = nCaptionHeight;
    m_captionText = captionText;
    m_rcCaptionBox = Rect(m_x, m_y , m_x + m_width, m_y + m_nCaptionHeight);
    m_captionFont = assetManger.getFont("Times New Roman:bold", 12);

    // sets dialog background texture
    // checks first that there is a texutre to load
    if (newTexturePath.empty())
    {
        m_texWidth = 0;
        m_texHeight = 0;
    }
    else
    {
        // load the texture for the dialog background
        textureName = assetManger.getTexture(newTexturePath);
        // get the texture width and height
        if (glGetTextureLevelParameteriv != nullptr)
        {
            glGetTextureLevelParameteriv(textureName, 0, GL_TEXTURE_WIDTH, &m_texWidth);
            glGetTextureLevelParameteriv(textureName, 0, GL_TEXTURE_HEIGHT, &m_texHeight);
        }
        else
        {
            TextureInfo textureInfo = assetManger.getTextureInfo(textureName);
            m_texWidth = textureInfo.width;
            m_texHeight = textureInfo.height;
        }

        m_texturePath = newTexturePath;
    }

    m_dialogColor = dialogColor;

    //initWoodControlElements(assetManger);

    return true;
}

//-----------------------------------------------------------------------------
// Name : initDefControlElements ()
// Desc : initialize defualt textures and fonts used by GUI
//-----------------------------------------------------------------------------
bool DialogUI::initDefControlElements(AssetManager &assetManger)
{
    std::vector<ELEMENT_GFX>  elementGFXvec;
    std::vector<ELEMENT_FONT> elementFontVec;
    CONTROL_GFX	controlGFX;

    //-------------------------------------
    // Init Static elements
    //-------------------------------------
    // sets the Static default font
    // this font is also used for all other controls ... for now..
    // create the controls font
    mkFont* controlsFont = assetManger.getFont("Times New Roman", 12);
    if (!controlsFont)
        return false;

    ELEMENT_FONT elementFont2(FontInfo("Times New Roman",12), controlsFont);

    elementFontVec.push_back(elementFont2);
    elementGFXvec.clear();

    controlGFX.nControlType =  ControlUI::STATIC;
    controlGFX.elementsGFXvec = elementGFXvec;
    controlGFX.elementsFontVec = elementFontVec;
    m_defaultControlsGFX.push_back(controlGFX);

    //-------------------------------------
    // Init Button elements
    //-------------------------------------
    // loads the button default texture
    const std::vector<Rect> buttonTexturesRects = {Rect(1, 2, 134, 51) ,Rect(138, 3, 248, 50)};
    if (!initControlGFX(assetManger, ControlUI::BUTTON, "data/textures/GUI/tex.png", buttonTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init CheckBox elements
    //-------------------------------------
    const std::vector<Rect> checkboxTexturesRects = {Rect(0, 54, 27, 81 ), Rect(27, 54, 54, 81 )};
    if (!initControlGFX(assetManger, ControlUI::CHECKBOX, "data/textures/GUI/tex.png", checkboxTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init RadioButton elements
    //-------------------------------------
    const std::vector<Rect> radioButtonTexturesRects = {Rect(54, 54, 81, 81), Rect(81, 54, 108, 81)};
    if (!initControlGFX(assetManger, ControlUI::RADIOBUTTON, "data/textures/GUI/tex.png", radioButtonTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init ComboBox elements
    //-------------------------------------
    const std::vector<Rect> comboboxTexturesRects = {Rect(7, 81, 247, 123),   // Main textrue rect
                                                     Rect(98, 189, 151, 238), // Button textrue rect
                                                     Rect(13, 123, 241, 160), // Drop down textrue rect
                                                     Rect(12, 163, 239, 183)};// selection textrue rect

    if (!initControlGFX(assetManger, ControlUI::COMBOBOX, "data/textures/GUI/tex.png", comboboxTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init ScrollBar elements
    //-------------------------------------
    int nScrollBarStartX = 196;
    int nScrollBarStartY = 191;
    const std::vector<Rect> scrollBarTexturesRects = {Rect(nScrollBarStartX + 0, nScrollBarStartY + 21, nScrollBarStartX + 22, nScrollBarStartY + 32),   // Track textrue rect
                                                     Rect(nScrollBarStartX + 0, nScrollBarStartY + 1, nScrollBarStartX + 22, nScrollBarStartY + 21),  // Up Arrow textrue rect
                                                     Rect(nScrollBarStartX + 0, nScrollBarStartY + 32, nScrollBarStartX + 22, nScrollBarStartY + 53), // Down Arrow textrue rect
                                                    Rect(220, 192, 238, 234)};                                                                        // Button textrue rect

    if (!initControlGFX(assetManger, ControlUI::SCROLLBAR, "data/textures/GUI/tex.png", scrollBarTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init ListBox elements
    //-------------------------------------
    const std::vector<Rect> listTexturesRects = {Rect( 13, 123, 241, 160),      // Main texture rect
                                                     Rect( 16, 166, 240, 183)}; // Selection texture rect

    if (!initControlGFX(assetManger, ControlUI::LISTBOX, "data/textures/GUI/tex.png", listTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init Slider elements
    //-------------------------------------
    const std::vector<Rect> sliderTexturesRects = {Rect( 1, 187, 93, 228),       // Track texture rect
                                                     Rect( 151, 193, 192, 234)}; // Button texture rect

    if (!initControlGFX(assetManger, ControlUI::SLIDER, "data/textures/GUI/tex.png", sliderTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init EditBox elements
    //-------------------------------------
    const std::vector<Rect> editboxTexturesRects = {Rect( 14, 90, 241, 113),       // text area texture rect 0
                                                   Rect( 8, 82, 14, 90 ),         // top left border texture rect 1
                                                   Rect( 14, 82, 241, 90 ),       // top border 2
                                                   Rect( 241, 82, 246, 90 ),      // top right border 3
                                                   Rect( 8, 90, 14, 113 ),        // left border 4
                                                   Rect( 241, 90, 246, 113 ),     // right border 5
                                                   Rect( 8, 113, 14, 121 ),       // lower left border 6
                                                   Rect( 14, 113, 241, 121 ),     // lower border 7
                                                   Rect( 241, 113, 246, 121 )};   // lower right border 8

    if (!initControlGFX(assetManger, ControlUI::EDITBOX, "data/textures/GUI/tex.png", editboxTexturesRects, elementFontVec))
        return false;

    return true;
}

//-----------------------------------------------------------------------------
// Name : initWoodControlElements ()
//-----------------------------------------------------------------------------
bool DialogUI::initWoodControlElements(AssetManager& assetManager)
{
    std::vector<ELEMENT_GFX>  elementGFXvec;
    std::vector<ELEMENT_FONT> elementFontVec;
    CONTROL_GFX	controlGFX;

    //-------------------------------------
    // Init Static elements
    //-------------------------------------
    // sets the Static default font
    // this font is also used for all other controls ... for now..
    // create the controls font
    mkFont* controlsFont = assetManager.getFont("Times New Roman", 12);
    if (!controlsFont)
        return false;;

    ELEMENT_FONT elementFont2(FontInfo("Times New Roman",12), controlsFont);
    elementFontVec.push_back(elementFont2);
    elementGFXvec.clear();

    controlGFX.nControlType =  ControlUI::STATIC;
    controlGFX.elementsGFXvec = elementGFXvec;
    controlGFX.elementsFontVec = elementFontVec;
    m_defaultControlsGFX.push_back(controlGFX);

    //-------------------------------------
    // Init Button elements
    //-------------------------------------
    // loads our wood GUI texture
    const std::vector<Rect> buttonTexturesRects = {Rect(0, 0, 84, 34), Rect(0, 34, 84, 68)};
    if (!initControlGFX(assetManager, ControlUI::BUTTON, "data/textures/GUI/woodGUI.png", buttonTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init CheckBox elements
    //-------------------------------------
    const std::vector<Rect> checkboxTexturesRects = {Rect(0, 115, 15, 130 ), Rect(60, 112, 76, 130 )};
    if (!initControlGFX(assetManager, ControlUI::CHECKBOX, "data/textures/GUI/woodGUI.png", checkboxTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init RadioButton elements
    //-------------------------------------
    const std::vector<Rect> radioButtonTexturesRects = {Rect(1, 145, 19, 163), Rect(61, 145, 79, 163)};
    if (!initControlGFX(assetManager, ControlUI::RADIOBUTTON, "data/textures/GUI/woodGUI.png", radioButtonTexturesRects, elementFontVec))
        return false;
                                                    
    //-------------------------------------
    // Init ComboBox elements
    //-------------------------------------
    const std::vector<Rect> comboboxTexturesRects = {Rect(97, 0, 323, 34),   // Main textrue rect
                                                     Rect(323, 0, 367, 34), // Button textrue rect
                                                     Rect(93, 108, 320, 145), // Drop down textrue rect
                                                     Rect(93, 150, 320, 172)};// selection textrue rect

    if (!initControlGFX(assetManager, ControlUI::COMBOBOX, "data/textures/GUI/woodGUI.png", comboboxTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init ListBox elements
    //-------------------------------------                                                   
    const std::vector<Rect> listTexturesRects = {Rect( 93, 108, 320, 145),      // Main texture rect
                                                     Rect( 93, 150, 320, 172)}; // Selection texture rect

    if (!initControlGFX(assetManager, ControlUI::LISTBOX, "data/textures/GUI/woodGUI.png", listTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // Init Slider elements
    //-------------------------------------
    const std::vector<Rect> sliderTexturesRects = {Rect( 102, 84, 243, 90),       // Track texture rect
                                                     Rect( 243, 81, 259, 100)}; // Button texture rect

    if (!initControlGFX(assetManager, ControlUI::SLIDER, "data/textures/GUI/woodGUI.png", sliderTexturesRects, elementFontVec))
        return false;

    //-------------------------------------
    // EditBox
    //-------------------------------------
    const std::vector<Rect> editboxTexturesRects = {Rect( 91, 36, 321, 66),       // text area texture rect
                                                   Rect( 97, 34, 100, 37 ),         // top left border texture rect
                                                   Rect( 100, 34, 354, 36 ),       // top border
                                                   Rect( 354, 34, 357, 37 ),      // top right border
                                                   Rect( 97, 38, 99, 65 ),        // left border
                                                   Rect( 355, 37, 357, 65 ),     // right border
                                                   Rect( 97, 65, 100, 98 ),       // lower left border
                                                   Rect( 100, 66, 354, 68 ),     // lower border
                                                   Rect( 354, 65, 357, 68 )};   // lower right border

    if (!initControlGFX(assetManager, ControlUI::EDITBOX, "data/textures/GUI/woodGUI.png", editboxTexturesRects, elementFontVec))
        return false;
                                                                                                       
    //-------------------------------------
    // Init ScrollBar elements
    //-------------------------------------
    const std::vector<Rect> scrollBarTexturesRects = {Rect(329, 127, 350, 139),  // Track textrue rect
                                                      Rect(329, 107, 350, 126),  // Up Arrow textrue rect
                                                      Rect(329, 140, 350, 159),  // Down Arrow textrue rect
                                                      Rect(220, 192, 238, 234)}; // Button textrue rect 

    if (!initControlGFX(assetManager, ControlUI::SCROLLBAR, "data/textures/GUI/woodGUI.png", scrollBarTexturesRects, elementFontVec))
        return false;

    return true;
}

//-----------------------------------------------------------------------------
// Name : initControlGFX ()
//-----------------------------------------------------------------------------
bool DialogUI::initControlGFX(AssetManager &assetManger, ControlUI::CONTROLS controlType,std::string texturePath,const std::vector<Rect> textureRects,std::vector<ELEMENT_FONT>& elementFontVec)
{
    GLuint textureIndex = assetManger.getTexture(texturePath);
	TextureInfo textureInfo = assetManger.getTextureInfo(textureIndex);
    if (textureIndex == NO_TEXTURE)
        return false;

    std::vector<ELEMENT_GFX>  elementGFXvec;
    for (const Rect& textureElementRect : textureRects)
        elementGFXvec.emplace_back(Texture(textureIndex, textureInfo.width, textureInfo.height),textureElementRect);

    // create the button control default GFX and adds to the vector
    m_defaultControlsGFX.emplace_back(controlType, elementGFXvec, elementFontVec);

    return true;
}

//-----------------------------------------------------------------------------
// Name : OnRender ()
// Desc : renders the dialog and all of his controls
//-----------------------------------------------------------------------------
bool DialogUI::OnRender(Sprite sprites[ControlUI::SPRITES_SIZE], Sprite topSprites[ControlUI::SPRITES_SIZE], AssetManager& assetManger, double timeStamp)
{
    GLuint textureName = NO_TEXTURE;
	GLuint textureWidth = 0;
	GLuint textureHeight = 0;

    if (!m_bVisible)
        return true;

    if (!m_texturePath.empty())
    {
        textureName = assetManger.getTexture(m_texturePath);
        if (textureName == NO_TEXTURE)
            std::cout << "Failed to load the dialog texture " << m_texturePath << "\n";
        else
        {
            TextureInfo texInfo = assetManger.getTextureInfo(textureName);
            textureWidth = texInfo.width;
            textureHeight = texInfo.height;
        }
    }

    sprites[ControlUI::NORMAL].AddTintedTexturedQuad(m_rcBoundingBox, m_dialogColor, Texture(textureName, textureWidth, textureHeight));
    if (m_bCaption)
    {
        sprites[ControlUI::NORMAL].AddTintedQuad(m_rcCaptionBox, glm::vec4(0.78125f, 1.0f, 0.0f, 1.0f));
        if (m_captionFont != nullptr)
            m_captionFont->renderToRect(sprites[ControlUI::TEXT], m_captionText, m_rcCaptionBox, glm::vec4(0.0f,0.0f,0.0f,1.0f), mkFont::TextFormat::Center);
    }

    bool drawFocusedControl = false;
    for(GLuint i = 0; i < m_Controls.size(); i++)
    {
        if (m_Controls[i] != m_pControlFocus)
            m_Controls[i]->Render(sprites, topSprites, timeStamp);
    }

    //if (drawFocusedControl)
    if(m_pControlFocus != nullptr)
        m_pControlFocus->Render(sprites, topSprites, timeStamp);

    return true;
}

//-----------------------------------------------------------------------------
// Name : handleKeyEvent ()
//-----------------------------------------------------------------------------
bool DialogUI::handleKeyEvent(unsigned char key, bool down)
{
        // If a control is in focus, it belongs to this dialog, and it's enabled, then give
        // it the first chance at handling the message.
        if( m_pControlFocus && m_pControlFocus->getEnabled() )
        {
            // If the control handles it, then we don't.
            if (m_pControlFocus->handleKeyEvent(key, down))
                return true;
        }

        return false;
}

//-----------------------------------------------------------------------------
// Name : handleVirtualKeyEvent ()
//-----------------------------------------------------------------------------
bool DialogUI::handleVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates)
{
    // If a control is in focus, it belongs to this dialog, and it's enabled, then give
    // it the first chance at handling the message.
    if( m_pControlFocus && m_pControlFocus->getEnabled() )
    {
        // If the control handles it, then we don't.
        if (m_pControlFocus->handleVirtualKey(virtualKey, down, modifierStates))
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent ()
//-----------------------------------------------------------------------------
bool DialogUI::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    if (!m_bVisible)
        return false;

    Point cursorPos = Point(event.cursorPos.x - m_x, event.cursorPos.y - m_y - getCaptionHeight());    
    bool& down = event.down;
    //let controls handle the mouse event first
    // If a control is in focus and it's enabled, then give
    // it the first chance at handling the message.
    if( m_pControlFocus && m_pControlFocus->getEnabled() )
    {
        if( m_pControlFocus->handleMouseEvent(MouseEvent(event.type, cursorPos,
                                                         event.down, event.timeStamp, event.nLinesToScroll), modifierStates))
            return true;
    }

    for (GLuint i = 0; i < m_Controls.size(); i++)
    {
        if( m_Controls[i]->handleMouseEvent(MouseEvent(event.type, cursorPos,
                                                         event.down, event.timeStamp, event.nLinesToScroll), modifierStates))
        {
            return true;
        }
    }

    // the event was not for any of the controls in the dialog
    // the dialog will attemt to handle the event
    switch (event.type)
    {
    case MouseEventType::LeftButton:
    {
        if (down)
        {
            // Mouse not over any controls in this dialog since none of them handled the event
            // set no control to be in focus
            if(m_pControlFocus)
            {
                m_pControlFocus->OnFocusOut();
                m_pControlFocus = nullptr;
            }

            if (m_rcCaptionBox.isPointInRect(cursorPos))
            {
                m_bDrag = true;
                m_startDragPos = cursorPos;
                return true;
            }
        }
        else
        {
            if (m_bDrag)
            {
                m_bDrag = false;
                return true;
            }
        }
    }break;

    case MouseEventType::DoubleRightButton:
    {
        ControlUI* pControl = getControlAtPoint(cursorPos);
        if( pControl != nullptr && pControl->getEnabled() )
        {
            RemoveControl( pControl->getID() );
        }
    }break;

    case MouseEventType::RightButton:
    {
        if (down)
        {
            m_pControlFocus = getControlAtPoint(cursorPos);

            if (m_pControlFocus != nullptr)
            {
                m_controlRightClkSig(m_pControlFocus);
                return true;
            }
        }
    }break;

    case MouseEventType::MouseMoved:
    {
        // check if we need to highlight a control as the mouse is over it
        OnMouseMove(event.cursorPos);
        // return false to allow others to proceess this event
        return false;
    }break;
    
    default:
        return false;

    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : connectToControlRightClicked
//-----------------------------------------------------------------------------
void DialogUI::connectToControlRightClicked(const signal_controlClicked::slot_type& subscriber)
{
    m_controlRightClkSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : ClearRadioButtonGruop
// Desc : Clears the selection from the selected radio button group
//-----------------------------------------------------------------------------
void DialogUI::ClearRadioButtonGruop(GLuint nButtonGroup)
{
    for (GLuint i = 0; i < m_Controls.size(); i++)
    {
        if (m_Controls[i]->getType() == ControlUI::CONTROLS::RADIOBUTTON)
        {
            RadioButtonUI* curRadioButton = (RadioButtonUI*)m_Controls[i];
            if (curRadioButton->getButtonGroup() == nButtonGroup)
                curRadioButton->setChecked(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Name : OnMouseMove
// Desc : being called each time the mouse is moved
//        check if the mouse is over a control
//        if it is highlight that control.
//-----------------------------------------------------------------------------
void DialogUI::OnMouseMove(Point pt)
{
    if (m_bDrag)
    {
        int dx = pt.x - m_startDragPos.x;
        int dy = pt.y - m_startDragPos.y;

        setLocation(m_x + dx, m_y + dy);
        UpdateRects();

        m_startDragPos = pt;
    }
    else
    {
        pt.x -= m_x;
        pt.y -= m_y;
        pt.y -= getCaptionHeight();

        ControlUI* pControl = getControlAtPoint(pt);
        ControlUI* pPrevMouseOverControl = m_pMouseOverControl;

        if (pControl != pPrevMouseOverControl)
        {
            // if there was a control highlighted restore it to normal
            if (pPrevMouseOverControl)
            {
                pPrevMouseOverControl->onMouseLeave();
                m_pMouseOverControl = nullptr;
            }

            if (pControl != nullptr)
            {
                // sets the new mouseOverControl and highlight it
                m_pMouseOverControl = pControl;
                pControl->onMouseEnter();
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Name : getControlAtPoint
// Desc : check if the given point is inside
//        one of the controls that is in the dialog
//        and if found returns a pointer to that control
//-----------------------------------------------------------------------------
ControlUI* DialogUI::getControlAtPoint(Point pt)
{
    for (GLuint i = 0; i < m_Controls.size(); i++)
    {
        if (m_Controls[i]->getVisible() && m_Controls[i]->getEnabled())
        {
            if (m_Controls[i]->ContainsPoint(pt))
                return m_Controls[i];
        }
    }

    // if no control was found to have the point within it return null
    return nullptr;
}

//-----------------------------------------------------------------------------
// Name : addStatic
// Desc : adds a control of type static to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addStatic(int ID, const std::string& strText, int x, int y, int width, int height, StaticUI** ppStaticCreated/* = NULL*/, std::string strID /*= ""*/)
{
    m_curControlID++;
    //initialized the static control
    StaticUI* pStatic = new StaticUI(this ,ID, strText, x, y, width, height);

    initControl(pStatic);

    //add it to the controls vector
    m_Controls.push_back(pStatic);
    // add the control info the Definition Vector
    m_defInfo.push_back( DEF_INFO(strID, ID) );

    if (ppStaticCreated != NULL)
        *ppStaticCreated = pStatic;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addButton
// Desc : add a control of type button to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addButton(int ID, const std::string &strText, int x, int y, int width, int height, GLuint nHotkey, ButtonUI** ppButtonCreated/* = NULL*/, std::string strID /*= ""*/)
{
    m_curControlID++;
    //initialized the button control
    ButtonUI* pButton = new  ButtonUI(this ,ID, strText, x, y, width, height, nHotkey);

    initControl(pButton);

    //add it to the controls vector
    m_Controls.push_back(pButton);
    m_defInfo.push_back( DEF_INFO(strID, ID) );

    if (ppButtonCreated != NULL)
        *ppButtonCreated = pButton;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addCheckBox()
// Desc : add a control of type checkBox to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addCheckBox(int ID, int x, int y, int width, int height, GLuint nHotkey, CheckboxUI** ppCheckBoxCreated/* = NULL*/,  std::string strID /*= ""*/)
{
    m_curControlID++;
    //initialized the checkBox control
    CheckboxUI* pCheckBox = new CheckboxUI(this, ID, x, y, width, height, nHotkey);

    initControl(pCheckBox);

    //add it to the controls vector
    m_Controls.push_back(pCheckBox);
    m_defInfo.push_back( DEF_INFO(strID, ID) );

    if (ppCheckBoxCreated != NULL)
        *ppCheckBoxCreated = pCheckBox;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addRadioButton()
// Desc : add a control of type radio button to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addRadioButton(int ID, int x, int y, int width, int height, GLuint nHotkey, GLuint nButtonGroup, RadioButtonUI** ppRadioButtonCreated/* = NULL*/ ,std::string strID /*= ""*/)
{
    m_curControlID++;
    RadioButtonUI* pRadioButton = new RadioButtonUI(this, ID, x, y, width, height, nHotkey, nButtonGroup);

    initControl(pRadioButton);

    //add it to the controls vector
    m_Controls.push_back(pRadioButton);
    m_defInfo.push_back( DEF_INFO(strID, ID) );

    if (ppRadioButtonCreated != NULL)
        *ppRadioButtonCreated = pRadioButton;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addComboBox()
// Desc : add a control of type Combobox to the dialog
//-----------------------------------------------------------------------------
template<class T>
bool DialogUI::addComboBox(int ID, std::string strText, int x, int y, int width, int height, GLuint nHotkey, ComboBoxUI<T>** ppComboxCreated/* = NULL*/ , std::string strID /*= ""*/)
{
    m_curControlID++;
    ComboBoxUI<T>* pComboBox = new ComboBoxUI<T>(this, ID, strText, x, y, width, height, nHotkey);

    initControl(pComboBox);

    pComboBox->UpdateRects();

    //add it to the controls vector
    m_Controls.push_back(pComboBox);
    m_defInfo.push_back( DEF_INFO(strID, ID) );

    if (ppComboxCreated != NULL)
        *ppComboxCreated = pComboBox;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addListBox()
// Desc : add a control of type ListBox to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addListBox(int ID, int x, int y, int width, int height, bool multiSelection/* = false*/, ListBoxUI<int> **ppListBoxCreated/* = NULL*/, std::string strID /*= ""*/)
{
    m_curControlID++;
    ListBoxUI<int>* pListBox = new ListBoxUI<int>(this, ID, x, y, width, height, multiSelection);

    initControl(pListBox);
    //TODO: answer this !!!
    // !!! list box needs to update it's Rects on init
    // why?
    pListBox->UpdateRects();

    //add it to the controls vector
    m_Controls.push_back(pListBox);
    m_defInfo.push_back( DEF_INFO(strID, ID) );

    if (ppListBoxCreated != NULL)
        *ppListBoxCreated = pListBox;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addSlider()
// Desc : add a control of type Slider to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addSlider( int ID, int x, int y, int width, int height, int min, int max, int nValue, SliderUI** ppSliderCreated/* = NULL*/, std::string strID /*= ""*/ )
{
    m_curControlID++;
    SliderUI* pSlider = new SliderUI(this, ID, x, y, width, height, min, max, nValue);

    initControl(pSlider);
    // !!! slider needs to update it's Rects on init
    pSlider->UpdateRects();

    //add it to the controls vector
    m_Controls.push_back(pSlider);
    m_defInfo.push_back( DEF_INFO(strID, ID) );

    if (ppSliderCreated != NULL)
        *ppSliderCreated = pSlider;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addEditbox()
// Desc : add a control of type EditBox to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addEditbox( int ID, const std::string& strText, int x, int y, int width, int height, EditBoxUI** ppEditBoxCreated/* = NULL*/, std::string strID /*= ""*/)
{
    m_curControlID++;
    EditBoxUI* pEditBox = new EditBoxUI(this, ID, strText, x, y, width, height);

    initControl(pEditBox);
    // !!! EditBox needs to update it's Rects on init
    pEditBox->UpdateRects();

    //add it to the controls vector
    m_Controls.push_back(pEditBox);
    m_defInfo.push_back( DEF_INFO(strID, ID) );

    if (ppEditBoxCreated != NULL)
        *ppEditBoxCreated = pEditBox;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addStaticFromFile
// Desc : loads form file  a control of type static to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addStaticFromFile(std::istream& InputFIle, StaticUI** ppStaticCreated/* = NULL*/)
{
    //initialized the static control from file
    StaticUI* pStatic = new StaticUI(InputFIle);

    initControl(pStatic);

    //add it to the controls vector
    m_Controls.push_back(pStatic);

    if (ppStaticCreated != NULL)
        *ppStaticCreated = pStatic;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addButtonFromFile
// Desc : loads from file a control of type Button to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addButtonFromFile(std::istream& InputFIle, ButtonUI** ppButtonCreated /*= NULL*/)
{
    //initialized the button control from file
    ButtonUI* pButton = new  ButtonUI(InputFIle);

    initControl(pButton);

    //add it to the controls vector
    m_Controls.push_back(pButton);

    if (ppButtonCreated != NULL)
        *ppButtonCreated = pButton;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addCheckBoxFromFile()
// Desc : loads from file a control of type CheckBox to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addCheckBoxFromFile(std::istream& InputFIle, CheckboxUI** ppCheckBoxCreated /* = NULL */)
{
    //initialized the checkBox control from file
    CheckboxUI* pCheckBox = new CheckboxUI(InputFIle);

    initControl(pCheckBox);

    //add it to the controls vector
    m_Controls.push_back(pCheckBox);

    if (ppCheckBoxCreated != NULL)
        *ppCheckBoxCreated = pCheckBox;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addRadioButtonFromFile()
// Desc : loads from file a control of type RadioButton to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addRadioButtonFromFile(std::istream& InputFIle, RadioButtonUI** ppRadioButtonCreated /* = NULL */)
{
    RadioButtonUI* pRadioButton = new RadioButtonUI(InputFIle);

    initControl(pRadioButton);

    //add it to the controls vector
    m_Controls.push_back(pRadioButton);

    if (ppRadioButtonCreated != NULL)
        *ppRadioButtonCreated = pRadioButton;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addComboBoxFromFile()
// Desc : loads from file a control of type ComboBox to the dialog
//-----------------------------------------------------------------------------
template<class T>
bool DialogUI::addComboBoxFromFile(std::istream& InputFIle, ComboBoxUI<T>** ppComboxCreated /* = NULL */)
{
    ComboBoxUI<T>* pComboBox = new ComboBoxUI<T>(InputFIle);

    initControl(pComboBox);

    pComboBox->UpdateRects();

    //add it to the controls vector
    m_Controls.push_back(pComboBox);

    if (ppComboxCreated != NULL)
        *ppComboxCreated = pComboBox;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addListBoxFromFile()
// Desc : loads from file a control of type ListBox to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addListBoxFromFile(std::istream& InputFIle, ListBoxUI<int> **ppListBoxCreated /* = NULL */)
{
    ListBoxUI<int>* pListBox = new ListBoxUI<int>(InputFIle);

    initControl(pListBox);
    // !!! list box needs to update it's Rects on init
    pListBox->UpdateRects();

    //add it to the controls vector
    m_Controls.push_back(pListBox);

    if (ppListBoxCreated != NULL)
        *ppListBoxCreated = pListBox;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addSliderFromFile()
// Desc : loads from file a control of type Slider to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addSliderFromFile(std::istream& InputFIle, SliderUI** ppSliderCreated /* = NULL */)
{
    SliderUI* pSlider = new SliderUI(InputFIle);

    initControl(pSlider);
    // !!! slider needs to update it's Rects on init
    pSlider->UpdateRects();

    //add it to the controls vector
    m_Controls.push_back(pSlider);

    if (ppSliderCreated != NULL)
        *ppSliderCreated = pSlider;

    return true;
}

//-----------------------------------------------------------------------------
// Name : addEditBoxFromFile()
// Desc : loads from file a control of type EditBox to the dialog
//-----------------------------------------------------------------------------
bool DialogUI::addEditBoxFromFile(std::istream& InputFIle, EditBoxUI** ppEditBoxCreated /* = NULL */)
{
    EditBoxUI* pEditBox = new EditBoxUI(InputFIle);

    initControl(pEditBox);
    // !!! EditBox needs to update it's Rects on init
    pEditBox->UpdateRects();

    //add it to the controls vector
    m_Controls.push_back(pEditBox);

    if (ppEditBoxCreated != NULL)
        *ppEditBoxCreated = pEditBox;

    return true;
}

//-----------------------------------------------------------------------------
// Name : RemoveControl()
//-----------------------------------------------------------------------------
void DialogUI::RemoveControl(int ID)
{
    for (GLuint i = 0; i < m_Controls.size(); i++)
    {
        if (m_Controls[i]->getID() == ID)
        {
            // clear the focus reference to this control
            if (m_pControlFocus == m_Controls[i])
                ClearFocus();

            // clear the mouseOver reference to this control
            if (m_pMouseOverControl == m_Controls[i])
                m_pMouseOverControl = nullptr;

            // Deletes the control and removes it from the vecotr
            delete m_Controls[i];
            m_Controls[i] = nullptr;

            m_Controls.erase(m_Controls.begin() + i);

            // remove the reference from the def vector to this control
            for (GLuint j = 0; j < m_defInfo.size(); j++)
            {
                if (m_defInfo[j].controlID == ID )
                {
                    m_defInfo.erase(m_defInfo.begin() + j);
                    break;
                }
            }

            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Name : RemoveAllControls
//-----------------------------------------------------------------------------
void DialogUI::RemoveAllControls()
{
    if( m_pControlFocus && m_pControlFocus->getParentDialog() == this )
        m_pControlFocus = nullptr;

    m_pMouseOverControl = nullptr;

    for( GLuint i = 0; i < m_Controls.size(); i++ )
    {
        delete m_Controls[i];
        m_Controls[i] = nullptr;
    }

    m_Controls.clear();
}

//-----------------------------------------------------------------------------
// Name : initControl()
// Desc : initialize a control by giving it the default control settings
//-----------------------------------------------------------------------------
//TODO: remove the setParent command form this function
bool DialogUI::initControl(ControlUI* pControl)
{
    if (pControl == NULL)
        return false;

    // look for a default graphics for the control
    for (GLuint i = 0; i < m_defaultControlsGFX.size(); i++)
    {
        if (m_defaultControlsGFX[i].nControlType == pControl->getType())
        {
            pControl->setControlGFX(m_defaultControlsGFX[i].elementsGFXvec);
            pControl->setControlFonts(m_defaultControlsGFX[i].elementsFontVec);
            break;
        }
    }

    pControl->setParent(this);
    return pControl->onInit();
}

//-----------------------------------------------------------------------------
// Name : UpdateControlDefText()
//-----------------------------------------------------------------------------
void DialogUI::UpdateControlDefText(const std::string& strDefText, int controlID)
{
    for (GLuint i = 0; i < m_defInfo.size(); i++)
        if (m_defInfo[i].controlID == controlID)
        {
            m_defInfo[i].controlIDText = strDefText;
            break;
        }
}

//-----------------------------------------------------------------------------
// Name : UpdateControlDefText()
//-----------------------------------------------------------------------------
void DialogUI::UpdateControlDefText(std::string&& strDefText, int controlID)
{
    for (GLuint i = 0; i < m_defInfo.size(); i++)
        if (m_defInfo[i].controlID == controlID)
        {
            m_defInfo[i].controlIDText = strDefText;
            break;
        }
}

//-----------------------------------------------------------------------------
// Name : setSize()
//-----------------------------------------------------------------------------
void DialogUI::setSize(GLuint width, GLuint height)
{
    m_width = width;
    m_height = height;
    UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : setLocation()
//-----------------------------------------------------------------------------
void DialogUI::setLocation(int x, int y)
{
    m_x = x;
    m_y = y;
    UpdateRects();
}

//-----------------------------------------------------------------------------
// Name : setVisible()
//-----------------------------------------------------------------------------
void DialogUI::setVisible(bool bVisible)
{
    m_bVisible = bVisible;
}

//-----------------------------------------------------------------------------
// Name : setCaption()
//-----------------------------------------------------------------------------
void DialogUI::setCaption(bool bCaption)
{
    m_bCaption = bCaption;
}

//-----------------------------------------------------------------------------
// Name : UpdateRects()
// Desc : Update the dialog bounding box and it's caption box every tine the dialog
//        is moved
//-----------------------------------------------------------------------------
void DialogUI::UpdateRects()
{
    m_rcBoundingBox = Rect(m_x ,m_y, m_x + m_width, m_y + m_height);
    m_rcCaptionBox = Rect( m_x, m_y, m_x + m_width, m_y + m_nCaptionHeight);
}

//-----------------------------------------------------------------------------
// Name : getLocation()
//-----------------------------------------------------------------------------
Point DialogUI::getLocation()
{
    Point loc = {m_x,m_y};
    return loc;
}

//-----------------------------------------------------------------------------
// Name : getWidth()
//-----------------------------------------------------------------------------
GLuint DialogUI::getWidth()
{
    return m_width;
}

//-----------------------------------------------------------------------------
// Name : getHeight()
//-----------------------------------------------------------------------------
GLuint DialogUI::getHeight()
{
    return m_height;
}

//-----------------------------------------------------------------------------
// Name : getCaptionStartPoint()
//-----------------------------------------------------------------------------
long DialogUI::getCaptionHeight()
{
    if (m_bCaption)
        return m_nCaptionHeight;
    else
        return 0;
}


//-----------------------------------------------------------------------------
// Name : RequestFocus()
//-----------------------------------------------------------------------------
void DialogUI::RequestFocus( ControlUI* pControl)
{
    if( m_pControlFocus == pControl )
        return;

    if( !pControl->CanHaveFocus() )
        return;

    if( m_pControlFocus )
        m_pControlFocus->OnFocusOut();

    pControl->OnFocusIn();
    m_pControlFocus = pControl;
}

//-----------------------------------------------------------------------------
// Name : ClearFocus
//-----------------------------------------------------------------------------
void DialogUI::ClearFocus()
{
    if( m_pControlFocus )
    {
        m_pControlFocus->OnFocusOut();
        m_pControlFocus = NULL;
    }
}

//-----------------------------------------------------------------------------
// Name : SaveDilaogToFile
// Desc : Saves the dialog and all of his contorls to file
//-----------------------------------------------------------------------------
bool DialogUI::SaveDilaogToFile(const std::string& FileName, GLulong curControlID)
{
    std::ofstream saveFile,defFile;
    std::string defFileName = FileName;

    defFileName.resize(defFileName.size() - 4); //deleting the file extension (.xxx)
    defFileName = defFileName + "Def.h";

    defFile.open(defFileName.c_str());

    for (GLuint i = 0; i < m_Controls.size(); i++)
    {
        defFile << "#define " << m_defInfo[i].controlIDText << " " << m_defInfo[i].controlID << "\n";
    }

    defFile.close();

    saveFile.open(FileName);

    saveFile << m_width << "| Dialog Width" << "\n";
    saveFile << m_height << "| Dialog Height" << "\n";
    saveFile << m_nCaptionHeight << "| Dialog Caption Height" << "\n";
    saveFile << m_captionText << "| Dialog Caption Text" << "\n";
    saveFile << curControlID << "| Current Control ID" << "\n";

    saveFile << "//--------------------------------------------------------------------------\n";

    for (GLuint i = 0; i < m_Controls.size(); i++)
    {
        saveFile << m_Controls[i]->getType() << "| Control Type" << "\n";
        m_Controls[i]->SaveToFile(saveFile);

        saveFile << "//---------------------------------------------------------------------------\n";
    }

    saveFile.close();
    return true;
}

//-----------------------------------------------------------------------------
// Name : LoadDialogFromFile
// Desc : loads the dialog and all of his controls from file
//-----------------------------------------------------------------------------
bool DialogUI::LoadDialogFromFile(const std::string& FileName)
{
   std::ifstream inputFile;
   GLuint controlType;

   inputFile.open(FileName, std::ifstream::in);

   if (!inputFile.good())
   {
       std::cout << "Failed to open " << FileName << "\n";
       return false;
   }
   
   // clear controls on the dialog
   RemoveAllControls();

   //load the Dialog parameters
   inputFile >> m_width;
   inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
   inputFile >> m_height;
   inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
   inputFile >> m_nCaptionHeight;
   inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

   std::getline(inputFile, m_captionText, '|');
   inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

   m_curControlID = 0;
   inputFile >> m_curControlID;
   inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

   inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

   do
   {
       inputFile >> controlType;
       inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
       if (inputFile.eof() && !inputFile.fail())
           break;

       switch(controlType)
       {
       case ControlUI::STATIC:
           {
               addStaticFromFile(inputFile);
           }break;

       case ControlUI::BUTTON:
           {
               addButtonFromFile(inputFile);
           }break;

       case ControlUI::CHECKBOX:
           {
               addCheckBoxFromFile(inputFile);
           }break;

       case ControlUI::COMBOBOX:
           {
               addComboBoxFromFile<int>(inputFile);
           }break;

       case ControlUI::EDITBOX:
           {
               addEditBoxFromFile(inputFile);
           }break;

       case ControlUI::LISTBOX:
           {
               addListBoxFromFile(inputFile);
           }break;

       case ControlUI::RADIOBUTTON:
           {
               addRadioButtonFromFile(inputFile);
           }break;

       case ControlUI::SLIDER:
           {
               addSliderFromFile(inputFile);
           }
       }

       inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
   }while(!inputFile.eof() && !inputFile.fail());

   if(!inputFile.eof())
   {
       std::cout << "Error occured while reading " << FileName << "\n";
       inputFile.close();
       return false;
   }
   
   inputFile.close();

   m_defInfo.clear();

   std::ifstream inputDefFile;

   std::string defFileName;
   std::string strID, sID;
   int ID;

   defFileName = FileName;
   defFileName.resize(defFileName.size() - 4); //deleting the file extension (.xxx)
   defFileName = defFileName + "Def.h";

   inputDefFile.open(defFileName);
   if (!inputDefFile.good())
   {
       std::cout << "Failed to open " << defFileName << "\n";
       return false;
   }

   do
   {
       inputDefFile >> strID;

       if (inputDefFile.eof() && !inputDefFile.fail())
           break;

       inputDefFile >> strID;
       inputDefFile >> sID;
       ID = std::stoi(sID);

       m_defInfo.push_back( DEF_INFO(strID, ID) );

       inputDefFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
   }while(!inputDefFile.eof() && !inputDefFile.fail());

   if(!inputDefFile.eof())
   {
       std::cout << "Error occured while reading " << defFileName << "\n";
       inputDefFile.close();
       return false;
   }
   
   inputDefFile.close();
   UpdateRects();

   return true;
}

//-----------------------------------------------------------------------------
// Name : getControl
// Desc : searches for a control base on ID ONLY
//-----------------------------------------------------------------------------
ControlUI* DialogUI::getControl( int ID )
{
    // Try to find the control with the given ID
    for( GLuint i = 0; i < m_Controls.size(); i++ )
    {
        ControlUI* pControl = m_Controls[i];

        if( pControl->getID() == ID )
        {
            return pControl;
        }
    }

    // Not found
    return NULL;
}


//-----------------------------------------------------------------------------
// Name : getControl
// Desc : searches for a control base on ID and Control Type
//-----------------------------------------------------------------------------
ControlUI* DialogUI::getControl( int ID, GLuint nControlType )
{
    // Try to find the control with the given ID
    for( GLuint i = 0; i < m_Controls.size(); i++ )
    {
        ControlUI* pControl = m_Controls[i];

        if( pControl->getID() == ID && pControl->getType() == nControlType )
        {
            return pControl;
        }
    }

    // Not found
    return nullptr;
}

//-----------------------------------------------------------------------------
// Name : getStatic
//-----------------------------------------------------------------------------
StaticUI* DialogUI::getStatic( int ID )
{
    return static_cast< StaticUI* >(getControl( ID, ControlUI::STATIC ) );
}

//-----------------------------------------------------------------------------
// Name : getButton
//-----------------------------------------------------------------------------
ButtonUI* DialogUI::getButton( int ID )
{
    return static_cast< ButtonUI* > (getControl(ID, ControlUI::BUTTON) );
}

//-----------------------------------------------------------------------------
// Name : getCheckBox
//-----------------------------------------------------------------------------
CheckboxUI * DialogUI::getCheckBox( int ID )
{
    return static_cast< CheckboxUI* > ( getControl(ID, ControlUI::CHECKBOX) );
}

//-----------------------------------------------------------------------------
// Name : getRadioButton
//-----------------------------------------------------------------------------
RadioButtonUI * DialogUI::getRadioButton ( int ID )
{
    return static_cast< RadioButtonUI* > ( getControl(ID, ControlUI::RADIOBUTTON) );
}

//-----------------------------------------------------------------------------
// Name : GetComboBox
//-----------------------------------------------------------------------------
template<class T>
ComboBoxUI<T> * DialogUI::getComboBox( int ID )
{
    return static_cast<ComboBoxUI<T>*> (getControl(ID, ControlUI::COMBOBOX));
}
template ComboBoxUI<int>* DialogUI::getComboBox<int>(int);
template ComboBoxUI<Resolution>* DialogUI::getComboBox<Resolution>(int);

//-----------------------------------------------------------------------------
// Name : GetSlider
//-----------------------------------------------------------------------------
SliderUI * DialogUI::getSlider( int ID )
{
    return static_cast< SliderUI* > (getControl(ID, ControlUI::SLIDER));
}

//-----------------------------------------------------------------------------
// Name : GetEditBox
//-----------------------------------------------------------------------------
EditBoxUI * DialogUI::getEditBox( int ID )
{
    return static_cast<EditBoxUI*> (getControl(ID, ControlUI::EDITBOX));
}

//-----------------------------------------------------------------------------
// Name : GetListBox
//-----------------------------------------------------------------------------
ListBoxUI<int> * DialogUI::getListBox( int ID )
{
    return static_cast<ListBoxUI<int>*> (getControl(ID, ControlUI::LISTBOX));
}

//-----------------------------------------------------------------------------
// Name : GetControlsNum
//-----------------------------------------------------------------------------
int DialogUI::getControlsNum()
{
    return m_Controls.size();
}

//-----------------------------------------------------------------------------
// Name : getControlIDText
//-----------------------------------------------------------------------------
std::string DialogUI::getControlIDText(int ID)
{
    for (GLuint i = 0; i <m_defInfo.size(); i++)
    {
        if (m_defInfo[i].controlID == ID)
            return m_defInfo[i].controlIDText;
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Name : getVisible
//-----------------------------------------------------------------------------
bool DialogUI::getVisible()
{
    return m_bVisible;
}

//-----------------------------------------------------------------------------
// Name : getCurControlID
//-----------------------------------------------------------------------------
GLulong DialogUI::getCurControlID()
{
    return m_curControlID;
}

//-----------------------------------------------------------------------------
// Name : copyToClipboard
//-----------------------------------------------------------------------------
void DialogUI::copyToClipboard(std::string textToCopy)
{
    m_clipboardCopyFunc(textToCopy);
}

//-----------------------------------------------------------------------------
// Name : pasteFromClipboard
//-----------------------------------------------------------------------------
std::string DialogUI::pasteFromClipboard()
{
    return m_clipboardPasteFunc();
}

//-----------------------------------------------------------------------------
// Name : pasteFromClipboard
//-----------------------------------------------------------------------------
void DialogUI::setClipboardFunctions(std::function<void (std::string)> clipboardCopyFunc, std::function<std::string (void)> clipboardPasteFunc)
{
    m_clipboardCopyFunc = clipboardCopyFunc;
    m_clipboardPasteFunc = clipboardPasteFunc;
}

#endif  //_DIALOGUI_CPP
