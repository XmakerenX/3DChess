#ifndef  _DIALOGUI_H
#define  _DIALOGUI_H

#include <vector>
#include <string>
#include <fstream>
#include <boost/signals2/signal.hpp>
#include <boost/bind/bind.hpp>
#include "../AssetManager.h"
#include "ControlUI.h"
#include "ButtonUI.h"
#include "StaticUI.h"
#include "../virtualKeysGame.h"
#include "../mouseEventsGame.h"
#include "CheckboxUI.h"
#include "RadioButtonUI.h"
#include "ComboBoxUI.h"
#include "ListBoxUI.h"
//#include "SliderUI.h"
//#include "EditBoxUI.h"

// the callback function that used to send back GUI events to the main program
//typedef void ( CALLBACK*PCALLBACKGUIEVENT )(HWND hWnd, GLuint nEvent, int nControlID, void* pUserContext );

struct DEF_INFO
{
    DEF_INFO(std::string newControlIDText, GLuint newControlID)
    {
        controlIDText = newControlIDText;
        controlID = newControlID;
    }

    std::string controlIDText;
    GLuint controlID;
};

//struct that holds the default elements for a control type
struct CONTROL_GFX
{
    CONTROL_GFX()
    {
        nControlType = -1;
    }

    CONTROL_GFX(GLuint newControlType, std::vector<ELEMENT_GFX> &newElementsVec, std::vector<ELEMENT_FONT> newElementsFontVec)
    {
        nControlType = newControlType;
        elementsGFXvec = newElementsVec;
        elementsFontVec = newElementsFontVec;
    }

    GLuint nControlType;
    std::vector<ELEMENT_GFX>  elementsGFXvec;
    std::vector<ELEMENT_FONT> elementsFontVec;

};

class DialogUI
{
public:
    typedef boost::signals2::signal<void (ControlUI*)>  signal_controlClicked;

    DialogUI(void);
    virtual ~DialogUI(void);

    bool init    (GLuint width, GLuint height, int nCaptionHeight, std::string captionText, std::string newTexturePath, glm::vec4 dialogColor, AssetManager& assetManger);
    bool initControlGFX(AssetManager &assetManger, ControlUI::CONTROLS controlType, std::string texturePath, const std::vector<Rect> textureRects, std::vector<ELEMENT_FONT>& elementFontVec);


    bool initDefControlElements(AssetManager& assetManger);
    bool initWoodControlElements(AssetManager& assetManager);

    virtual bool    MsgProc	 (GLuint uMsg, Timer *timer, bool windowed );
    void	OnMouseMove(Point pt);

    bool handleKeyEvent(unsigned char key, bool down);
    bool handleVirtualKeyEvent(GK_VirtualKey virtualKey, bool down);
    bool handleMouseEvent(MouseEvent event);


    //void    SendEvent(GLuint nEvent, bool bTriggeredByUser, int nControlID, HWND hWnd = NULL );
    //void    SetCallback( PCALLBACKGUIEVENT pCallback);
    void    connectToControlRightClicked( const signal_controlClicked::slot_type& subscriber);

    virtual bool OnRender(Sprite &sprite, Sprite& textSprite, AssetManager& assetManger, double timeStamp);

    void	UpdateRects();

    //-------------------------------------------------------------------------
    // Functions that handle the Dialog Controls
    //-------------------------------------------------------------------------
    bool			 initControl			(ControlUI* pControl);
    void			 UpdateControlDefText	(std::string&& strDefText, int controlID);

    bool			 addStatic				( int ID, const std::string& strText, int x, int y, int width, int height, StaticUI** ppStaticCreated = NULL, std::string strID = "");
    bool             addButton              (int ID, const std::string& strText, int x, int y, int width, int height, GLuint nHotkey, ButtonUI **ppButtonCreated = NULL, std::string strID = "");
    bool			 addCheckBox			(int ID, int x, int y, int width, int height, GLuint nHotkey, CheckboxUI** ppCheckBoxCreated = NULL, std::string strID = "");
    bool			 addRadioButton			(int ID, int x, int y, int width, int height, GLuint nHotkey, GLuint nButtonGroup, RadioButtonUI** ppRadioButtonCreated = NULL, std::string strID = "");
    bool			 addComboBox			( int ID, std::string strText, int x, int y, int width, int height, GLuint nHotkey, ComboBoxUI** ppComboxCreated = NULL, std::string strID = "");
    bool		     addListBox				(int ID, int x, int y, int width, int height, GLuint style = 0, ListBoxUI** ppListBoxCreated = NULL, std::string strID = "");
//    bool		     addSlider				( int ID, int x, int y, int width, int height, int min, int max, int nValue, SliderUI** ppSliderCreated = NULL, std::string strID = "");
//    bool			 addEditbox				( int ID, std::string&& strText, int x, int y, int width, int height, CTimer* timer, EditBoxUI** ppEditBoxCreated = NULL, std::string strID = "");

