#ifndef  _GAMEWIN_H
#define  _GAMEWIN_H

// must be included first becuase of conflict between x11 and fbxsdk.h
#include "AssetManager.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

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

struct Resolution
{
    Resolution(GLuint _width, GLuint _height)
    :width(_width), height(_height)
    {}
    
    GLuint width;
    GLuint height;
};

std::ostream& operator<<(std::ostream& os, const Resolution res);
std::istream& operator>>(std::istream& is, Resolution res);

struct Mode1
{
    Mode1(GLuint _width, GLuint _height)
    :width(_width), height(_height)
    {}
    
    GLuint width;
    GLuint height;
};

struct MonitorInfo
{
    struct Mode
    {
        Mode(RRMode _ID, GLuint _width, GLuint _height, GLuint _frequency)
        : ID(_ID), width(_width), height(_height), frequency(_frequency)
        {}
        RRMode ID;
        GLuint width;
        GLuint height;
        GLuint frequency;
    };
    
    MonitorInfo(int _index , const Rect& _positionRect, const std::vector<Mode>& _modes, const std::vector<RROutput>& _outputs)
    :modes(_modes), outputs(_outputs)
    {
        index = _index;
        positionRect = _positionRect;
    }
    
    MonitorInfo(int _index , const Rect _positionRect, std::vector<Mode>&& _modes, std::vector<RROutput>&& _outputs)
    :modes(_modes), outputs(_outputs)
    {
        index = _index;
        positionRect = _positionRect;
    }
    
    int index;
    Rect positionRect;
    std::vector<Mode> modes;
    std::vector<RROutput> outputs;
};

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
    void setFullScreenMode(bool fullscreen);    
    bool setMonitorResolution(int monitorIndex, Resolution newResolution);
    void setWindowPosition(int x, int y);
    void moveWindowToMonitor(int monitorIndex);
    
    void setRenderStates();
    
    void drawing        ();
    
    void renderFPS      (Sprite& textSprite, mkFont& font);
    void reshape        (int width, int height);
    
    void ProcessInput   (double timeDelta);
    void setCursorPos   (Point newPos);
    Point getCursorPos();

    int  BeginGame      ();
    bool Shutdown       ();
    
    Point getWindowPosition();
    GLuint getWindowCurrentMonitor();
    
    static bool isExtensionSupported    (const char *extList, const char *extension);
    static int  ctxErrorHandler         (Display *dpy, XErrorEvent *ev);

    static void copyToClipboard(const std::string& text);
    static std::string PasteClipboard();
    
    std::vector<std::vector<Mode1>> getMonitorsModes() const;
    
protected:
    static void sendClipboardLoop(Window clipboardWindow);
    static void sendEventToXWindow(XSelectionRequestEvent *sev, Atom type, int bitsPerDataElement, unsigned char *data, int dataLength);
    
    void getMonitorsInfo();
    
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
    bool m_sceneInput;

    Shader* spriteShader;
    Shader* spriteTextShader;

    bool keysStatus[256];
    Point oldCursorLoc;
    bool mouseDrag;

    AssetManager m_asset;
    mkFont* font_;
    Sprite m_sprites[2];
    
    
    std::vector<MonitorInfo> m_monitors;
    GLuint m_primaryMonitorIndex;
    
    Sprite m_topSprites[2];
};

#endif  //_GAMEWIN_H
