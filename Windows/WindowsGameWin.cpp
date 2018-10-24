#include "WindowsGameWin.h"
#include <windowsx.h>
#undef max
#undef min
#include "virtualKeysWindows.h"

//-----------------------------------------------------------------------------
// Name : WindowsGameWin (constructor)
//-----------------------------------------------------------------------------
WindowsGameWin::WindowsGameWin()
{
    m_hInstance = nullptr;
    m_hWnd = nullptr;
    m_hDC = nullptr;
    m_hRC = nullptr;

    m_primaryMonitorIndex = 0;
}

//-----------------------------------------------------------------------------
// Name : WindowsGameWin (destructor)
//-----------------------------------------------------------------------------
WindowsGameWin::~WindowsGameWin()
{
}

//-----------------------------------------------------------------------------
// Name : setHINSTANCE (destructor)
//-----------------------------------------------------------------------------
void WindowsGameWin::setHINSTANCE(HINSTANCE hInstance)
{
    m_hInstance = hInstance;
}

//-----------------------------------------------------------------------------
// Name : platformInit ()
//-----------------------------------------------------------------------------
bool WindowsGameWin::platformInit(int width, int height)
{
    std::cout << "InitOpenGL started\n";

    if (!createWindow(width, height))
        return false; 
        
    if(!createOpenGLContext())
        return false;
    
    return true;
}

//-----------------------------------------------------------------------------
// Name : getMonitorsInfo ()
//-----------------------------------------------------------------------------
void WindowsGameWin::getMonitorsInfo()
{
    int i = 0;
    Resolution moniotrResolutions[] = { Resolution(1920, 1080),
                                        Resolution(1600, 1200),
                                        Resolution(1680, 1050),
                                        Resolution(1400, 1050),
                                        Resolution(1600, 900),
                                        Resolution(1280, 1024),
                                        Resolution(1440, 900),
                                        Resolution(1280, 960),
                                        Resolution(1280, 800),
                                        Resolution(1152, 864),
                                        Resolution(1280, 720),
                                        Resolution(1024, 768) };
                                        
    DISPLAY_DEVICE deviceInfo;
    deviceInfo.cb = sizeof(DISPLAY_DEVICE);
    BOOL validDeviceIndex = EnumDisplayDevices(NULL, i, &deviceInfo, EDD_GET_DEVICE_INTERFACE_NAME);
    while (validDeviceIndex)
    {
        std::vector<MonitorInfo::Mode> modes;
        std::string deviceName = deviceInfo.DeviceName;
        deviceName += "||||";
        deviceName += deviceInfo.DeviceString;
        if (deviceInfo.StateFlags & DISPLAY_DEVICE_ACTIVE)
        {
            //MessageBox(m_hWnd, deviceName.c_str(), "Device Info", MB_OK);
            DEVMODE deviceMode;
            deviceMode.dmSize = sizeof(DEVMODE);
            if (EnumDisplaySettings(deviceInfo.DeviceName, ENUM_CURRENT_SETTINGS, &deviceMode))
            {
                Resolution originalResolution(deviceMode.dmPelsWidth, deviceMode.dmPelsHeight);
                modes.emplace_back(deviceMode, deviceMode.dmPelsWidth, deviceMode.dmPelsHeight, 0);
                //streamSettings << deviceMode.dmPelsWidth << "X" << deviceMode.dmPelsHeight << " " << deviceMode.dmDisplayFrequency << "Hz";
                //MessageBox(m_hWnd, streamSettings.str().c_str(), "Device Settings", MB_OK);
                DEVMODE testDeviceMode;
                testDeviceMode = deviceMode;
                for (const Resolution& monitorResolution : moniotrResolutions)
                {
                    // The original resolution is always added first so no point in reAdding it
                    if (monitorResolution.width == originalResolution.width && monitorResolution.height == originalResolution.height)
                        continue;

                    testDeviceMode.dmPelsWidth = monitorResolution.width;
                    testDeviceMode.dmPelsHeight = monitorResolution.height;
                    LONG ret = ChangeDisplaySettingsEx(deviceInfo.DeviceName, &testDeviceMode, nullptr, CDS_TEST, nullptr);
                    if (ret == DISP_CHANGE_SUCCESSFUL)
                    {
                        std::cout << monitorResolution.width << "X" << monitorResolution.height << "\n";
                        modes.emplace_back(testDeviceMode, monitorResolution.width, monitorResolution.height, 0);
                    }
                }

                Rect monitorRect = Rect(deviceMode.dmPosition.x, deviceMode.dmPosition.y,
                deviceMode.dmPosition.x + deviceMode.dmPelsWidth, deviceMode.dmPosition.y + deviceMode.dmPelsHeight);
                m_monitors.emplace_back(i, deviceInfo.DeviceName, monitorRect, std::move(modes));
            }
        }
        
        i++;
        validDeviceIndex = EnumDisplayDevices(NULL, i, &deviceInfo, EDD_GET_DEVICE_INTERFACE_NAME);
    }
}

