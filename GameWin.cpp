#include "GameWin.h"
#include <windowsx.h>
#undef max
#undef min
//#include <cmath>
#include "virtualKeysWindows.h"
#include "gameInput.h"

std::string GameWin::s_clipboardString;
const double GameWin::s_doubleClickTime = 0.5;

std::ostream& operator<<(std::ostream& os, const Resolution res)
{
    os << res.width << " " << res.height;
    return os;
}

std::istream& operator>>(std::istream& is, Resolution res)
{
    is >> res.width;
    is >> res.height;
    return is;
}

//-----------------------------------------------------------------------------
// Name : GameWin (constructor)
//-----------------------------------------------------------------------------
GameWin::GameWin()
{
	m_hWnd = nullptr;
	m_hDC = nullptr;
	m_hRC = nullptr;

    font_ = nullptr;
    
    gameRunning = true;

    for (int i = 0; i < 256; i++)
        keysStatus[i] = false;

    mouseDrag = false;

    faceCount = -1;
    meshIndex = -1;
    hit = false;
    selectedObj = nullptr;

    lastLeftClickTime = 0;
    lastRightClickTime = 0;
    m_scene = nullptr;
    m_sceneInput = true;
    
	spriteShader = nullptr;
	spriteTextShader = nullptr;

    m_primaryMonitorIndex = 0;
}

//-----------------------------------------------------------------------------
// Name : GameWin (destructor)
//-----------------------------------------------------------------------------
GameWin::~GameWin()
{
}

//-----------------------------------------------------------------------------
// Name : initWindow ()
//-----------------------------------------------------------------------------
bool GameWin::initWindow()
{
    return true;
}

//-----------------------------------------------------------------------------
// Name : getMonitorsInfo ()
//-----------------------------------------------------------------------------
void GameWin::getMonitorsInfo()
{

}

