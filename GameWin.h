#ifndef  _GAMEWIN_H
#define  _GAMEWIN_H

// must be included first becuase of conflict between x11 and fbxsdk.h
#include "AssetManager.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
//#include <GL/glcorearb.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <X11/extensions/Xrandr.h>

#include <iostream>
#include <sstream>
#include <future>

#include "timer.h"
#include "Scene.h"
#include "input.h"
#include "Sprite.h"
#include "GUI/DialogUI.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

class GameWin
{
public:
    GameWin             ();
    virtual ~GameWin    ();
    
    bool initWindow     ();
    bool initOpenGL     (int width, int height);
    GLXFBConfig getBestFBConfig();
    bool createWindow(int width, int height ,GLXFBConfig bestFbc);
    bool createOpenGLContext(GLXFBConfig bestFbc);
        
    void setRenderStates();
    
    void drawing        ();
    
    void renderFPS      (Sprite& textSprite, mkFont& font);
    void reshape        (int width, int height);
    
    void ProcessInput   (double timeDelta);
    void setCursorPos   (Point newPos);
    Point getCursorPos();

    int  BeginGame      ();
    bool Shutdown       ();
    
    static bool isExtensionSupported    (const char *extList, const char *extension);
    static int  ctxErrorHandler         (Display *dpy, XErrorEvent *ev);

    static void copyToClipboard(const std::string& text);
    static std::string PasteClipboard();
    
protected:
    static void sendClipboardLoop(Window clipboardWindow);
    static void sendEventToXWindow(XSelectionRequestEvent *sev, Atom type, int bitsPerDataElement, unsigned char *data, int dataLength);
    
    virtual void initGUI();
    virtual void renderGUI();
    virtual bool handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual void sendKeyEvent(unsigned char key, bool down);
    virtual void sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates);
    virtual void sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual void onSizeChanged();
    
    

    Display * m_display;
    static Display * s_clipboardDisplay;
    static Atom s_utf8, s_targets, s_selection;
    static std::future<void> s_clipboardSender;
    static Window s_clipboardWindow;
    static std::string s_clipboardString;

    static bool ctxErrorOccurred;
    static const double s_doubleClickTime;

    Window m_win;
    
    GLuint m_winWidth;
    GLuint m_winHeight;

    int faceCount;
    int meshIndex;
    bool hit;

    Object* selectedObj;

    Atom wmDeleteMessage;
    
    GLXContext ctx;
    Colormap cmap;
    
    Cursor emptyCursorPixmap;

    bool gameRunning;
    
    Timer timer;
    double lastLeftClickTime;
    double lastRightClickTime;

    float m_hDpi;
    float m_vDpi;

    Scene* m_scene;

    Shader* spriteShader;
    Shader* spriteTextShader;

    bool keysStatus[256];
    Point oldCursorLoc;
    bool mouseDrag;

    AssetManager m_asset;
    mkFont* font_;
    Sprite m_sprites[2];
    //Sprite m_sprite;
    //Sprite m_textSprite;
    Sprite m_topSprites[2];
    //DialogUI m_dialog;
};

#endif  //_GAMEWIN_H