//-----------------------------------------------------------------------------
// Name : getMonitorsModes ()
//-----------------------------------------------------------------------------
std::vector<std::vector<Mode1>> WindowsGameWin::getMonitorsModes() const
{
    std::vector<std::vector<Mode1>> monitorsModes;
    for (MonitorInfo monitor : m_monitors)
    {
        std::vector<Mode1> monitorModes;
        for (MonitorInfo::Mode mode : monitor.modes)
            monitorModes.emplace_back(mode.width, mode.height);
                
        monitorsModes.push_back(std::move(monitorModes));
    }
        
    return monitorsModes;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc 
// Desc : forward the message to the proper instance of the class 
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowsGameWin::staticWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    // If this is a create message, trap the 'this' pointer passed in and store it within the window.
    if (Message == WM_CREATE) SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

    // Obtain the correct destination for this message
    WindowsGameWin *Destination = (WindowsGameWin*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    // If the hWnd has a related class, pass it through
    if (Destination) return Destination->windowProc(hWnd, Message, wParam, lParam);

    // No destination found, defer to system...
    return DefWindowProc(hWnd, Message, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Name : windowProc 
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowsGameWin::windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ModifierKeysStates modifierKeys(GetAsyncKeyState(VK_SHIFT) < 0, GetAsyncKeyState(VK_CONTROL) < 0, false);

    switch (message)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            break;
    
        case WM_SIZE:
        {
            if (LOWORD(lParam) != 0 && HIWORD(lParam) != 0)
                reshape(LOWORD(lParam), HIWORD(lParam));
            
            return 0;
        }break;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONUP:
        {
            m_oldCursorLoc.x = GET_X_LPARAM(lParam);
            m_oldCursorLoc.y = GET_Y_LPARAM(lParam);
            m_mouseDrag = message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK;

            MouseEventType type;
            if (message == WM_LBUTTONDBLCLK)
                type = MouseEventType::DoubleLeftButton;
            else
                type = MouseEventType::LeftButton;

            sendMouseEvent(MouseEvent(type,
                                      Point(GET_X_LPARAM(lParam),
                                            GET_Y_LPARAM(lParam)),
                                      message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK,
                                      m_timer.getCurrentTime(),
                                      0), 
                           modifierKeys);

            return 0;
        }break;

        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_RBUTTONUP:
        {
            MouseEventType type;
            if (message == WM_RBUTTONDBLCLK)
                type = MouseEventType::DoubleRightButton;
            else
                type = MouseEventType::RightButton;

            sendMouseEvent(MouseEvent(type,
                                      Point(GET_X_LPARAM(lParam),
                                            GET_Y_LPARAM(lParam)),
                                      message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK,
                                      m_timer.getCurrentTime(),
                                      0),
                           modifierKeys);
            
            return 0;
        }break;

        case WM_MOUSEWHEEL:
        {
            int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            bool down;

            if (wheelDelta == 0)
                return 0;

            if (wheelDelta > 0)
            {
                wheelDelta = 1;
                down = false;
            }
            else
            {
                wheelDelta = -1;
                down = true;
            }
            
            sendMouseEvent(MouseEvent(MouseEventType::ScrollVert,
                                      Point(GET_X_LPARAM(lParam),
                                            GET_Y_LPARAM(lParam)),
                                      down,
                                      m_timer.getCurrentTime(),
                                      wheelDelta),
                           modifierKeys);

            return 0;
        }break;

        case WM_MOUSEMOVE:
        {
            sendMouseEvent(MouseEvent(MouseEventType::MouseMoved, 
                                      Point(GET_X_LPARAM(lParam),
                                            GET_Y_LPARAM(lParam)), 
                                      false,
                                      m_timer.getCurrentTime(),
                                      0),
                           modifierKeys);
            return 0;
        }break;

        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            GK_VirtualKey key = windowsVirtualKeysTable[wParam];
            if (key != GK_VirtualKey::GK_UNKNOWN)
            {
                m_keysStatus[static_cast<int>(key)] = message == WM_KEYDOWN;
                sendVirtualKeyEvent(key, message == WM_KEYDOWN, modifierKeys);
            }
            else
            {
                int scanCode = (lParam & 0xFF0000) >> 16;
                m_keysStatus[scanCode] = message == WM_KEYDOWN;
            }
            
            return 0;
        }break;

        case WM_CHAR:
        {
            sendKeyEvent((unsigned char)wParam, true);
            return 0;
        }break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Name : createWindow ()
//-----------------------------------------------------------------------------
bool WindowsGameWin::createWindow(int width, int height)
{
    //Creating the window 
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = staticWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = "Chess";

    if (!RegisterClass(&wc))
    {
        ::MessageBox(0, "RegisterClass() - FAILED", 0, 0);
        return nullptr;
    }

    m_hWnd = ::CreateWindowEx(WS_EX_CLIENTEDGE, "Chess", "3D Chess", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, width, height, NULL, NULL, m_hInstance, (void*)this);
    if (m_hWnd)
    {
        ::ShowWindow(m_hWnd, SW_SHOW);
        ::UpdateWindow(m_hWnd);
        
        getMonitorsInfo();
        return true;
    }
    else
    {
        ::MessageBox(NULL, "failed to create a window", "Error", MB_OK);
        return false;
    }
}

//-----------------------------------------------------------------------------
// Name : dummyWindowProc ()
//-----------------------------------------------------------------------------
LRESULT CALLBACK dummyWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Name : createDummyWindow ()
//-----------------------------------------------------------------------------
HWND createDummyWindow(HINSTANCE hInstance)
{
    HWND dummyHwnd = nullptr;
    //Creating the window 
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = dummyWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = "dummyWindow";

    if (!RegisterClass(&wc))
    {
        ::MessageBox(0, "RegisterClass() - FAILED", 0, 0);
        return nullptr;
    }

    dummyHwnd = ::CreateWindowEx(WS_EX_CLIENTEDGE, "dummyWindow", "dummy Chess Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, hInstance, nullptr);

    if (dummyHwnd)
    {
        return dummyHwnd;
    }
    else
    {
        ::MessageBox(NULL, "failed to create a window", "Error", MB_OK);
        return nullptr;
    }
}

//-----------------------------------------------------------------------------
// Name : createOpenGLContext ()
//-----------------------------------------------------------------------------
bool WindowsGameWin::createOpenGLContext()
{
    // create dummy window to find the wanted pixel format
    HWND dummyhWnd = createDummyWindow(m_hInstance);
    if (!dummyhWnd)
        return false;

    HDC dummyhDC = GetDC(dummyhWnd);

    PIXELFORMATDESCRIPTOR pfd = {
                sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd  
                1,                                // version number  
                PFD_DRAW_TO_WINDOW |              // support window  
                PFD_SUPPORT_OPENGL |              // support OpenGL  
                PFD_DOUBLEBUFFER,                 // double buffered  
                PFD_TYPE_RGBA,                    // RGBA type  
                24,                               // 24-bit color depth  
                0, 0, 0, 0, 0, 0,                 // color bits ignored  
                0,                                // no alpha buffer  
                0,                                // shift bit ignored  
                0,                                // no accumulation buffer  
                0, 0, 0, 0,                       // accum bits ignored  
                32,                               // 32-bit z-buffer      
                0,                                // no stencil buffer  
                0,                                // no auxiliary buffer  
                PFD_MAIN_PLANE,                   // main layer  
                0,                                // reserved  
                0, 0, 0                           // layer masks ignored  
    };
    
    int  iPixelFormat;

    // get the device context's best, available pixel format match  
    iPixelFormat = ChoosePixelFormat(dummyhDC, &pfd);
    // make that match the device context's current pixel format  
    SetPixelFormat(dummyhDC, iPixelFormat, &pfd);

    HGLRC dummyhRC = wglCreateContext(dummyhDC);

    wglMakeCurrent(dummyhDC, dummyhRC);

    wglChoosePixelFormatARBProc wglChoosePixelFormatARB = nullptr;
    wglChoosePixelFormatARB = (wglChoosePixelFormatARBProc)wglGetProcAddress("wglChoosePixelFormatARB");

    int attributes[] = {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_COLOR_BITS_ARB, 32,
                WGL_DEPTH_BITS_ARB, 24,
                WGL_STENCIL_BITS_ARB, 8,
                WGL_SAMPLE_BUFFERS_ARB, 1, // Number of buffers (must be 1 at time of writing)
                WGL_SAMPLES_ARB, 4,        // Number of samples
                0,0
    };

    int pixelFormat;
    UINT numFormats;

    wglChoosePixelFormatARB(dummyhDC, attributes, nullptr, 1, &pixelFormat, &numFormats);

    m_hDC = GetDC(m_hWnd);
    SetPixelFormat(m_hDC, pixelFormat, &pfd);

    wglCreateContextAttribsARBProc wglCreateContextAttribsARB = nullptr;
    wglCreateContextAttribsARB = (wglCreateContextAttribsARBProc)wglGetProcAddress("wglCreateContextAttribsARB");

    int context_attribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 3, WGL_CONTEXT_MINOR_VERSION_ARB, 3, 0 };
    m_hRC = wglCreateContextAttribsARB(m_hDC, 0, context_attribs);

    wglMakeCurrent(m_hDC, m_hRC);
    wglDeleteContext(dummyhRC);

    DestroyWindow(dummyhWnd);
    
    std::cout << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << "\n";
        
    return true;
}

//-----------------------------------------------------------------------------
// Name : glSwapBuffers ()
//-----------------------------------------------------------------------------
void WindowsGameWin::glSwapBuffers()
{
    SwapBuffers(m_hDC);
}

//-----------------------------------------------------------------------------
// Name : setFullScreenMode ()
//-----------------------------------------------------------------------------
void WindowsGameWin::setFullScreenMode(bool fullscreen)
{
    if (fullscreen)
    {
        Point windowPos = getWindowPosition();

        for (const MonitorInfo& monitorInfo : m_monitors)
        {
            if (monitorInfo.positionRect.isPointInRect(windowPos))
            {
                SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
                SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, 0);
                setWindowPosition(monitorInfo.positionRect.left, monitorInfo.positionRect.top);
                setWindowSize(monitorInfo.positionRect.getWidth(), monitorInfo.positionRect.getHeight());
                break;
            }
        }
    }
    else
    {
        SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        setWindowSize(1024, 768);
    }
}

//-----------------------------------------------------------------------------
// Name : setMonitorResolution ()
//-----------------------------------------------------------------------------
bool WindowsGameWin::setMonitorResolution(int monitorIndex, Resolution newResolution)
{
    if (monitorIndex > m_monitors.size())
        return false;

    bool IDfound = false;

    DEVMODE* modeInfo;
    std::string deviceName;
    std::vector<Mode1> monitorModes;
    for (MonitorInfo::Mode mode : m_monitors[monitorIndex].modes)
    {
        if (mode.width == newResolution.width && mode.height == newResolution.height)
        {
            modeInfo = &mode.deviceMode;
            IDfound = true;
            break;
        }
    }

    if (IDfound)
    {
        LONG ret = ChangeDisplaySettingsEx(m_monitors[monitorIndex].deviceName.c_str(), modeInfo, nullptr, CDS_FULLSCREEN, nullptr);
        if (ret == DISP_CHANGE_SUCCESSFUL)
        {
            m_monitors[monitorIndex].positionRect.right = m_monitors[monitorIndex].positionRect.left + newResolution.width;
            m_monitors[monitorIndex].positionRect.bottom = m_monitors[monitorIndex].positionRect.top + newResolution.height;
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : setWindowPosition ()
//-----------------------------------------------------------------------------
void WindowsGameWin::setWindowPosition(int x, int y)
{
    SetWindowPos(m_hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
}

//-----------------------------------------------------------------------------
// Name : setWindowSize ()
//-----------------------------------------------------------------------------
void WindowsGameWin::setWindowSize(GLuint width, GLuint height)
{
    RECT wr = { 0, 0, width, height};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    int newWidth = wr.right - wr.left;
    int newHeight = wr.bottom - wr.top;
    SetWindowPos(m_hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
    RECT clientRC;
    GetClientRect(m_hWnd, &clientRC);
    std::cout << "setWindowSize window size is: " << clientRC.right - clientRC.left << "X" << clientRC.bottom - clientRC.top << "\n";
}

//-----------------------------------------------------------------------------
// Name : moveWindowToMonitor ()
//-----------------------------------------------------------------------------
void WindowsGameWin::moveWindowToMonitor(int monitorIndex)
{
    Point windowPosition = getWindowPosition();
    if (!m_monitors[monitorIndex].positionRect.isPointInRect(windowPosition))
    {
        setWindowPosition(m_monitors[monitorIndex].positionRect.left,
                          m_monitors[monitorIndex].positionRect.top);
    }
}
    
//-----------------------------------------------------------------------------
// Name : copyToClipboard ()
//-----------------------------------------------------------------------------
void WindowsGameWin::copyToClipboard(const std::string &text)
{
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size());
    memcpy(GlobalLock(hMem), text.data(), text.size());
    GlobalUnlock(hMem);
    OpenClipboard(nullptr);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}

//-----------------------------------------------------------------------------
// Name : PasteClipboard ()
//-----------------------------------------------------------------------------
std::string WindowsGameWin::PasteClipboard()
{
    // Try opening the clipboard
    if (!OpenClipboard(nullptr))
    {
        std::cout << "Failed to open clipboard for pasting\n";
        return std::string();
    }

    // Get handle of clipboard object for ANSI text
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr)
    {
        std::cout << "Failed to get handle to clipboard text object\n";
        CloseClipboard();
        return std::string();
    }

    // Lock the handle to get the actual text pointer
    char * pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == nullptr)
    {
        std::cout << "Failed to get a lock on actual text data\n";
        CloseClipboard();
        return std::string();
    }

    // Save text in a string class instance
    std::string text(pszText);
    // Release the lock
    GlobalUnlock(hData);
    // Release the clipboard
    CloseClipboard();

    return text;
}

//-----------------------------------------------------------------------------
// Name : setCursorPos ()
//-----------------------------------------------------------------------------
void WindowsGameWin::setCursorPos(Point newPos)
{
    POINT newCursorPos;
    newCursorPos.x = newPos.x;
    newCursorPos.y = newPos.y;
    ClientToScreen(m_hWnd,&newCursorPos);

    SetCursorPos(newCursorPos.x, newCursorPos.y);
}

//-----------------------------------------------------------------------------
// Name : getCursorPos ()
//-----------------------------------------------------------------------------
Point WindowsGameWin::getCursorPos()
{
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(m_hWnd, &cursorPos);

    return Point(cursorPos.x, cursorPos.y);
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
//-----------------------------------------------------------------------------
int WindowsGameWin::BeginGame()
{
    MSG msg;

    // Start main loop
    while (m_gameRunning)
    {
        // Did we receive a message, or are we idling ?
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                m_gameRunning = false;
                break;
            }
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Advance Game Frame.
        m_timer.frameAdvanced();
        int err = glGetError();
        if (err != GL_NO_ERROR)
            std::cout << "MsgLoop: ERROR bitches\n";

        if (!m_timer.isCap())
        {
            drawing();
        }
    }

    return 0;

}

//-----------------------------------------------------------------------------
// Name : Shutdown ()
//-----------------------------------------------------------------------------
bool WindowsGameWin::Shutdown()
{
    wglMakeCurrent(m_hDC, NULL);
    wglDeleteContext(m_hRC);
        
    return true;
}

//-----------------------------------------------------------------------------
// Name : getWindowPosition ()
//-----------------------------------------------------------------------------
Point WindowsGameWin::getWindowPosition()
{
    RECT winRect;
    GetWindowRect(m_hWnd, &winRect);

    return Point(winRect.left, winRect.top);
}

//-----------------------------------------------------------------------------
// Name : getWindowCurrentMonitor ()
//-----------------------------------------------------------------------------
GLuint WindowsGameWin::getWindowCurrentMonitor()
{    
    for (GLuint i = 0; i < m_monitors.size(); i++)
    {
        if (m_monitors[i].positionRect.isPointInRect(getWindowPosition()))
            return i;
    }
    
    return 0;
}