//-----------------------------------------------------------------------------
// Name : getMonitorsModes ()
//-----------------------------------------------------------------------------
std::vector<std::vector<Mode1>> GameWin::getMonitorsModes() const
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
LRESULT CALLBACK GameWin::staticWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int tmep = sizeof(LONG_PTR);
	int temp2 = sizeof(LPVOID);
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if (Message == WM_CREATE) SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	GameWin *Destination = (GameWin*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->windowProc(hWnd, Message, wParam, lParam);

	// No destination found, defer to system...
	return DefWindowProc(hWnd, Message, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Name : windowProc 
//-----------------------------------------------------------------------------
LRESULT CALLBACK GameWin::windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
		reshape(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
	{
		oldCursorLoc.x = GET_X_LPARAM(lParam);
		oldCursorLoc.y = GET_Y_LPARAM(lParam);
		mouseDrag = message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK;

		MouseEventType type;
		if (message == WM_LBUTTONDBLCLK)
			type = MouseEventType::DoubleLeftButton;
		else
			type = MouseEventType::LeftButton;

		sendMouseEvent(MouseEvent(type,
								  Point(GET_X_LPARAM(lParam),
									    GET_Y_LPARAM(lParam)),
								  message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK,
								  timer.getCurrentTime(),
								  0), 
					   ModifierKeysStates(wParam & MK_SHIFT, wParam & MK_CONTROL, false));

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
			timer.getCurrentTime(),
			0),
			ModifierKeysStates(wParam & MK_SHIFT, wParam & MK_CONTROL, false));
		
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
								  timer.getCurrentTime(),
								  wheelDelta),
					   ModifierKeysStates(false, false, false));

		return 0;
	}break;

	case WM_MOUSEMOVE:
	{
		sendMouseEvent(MouseEvent(MouseEventType::MouseMoved, 
								  Point(GET_X_LPARAM(lParam),
									    GET_Y_LPARAM(lParam)), 
								  false,
								  timer.getCurrentTime(),
								  0),
					   ModifierKeysStates(wParam & MK_SHIFT, wParam & MK_CONTROL, false));
		return 0;
	}break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		GK_VirtualKey key = windowsVirtualKeysTable[wParam];
		if (key != GK_VirtualKey::GK_UNKNOWN)
		{
			keysStatus[static_cast<int>(key)] = message == WM_KEYDOWN;
			sendVirtualKeyEvent(key, message == WM_KEYDOWN, ModifierKeysStates(false, false, false));
		}
		else
		{
			int scanCode = (lParam & 0xFF0000) >> 16;
			keysStatus[scanCode] = message == WM_KEYDOWN;
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
bool GameWin::createWindow(int width, int height , HINSTANCE hInstance)
{
	//Creating the window 
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = staticWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
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

	m_hWnd = ::CreateWindowEx(WS_EX_CLIENTEDGE, "Chess", "3D Chess", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, width, height, NULL, NULL, hInstance, (void*)this);

	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, SW_SHOW);
		::UpdateWindow(m_hWnd);
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

	dummyHwnd = ::CreateWindowEx(WS_EX_CLIENTEDGE, "dummyWindow", "dummy Chess Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 100, 100, nullptr, nullptr, hInstance, nullptr);

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
bool GameWin::createOpenGLContext(HINSTANCE hInstance)
{
	HWND dummyhWnd = createDummyWindow(hInstance);
	if (!dummyhWnd)
		return false;

	HDC	dummyhDC = GetDC(dummyhWnd);

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
// Name : setFullScreenMode ()
//-----------------------------------------------------------------------------
void GameWin::setFullScreenMode(bool fullscreen)
{

}

//-----------------------------------------------------------------------------
// Name : setMonitorResolution ()
//-----------------------------------------------------------------------------
bool GameWin::setMonitorResolution(int monitorIndex, Resolution newResolution)
{
	return false;
}

//-----------------------------------------------------------------------------
// Name : setWindowPosition ()
//-----------------------------------------------------------------------------
void GameWin::setWindowPosition(int x, int y)
{
	SetWindowPos(m_hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
}

//-----------------------------------------------------------------------------
// Name : moveWindowToMonitor ()
//-----------------------------------------------------------------------------
void GameWin::moveWindowToMonitor(int monitorIndex)
{
    Point windowPosition = getWindowPosition();
    if (!m_monitors[monitorIndex].positionRect.isPointInRect(windowPosition))
    {
        setWindowPosition(m_monitors[monitorIndex].positionRect.left,
                          m_monitors[monitorIndex].positionRect.top);
    }
}

//-----------------------------------------------------------------------------
// Name : initGUI ()
//-----------------------------------------------------------------------------
void GameWin::initGUI()
{
//     m_dialog.init(300,300, 18, "Caption!", "", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), m_asset);
//     m_dialog.setLocation(50, 100);
//     m_dialog.initDefControlElements(m_asset);
//     //m_dialog.initWoodControlElements(m_asset);
//     ButtonUI* pButton;
//     m_dialog.addButton(1, "button text", 20,20, 200, 25, 0, &pButton);
//     pButton->setEnabled(false);
//     m_dialog.addButton(2, "enabled button", 20, 60, 200, 25, 0);
//     m_dialog.addCheckBox(3, 100,100, 50, 50, 0);
//     m_dialog.addRadioButton(4, 30, 150, 25,25,0,1);
//     m_dialog.addRadioButton(5, 90, 150, 25,25,0,1);
//     ComboBoxUI* pCombo;
//     //m_dialog.addComboBox(6, "Box", 20, 200, 200, 40, 0, &pCombo);
//     m_dialog.addComboBox(6, "Box", 20, 200, 300, 60, 0, &pCombo);
//     pCombo->AddItem("Sunday", 1);
//     pCombo->AddItem("Monday", 2);
//     pCombo->AddItem("Tuesday", 3);
//     pCombo->AddItem("Wednesday", 4);
//     pCombo->AddItem("Thursday", 5);
//     pCombo->AddItem("Friday", 6);
//     pCombo->AddItem("Saturday", 7);
// 
//     ListBoxUI<int>* pListbox;
//     m_dialog.addListBox(7, 350,200, 200,100, true, &pListbox);
//     pListbox->AddItem("Sunday", 1);
//     pListbox->AddItem("Monday", 2);
//     pListbox->AddItem("Tuesday", 3);
//     pListbox->AddItem("Wednesday", 4);
//     pListbox->AddItem("Thursday", 5);
//     pListbox->AddItem("Friday", 6);
//     pListbox->AddItem("Saturday", 7);
// 
//     m_dialog.addSlider(8, 0, 0, 200, 40, 0, 100, 50);
// 
//     m_dialog.addEditbox(9, "Test TgTy",180, 100, 100, 35, nullptr );
// 
//     //m_dialog.addButton(10, "Center me", 20, 300, 400, 141, 0);
// 
//     m_dialog.addEditbox(11, "Test TgTy",420, 300, 400, 100, nullptr );
}

//-----------------------------------------------------------------------------
// Name : setRenderStates ()
//-----------------------------------------------------------------------------
void GameWin::setRenderStates()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
    
//-----------------------------------------------------------------------------
// Name : initOpenGL ()
//-----------------------------------------------------------------------------
bool GameWin::initOpenGL(int width, int height, HINSTANCE hInstance)
{
    int err;
    std::cout << "InitOpenGL started\n";

    if (!createWindow(width, height, hInstance))
        return false; 
        
    if(!createOpenGLContext(hInstance))
        return false;
    
    glewInit();

    //------------------------------------
    // Render states
    //------------------------------------
    setRenderStates();

    //------------------------------------
    // buffers creation
    //------------------------------------
    glBindVertexArray(0);

    //------------------------------------
    // Shader loading
    //------------------------------------
    // complie shaders
    spriteShader = m_asset.getShader("sprite");
    spriteTextShader = m_asset.getShader("spriteText");

    m_sprites[0].Init();
    m_sprites[1].Init();
    m_topSprites[0].Init();
    m_topSprites[1].Init();

    initGUI();
    
    //------------------------------------
    // Init Scene
    //------------------------------------
    if (m_scene)
        m_scene->InitScene(width, height);
    
    // init our font
    font_ = m_asset.getFont("NotoMono", 40);
    // make sure the viewport is updated
	RECT clientRC;
	GetClientRect(m_hWnd, &clientRC);
    reshape(clientRC.right - clientRC.left , clientRC.bottom - clientRC.top);
    
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout <<"Init: ERROR bitches\n";
        std::cout << gluErrorString(err) << "\n";
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// Name : isExtensionSupported ()
//-----------------------------------------------------------------------------
bool GameWin::isExtensionSupported(const char *extList, const char *extension)
{
    const char *start;
    const char *where, *terminator;
  
    // Extension names should not have spaces.
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return false;

    // It takes a bit of care to be fool-proof about parsing the
    // OpenGL extensions string. Don't be fooled by sub-strings,
    // etc.
    for (start = extList;;) 
    {
        where = strstr(start, extension);

        if (!where)
            break;

        terminator = where + strlen(extension);

        if ( where == start || *(where - 1) == ' ' )
        if ( *terminator == ' ' || *terminator == '\0' )
            return true;

        start = terminator;
    }

    return false;
}


//-----------------------------------------------------------------------------
// Name : copyToClipboard ()
//-----------------------------------------------------------------------------
void GameWin::copyToClipboard(const std::string &text)
{

}

//-----------------------------------------------------------------------------
// Name : PasteClipboard ()
//-----------------------------------------------------------------------------
std::string GameWin::PasteClipboard()
{
	return std::string();
}

//-----------------------------------------------------------------------------
// Name : drawing ()
//-----------------------------------------------------------------------------
void GameWin::drawing()
{
    int err;

    ProcessInput(timer.getTimeElapsed());

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_scene)
        m_scene->Drawing(timer.getTimeElapsed());

    std::stringstream ss;
    if (m_scene != nullptr)
    {
        ss << m_scene->getMeshIndex();
        ss << " ";
        ss << m_scene->getFaceCount();
        ss << " | ";
        int faceCount = m_scene->getFaceCount();
        int temp = (faceCount / 8);
        int square = (faceCount / 2) - (((faceCount / 2) / 4) * 4 );
        if ( (m_scene->getMeshIndex() == 0 && temp % 2 == 0) ||
         (m_scene->getMeshIndex() == 1 && temp % 2 != 0))
        {
            ss << square * 2;
            square = square * 2;
        }
        else
        {
            ss << square * 2 + 1;
            square = square * 2 + 1;
        }
        ss << " ";
        //int temp = (8 - 1 - (faceCount / 8));
        ss << temp;
        ss << " | ";
        ss << temp << " " << 7 - square;
    }
    

    glDisable(GL_DEPTH_TEST);
    renderFPS(m_sprites[1], *font_);
    font_->renderToRect(m_sprites[1], ss.str(), Rect(0,65, 500, 200), 
                        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    
    if(m_scene)
        font_->renderToRect(m_sprites[1], m_scene->getStatus(), Rect(0, 130, 2000, 200), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    
    renderGUI();

    m_sprites[0].Render(spriteShader);
    m_sprites[0].Clear();
    m_sprites[1].Render(spriteTextShader);
    m_sprites[1].Clear();
    m_topSprites[0].Render(spriteShader);
    m_topSprites[0].Clear();
    m_topSprites[1].Render(spriteTextShader);
    m_topSprites[1].Clear();
        
    glEnable(GL_DEPTH_TEST);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout <<"Drawing: ERROR bitches " << err << " :";
        std::cout << gluErrorString(err) << "\n";
    }

	SwapBuffers(m_hDC);
}

//-----------------------------------------------------------------------------
// Name : renderGUI ()
//-----------------------------------------------------------------------------
void GameWin::renderGUI()
{
//     m_dialog.OnRender(m_sprite, m_textSprite, m_asset, timer.getCurrentTime());;
//     Point textSize = font_->calcTextRect("[]a");
//     font_->renderToRect(m_textSprite, "[]a", Rect(0,50, 0 + textSize.x, 50 + textSize.y), WHITE_COLOR);
//     m_sprite.AddTintedQuad(Rect(0,50,0 + textSize.x, 50 + textSize.y), glm::vec4(1.0f, 0.0, 0.0, 1.0f));
}

//-----------------------------------------------------------------------------
// Name : renderFPS ()
//-----------------------------------------------------------------------------
void GameWin::renderFPS(Sprite& textSprite, mkFont& font)
{
    font.renderToRect(textSprite, std::to_string(timer.getFPS()), Rect(0,0, 200,60), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

//-----------------------------------------------------------------------------
// Name : reshape ()
//-----------------------------------------------------------------------------
void GameWin::reshape(int width, int height)
{
    //if(m_winWidth != width || m_winHeight != height)
    {
        std::cout <<"reshape called\n";
        std::cout << "New window size is " << width << "X" << height << "\n";
        m_winWidth = width;
        m_winHeight = height;

        if(m_scene)
            m_scene->reshape(m_winWidth,m_winHeight);
        else
            glViewport(0.0f,0.0f, width, height);

        if (spriteShader != nullptr)
        {
            spriteShader->Use();
            glUniform2i( glGetUniformLocation(spriteShader->Program, "screenSize"), width / 2, height / 2);
        }
        
        if (spriteTextShader != nullptr)
        {
            spriteTextShader->Use();
            glUniform2i( glGetUniformLocation(spriteTextShader->Program, "screenSize"), width / 2, height / 2);
        }
        
        onSizeChanged();
    }
    //else
    //    std::cout <<"reshape ignored\n";
}

//-----------------------------------------------------------------------------
// Name : ProcessInput ()
//-----------------------------------------------------------------------------
void GameWin::ProcessInput(double timeDelta)
{
    float X = 0.0f,Y = 0.0f;
    if (mouseDrag)
    {
        Point currentCursorPos = getCursorPos();

        X = (float)(currentCursorPos.x - oldCursorLoc.x) / 3.0f;
        Y = (float)(currentCursorPos.y - oldCursorLoc.y) / 3.0f;

        setCursorPos(Point(oldCursorLoc));
    }
    
    if(m_scene && m_sceneInput)
        m_scene->processInput(timeDelta, keysStatus, X, Y);
}

//-----------------------------------------------------------------------------
// Name : setCursorPos ()
//-----------------------------------------------------------------------------
void GameWin::setCursorPos(Point newPos)
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
Point GameWin::getCursorPos()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(m_hWnd, &cursorPos);

    return Point(cursorPos.x, cursorPos.y);
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
//-----------------------------------------------------------------------------
int GameWin::BeginGame()
{
	MSG		msg;

	// Start main loop
	while (gameRunning)
	{
		// Did we receive a message, or are we idling ?
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				gameRunning = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Advance Game Frame.
		timer.frameAdvanced();
		int err = glGetError();
		if (err != GL_NO_ERROR)
			std::cout << "MsgLoop: ERROR bitches\n";

		if (!timer.isCap())
		{
			drawing();
		}
	}

	return 0;

}

//-----------------------------------------------------------------------------
// Name : sendKeyEvent ()
//-----------------------------------------------------------------------------
bool GameWin::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : sendKeyEvent ()
//-----------------------------------------------------------------------------
void GameWin::sendKeyEvent(unsigned char key, bool down)
{
    //m_dialog.handleKeyEvent(key, down);
}

//-----------------------------------------------------------------------------
// Name : sendVirtualKeyEvent ()
//-----------------------------------------------------------------------------
void GameWin::sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates)
{
    //m_dialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
}

//-----------------------------------------------------------------------------
// Name : sendMouseEvent ()
//-----------------------------------------------------------------------------
void GameWin::sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    handleMouseEvent(event, modifierStates);
    if (m_scene != nullptr && m_sceneInput)
        m_scene->handleMouseEvent(event, modifierStates);
    //m_dialog.handleMouseEvent(event, modifierStates);
}

//-----------------------------------------------------------------------------
// Name : onSizeChanged ()
//-----------------------------------------------------------------------------
void GameWin::onSizeChanged()
{
    
}

//-----------------------------------------------------------------------------
// Name : Shutdown ()
//-----------------------------------------------------------------------------
bool GameWin::Shutdown()
{
	wglMakeCurrent(m_hDC, NULL);
	wglDeleteContext(m_hRC);
        
    return true;
}

//-----------------------------------------------------------------------------
// Name : getWindowPosition ()
//-----------------------------------------------------------------------------
Point GameWin::getWindowPosition()
{
	RECT winRect;
	GetWindowRect(m_hWnd, &winRect);

	return Point(winRect.left, winRect.top);
}

//-----------------------------------------------------------------------------
// Name : getWindowCurrentMonitor ()
//-----------------------------------------------------------------------------
GLuint GameWin::getWindowCurrentMonitor()
{    
    for (GLuint i = 0; i < m_monitors.size(); i++)
    {
        if (m_monitors[i].positionRect.isPointInRect(getWindowPosition()))
            return i;
    }
    
    return 0;
}
