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
    
    void drawing        ();
    void reshape        (int width, int height);
    
    void ProcessInput   (double timeDelta);

    int  BeginGame      ();
    bool Shutdown       ();
    
    static bool isExtensionSupported    (const char *extList, const char *extension);
    static int  ctxErrorHandler         (Display *dpy, XErrorEvent *ev);

    static void copyToClipboard(const std::string& text);
    static std::string PasteClipboard();
    
private:
    static void sendClipboardLoop(Window clipboardWindow);
    static void sendEventToXWindow(XSelectionRequestEvent *sev, Atom type, int bitsPerDataElement, unsigned char *data, int dataLength);

    Display * m_display;
    static Display * clipboardDisplay;
    static Atom s_utf8, s_targets, s_selection;
    static std::future<void> clipboardSender;
    static Window clipboardWindow;
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

    int height_;
    float m_hDpi;
    float m_vDpi;

    Scene m_scene;

    Shader* textShader;
    Shader* textureShader;
    Shader* spriteShader;
    Shader* spriteTextShader;

    bool keysStatus[256];
    Point oldCursorLoc;
    bool mouseDrag;

    AssetManager m_asset;
    mkFont* font_;
    Sprite m_sprite;
    Sprite m_textSprite;
    DialogUI m_dialog;

    //crap to delete
    GLuint VAO;
    GLuint VBO;
    VertexIndex indices[6];

};

#endif  //_GAMEWIN_H
