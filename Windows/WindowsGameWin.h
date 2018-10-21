#ifndef  _WINDOWSGAMEWIN_H
#define  _WINDOWSGAMEWIN_H

#include <windows.h>
#undef min
#undef max
// must be included first becuase of conflict between x11 and fbxsdk.h
#include "../BaseGameWin.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

struct MonitorInfo
{
    struct Mode
    {
        Mode(DEVMODE _deviceMode ,GLuint _width, GLuint _height, GLuint _frequency)
        :width(_width), height(_height), frequency(_frequency)
        {
			deviceMode = _deviceMode;
		}

		DEVMODE deviceMode;
        GLuint width;
        GLuint height;
        GLuint frequency;
    };
    
    MonitorInfo(int _index , const std::string& _deviceName,const Rect& _positionRect, const std::vector<Mode>& _modes)
    :deviceName(_deviceName),modes(_modes)
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
	std::string deviceName;
    Rect positionRect;
    std::vector<Mode> modes;
};

class WindowsGameWin : public BaseGameWin
{
public:
	WindowsGameWin();
    virtual ~WindowsGameWin();
	void setHINSTANCE(HINSTANCE hInstance);
    
    bool initWindow     ();
    bool initOpenGL     (int width, int height);
    bool createWindow   (int width, int height);
    bool createOpenGLContext();
            
    int  BeginGame      ();
    bool Shutdown       ();

	void setFullScreenMode(bool fullscreen);
	bool setMonitorResolution(int monitorIndex, Resolution newResolution);
	void setWindowPosition(int x, int y);
	void setWindowSize(GLuint width, GLuint height);
	void moveWindowToMonitor(int monitorIndex);

	void setCursorPos(Point newPos);
	Point getCursorPos();

    Point getWindowPosition();
    GLuint getWindowCurrentMonitor();
    
    static bool isExtensionSupported    (const char *extList, const char *extension);

    static void copyToClipboard(const std::string& text);
    static std::string PasteClipboard();
    
    std::vector<std::vector<Mode1>> getMonitorsModes() const;
    
protected:    
	LRESULT CALLBACK windowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK staticWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	virtual void glSwapBuffers();
	virtual void getMonitorsInfo();
        
    static std::string s_clipboardString;
    
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HDC	m_hDC;
	HGLRC m_hRC;

    std::vector<MonitorInfo> m_monitors;
    GLuint m_primaryMonitorIndex;
};

#endif  //_WINDOWSGAMEWIN_H
