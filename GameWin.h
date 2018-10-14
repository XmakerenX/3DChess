#ifndef  _GAMEWIN_H
#define  _GAMEWIN_H

#include <windows.h>
// must be included first becuase of conflict between x11 and fbxsdk.h
#include "AssetManager.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <future>

#include "timer.h"
#include "Scene.h"
#include "input.h"
#include "Sprite.h"
#include "GUI/DialogUI.h"

typedef BOOL(WINAPI *wglChoosePixelFormatARBProc)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC(WINAPI *wglCreateContextAttribsARBProc)(HDC hDC, HGLRC hshareContext, const int *attribList);
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092

#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_TYPE_RGBA_ARB 0x202B

#define WGL_SAMPLE_BUFFERS_ARB 0x2041
#define WGL_SAMPLES_ARB 0x2042

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
        Mode(GLuint _width, GLuint _height, GLuint _frequency)
        :width(_width), height(_height), frequency(_frequency)
        {}
        GLuint width;
        GLuint height;
        GLuint frequency;
    };
    
    MonitorInfo(int _index , const Rect& _positionRect, const std::vector<Mode>& _modes)
    :modes(_modes)
    {
        index = _index;
        positionRect = _positionRect;
    }
    
    MonitorInfo(int _index , const Rect _positionRect, std::vector<Mode>&& _modes)
    :modes(_modes)
    {
        index = _index;
        positionRect = _positionRect;
    }
    
    int index;
    Rect positionRect;
    std::vector<Mode> modes;
};

class GameWin
{
public:
    GameWin             ();
    virtual ~GameWin    ();
    
	LRESULT CALLBACK windowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
    bool initWindow     ();
    bool initOpenGL     (int width, int height, HINSTANCE hInstance);
    bool createWindow(int width, int height, HINSTANCE hInstance);
    bool createOpenGLContext(HINSTANCE hInstance);
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

    static void copyToClipboard(const std::string& text);
    static std::string PasteClipboard();
    
    std::vector<std::vector<Mode1>> getMonitorsModes() const;
    
protected:    
	static LRESULT CALLBACK staticWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
    void getMonitorsInfo();
    
    virtual void initGUI();
    virtual void renderGUI();
    virtual bool handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual void sendKeyEvent(unsigned char key, bool down);
    virtual void sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates);
    virtual void sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual void onSizeChanged();
    
    static std::string s_clipboardString;
    static const double s_doubleClickTime;
    
	HWND m_hWnd;
	HDC	m_hDC;
	HGLRC m_hRC;

    GLuint m_winWidth;
    GLuint m_winHeight;

    int faceCount;
    int meshIndex;
    bool hit;

    Object* selectedObj;

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