    bool			 addStaticFromFile		( std::istream& InputFIle, StaticUI** ppStaticCreated = NULL);
    bool			 addButtonFromFile		( std::istream& InputFIle, ButtonUI** ppButtonCreated = NULL);
    bool			 addCheckBoxFromFile    ( std::istream& InputFIle, CheckboxUI** ppCheckBoxCreated = NULL);
    bool			 addRadioButtonFromFile ( std::istream& InputFIle, RadioButtonUI** ppRadioButtonCreated = NULL);
    bool			 addComboBoxFromFile    ( std::istream& InputFIle, ComboBoxUI** ppComboxCreated = NULL);
    bool			 addListBoxFromFile     ( std::istream& InputFIle, ListBoxUI** ppListBoxCreated = NULL);
//    bool			 addSliderFromFile      ( std::istream& InputFIle, SliderUI** ppSliderCreated = NULL);
//    bool			 addEditBoxFromFile     ( std::istream& InputFIle, Timer* timer, CEditBoxUI** ppEditBoxCreated = NULL);

    void			 RemoveControl			( int ID);
    void			 RemoveAllControls		( );

    ControlUI     * getControl				( int ID );
    ControlUI     * getControl			    (int ID, GLuint nControlType );
    StaticUI      * getStatic			    ( int ID );
    ButtonUI      * getButton		        ( int ID );
    CheckboxUI    * getCheckBox             ( int ID );
    RadioButtonUI * getRadioButton			( int ID );
    ComboBoxUI	   * getComboBox			( int ID );
    ListBoxUI	   * getListBox			    ( int ID );
//    SliderUI	   * getSlider				( int ID );
//    EditBoxUI	   * getEditBox				( int ID );
    int				 getControlsNum			();
    const char	   * getControlIDText		( int ID);
    bool			 getVisible				();

    void		     ClearRadioButtonGruop	( GLuint nButtonGroup);

    ControlUI	   * getControlAtPoint		( Point pt);

    void			 RequestFocus			( ControlUI* pControl );

    static void      ClearFocus();

    //-------------------------------------------------------------------------
    // Save/Load Functions
    //-------------------------------------------------------------------------
    bool				SaveDilaogToFile	(std::string &&FileName, GLulong curControlID);
    GLulong			    LoadDialogFromFile	(std::string &&FileName, Timer *timer);

    //-------------------------------------------------------------------------
    // get and set Functions
    //-------------------------------------------------------------------------
    void	setSize			 (GLuint width, GLuint height);
    void	setLocation		 (int x, int y);
    void	setVisible		 (bool bVisible);
    void	setCaption		 (bool bCaption);

    Point   getLocation	     ();
    GLuint	getWidth	     ();
    GLuint	getHeight		 ();

    long	getCaptionHeight ();

private:
    int  m_x, m_y;
    GLuint m_width;
    GLuint m_height;
    Rect m_rcBoundingBox;

    GLint m_texWidth;
    GLint m_texHeight;
    std::string m_texturePath;

    int m_nCaptionHeight;
    Rect m_rcCaptionBox;
    std::string m_captionText;
    mkFont* m_captionFont;

    glm::vec4 m_dialogColor;

    //TODO: delete the  vars that aren't needed for the drag operation
    Point m_startDragPos;
    Point m_curDragPos;

    bool m_bVisible;
    bool m_bCaption;
    bool m_bMinimized;
    bool m_bDrag;

    std::vector<ControlUI*> m_Controls;
    //Default graphics for the controls elements stuff like : textures,Rects
    std::vector<CONTROL_GFX> m_defaultControlsGFX;

    //TODO: map seems to fit defInfo use better..
    std::vector<DEF_INFO> m_defInfo;

    static ControlUI* m_pControlFocus; // The control which has focus
    ControlUI* m_pMouseOverControl;

    // Pointer to the callback event function that the dialog sends events to
    //PCALLBACKGUIEVENT m_pCallbackEvent;
    boost::signals2::signal<void (ControlUI*)> m_controlRightClkSig;
};

#endif  //_DIALOGUI_H
