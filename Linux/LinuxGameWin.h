#ifndef  _LINUXGAMEWIN_H
#define  _LINUXGAMEWIN_H

#include "../BaseGameWin.h"
#include <future>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

#include <GL/glx.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

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

class LinuxGameWin : public BaseGameWin
{
public:
    LinuxGameWin();
    virtual ~LinuxGameWin();
    
    int  BeginGame      ();
    bool Shutdown       ();
    
    void setFullScreenMode(bool fullscreen);
    bool setMonitorResolution(int monitorIndex, Resolution newResolution);
    void setWindowPosition(int x, int y);
    void moveWindowToMonitor(int monitorIndex);

    void setCursorPos(Point newPos);
    Point getCursorPos();

    Point getWindowPosition();
    GLuint getWindowCurrentMonitor();
    
    static void copyToClipboard(const std::string& text);
    static std::string PasteClipboard();
    
    std::vector<std::vector<Mode1>> getMonitorsModes() const;

private:
    static bool isExtensionSupported    (const char *extList, const char *extension);
    static int  ctxErrorHandler         (Display *dpy, XErrorEvent *ev);
    
    static void sendClipboardLoop(Window clipboardWindow);
    static void sendEventToXWindow(XSelectionRequestEvent *sev, Atom type, int bitsPerDataElement, unsigned char *data, int dataLength);
    
    static Display * s_clipboardDisplay;
    static Atom s_utf8, s_targets, s_selection;
    static std::future<void> s_clipboardSender;
    static Window s_clipboardWindow;
    static std::string s_clipboardString;

    static bool ctxErrorOccurred;
    static const double s_doubleClickTime;
    
    bool platformInit   (int width, int height);
    bool initDisplay();
    GLXFBConfig getBestFBConfig();
    bool createWindow(int width, int height ,GLXFBConfig bestFbc);
    bool createEmptyCursorPixmap();
    bool createOpenGLContext(GLXFBConfig bestFbc);
    
    void glSwapBuffers();
    void getMonitorsInfo();
    
    Display * m_display;
    Window m_win;
    Atom wmDeleteMessage;
    GLXContext ctx;
    Colormap cmap;    
    Cursor emptyCursorPixmap;

    double lastLeftClickTime;
    double lastRightClickTime; 
    
    std::vector<MonitorInfo> m_monitors;
    GLuint m_primaryMonitorIndex;    
};

#endif  //_LINUXGAMEWIN_H
