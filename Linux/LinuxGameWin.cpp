#include "LinuxGameWin.h"
#include <cmath>
#include <GL/glut.h>
#include <unistd.h>
#include <linux/input-event-codes.h>
#include <X11/Xatom.h>
#include "../input.h"
#include "virtualKeysLinux.h"


bool LinuxGameWin::ctxErrorOccurred = false;
Display * LinuxGameWin::s_clipboardDisplay = nullptr;
Atom LinuxGameWin::s_utf8 = None ;
Atom LinuxGameWin::s_targets = None;
Atom LinuxGameWin::s_selection = None;
std::future<void> GameWin::s_clipboardSender;
Window LinuxGameWin::s_clipboardWindow = 0;
std::string LinuxGameWin::s_clipboardString;
const double LinuxGameWin::s_doubleClickTime = 0.5;

double calculateXRefreshRate(const XRRModeInfo *info)
{
    return (info->hTotal && info->vTotal) ?
        round(((double)info->dotClock / (double)(info->hTotal * info->vTotal))) : 0;
}

//-----------------------------------------------------------------------------
// Name : LinuxGameWin (constructor)
//-----------------------------------------------------------------------------
LinuxGameWin::LinuxGameWin()
{
    font_ = nullptr;
    
    gameRunning = true;
    ctx = nullptr;

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
    
    m_primaryMonitorIndex = 0;
}

//-----------------------------------------------------------------------------
// Name : LinuxGameWin (destructor)
//-----------------------------------------------------------------------------
LinuxGameWin::~LinuxGameWin()
{
}

//-----------------------------------------------------------------------------
// Name : initWindow ()
//-----------------------------------------------------------------------------
bool LinuxGameWin::initWindow()
{
    m_display = XOpenDisplay(nullptr);
    
    if (!m_display)
    {
        std::cout << "Failed to open X display\n";
        return false;
    }

    Screen* pScreen = DefaultScreenOfDisplay(m_display);
    std::cout << pScreen->width << "X" << pScreen->height << "\n";
    
    if ( s_clipboardDisplay == nullptr)
    {
        s_clipboardDisplay = XOpenDisplay(nullptr);
        if (!s_clipboardDisplay )
        {
                std::cout << "Failed to open X display for clipboard\n";
                return false;
        }

        // get needed atoms for the clipboard operations
        s_selection = XInternAtom( s_clipboardDisplay, "CLIPBOARD", False);
        s_utf8 = XInternAtom( s_clipboardDisplay, "UTF8_STRING", False);
        s_targets = XInternAtom( s_clipboardDisplay, "TARGETS", False);
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// Name : getBestFBConfig ()
//-----------------------------------------------------------------------------
GLXFBConfig LinuxGameWin::getBestFBConfig()
{
    // Get a matching FB config
    int visual_attribs[] =
    {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      None
    };
    
    int glx_major, glx_minor;
 
    if (m_display == nullptr)
        std::cout << "No X display! but it did open it....\n";
    
    // FBConfigs were added in GLX version 1.3.
    if ( !glXQueryVersion(m_display, &glx_major, &glx_minor) ||
        ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
    {
        std::cout << "Invalid GLX Version\n";
        return nullptr;
    }
       
    std::cout << "Getting matching framebuffer configs\n"; 
    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(m_display, DefaultScreen(m_display), visual_attribs, &fbcount);
    if (!fbc)
    {
        std::cout << "Failed to retrieve a framebuffer config\n";
        return nullptr;
    }
    std::cout << "Found " << fbcount << " matching FB configs.\n";
    
    // Pick the FB config/visual with the most samples per pixel
    //std::cout << "Getting XVisualInfos\n";
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    int i;
    for (i=0; i<fbcount; ++i)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig( m_display, fbc[i] );
        if ( vi )
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib( m_display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
            glXGetFBConfigAttrib( m_display, fbc[i], GLX_SAMPLES       , &samples  );
      
//             std::cout << "  Matching fbconfig " << i << " , visual ID 0x" << std::hex << vi->visualid
//              << ": SAMPLE_BUFFERS = " << samp_buf << " SAMPLES = " << samples << "\n";
      
            if ( (best_fbc < 0) || (samp_buf && (samples > best_num_samp)) )
                best_fbc = i, best_num_samp = samples;
            
            if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
                worst_fbc = i, worst_num_samp = samples;
        }
        XFree( vi );
    }
    
    GLXFBConfig bestFbc = fbc[ best_fbc ];
    // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
    XFree( fbc );
    return bestFbc;
}

//-----------------------------------------------------------------------------
// Name : getMonitorsInfo ()
//-----------------------------------------------------------------------------
void LinuxGameWin::getMonitorsInfo()
{
    int nMonitors;
    //XRRMonitorInfo* monitorInfo = XRRGetMonitors(m_display, m_win, true, &nMonitors);
    XRRMonitorInfo* monitorInfo = XRRGetMonitors(m_display, DefaultRootWindow(m_display), true, &nMonitors);
    XRRScreenResources* screenRes = XRRGetScreenResources(m_display, DefaultRootWindow(m_display));
    
    for (int monitorIndex = 0; monitorIndex < nMonitors; monitorIndex++)
    {        
        std::cout << "Monitor " << monitorIndex + 1<< " " << monitorInfo[monitorIndex].width << "X" << monitorInfo[monitorIndex].height << " " << monitorInfo[monitorIndex].noutput <<"\n";
        if (monitorInfo[monitorIndex].primary)
        {
            m_primaryMonitorIndex = monitorIndex;
            std::cout << "Monitor is primary\n";
        }
        
        std::vector<MonitorInfo::Mode> modes;
        std::vector<RROutput> outputs;
        
        for (int outputIndex = 0; outputIndex < monitorInfo[monitorIndex].noutput ; outputIndex++)
            outputs.push_back(monitorInfo[monitorIndex].outputs[outputIndex]);
        
        if (monitorInfo[monitorIndex].noutput > 0)
        {
            if (monitorInfo[monitorIndex].noutput > 1)
                std::cout << "Warning monitor " << monitorIndex << "has more than one output\n";
            
            std::vector<RRMode> monitorModes;
            XRROutputInfo* outputInfo = XRRGetOutputInfo(m_display, screenRes, monitorInfo[monitorIndex].outputs[0]);
                        
            for (int k = 0; k < outputInfo->nmode; k++)
                monitorModes.push_back(outputInfo->modes[k]);
            
            GLuint lastWidth = 0;
            GLuint lastHeight = 0;
            for (int outputModeID = 0; outputModeID < outputInfo->nmode; outputModeID++)
            {
                for (int screenModeID = 0; screenModeID < screenRes->nmode; screenModeID++)
                {
                    if (screenRes->modes[screenModeID].id == outputInfo->modes[outputModeID])
                    {
                        if (lastWidth == screenRes->modes[screenModeID].width && lastHeight == screenRes->modes[screenModeID].height)
                            continue;
                        
                        modes.emplace_back(outputInfo->modes[outputModeID],
                                           screenRes->modes[screenModeID].width,
                                           screenRes->modes[screenModeID].height,
                                           calculateXRefreshRate(&screenRes->modes[screenModeID]));
                        std::cout << screenRes->modes[screenModeID].width << "X" << screenRes->modes[screenModeID].height
                                  << " " << calculateXRefreshRate(&screenRes->modes[screenModeID]) << "Hz"  "\n";
                                  
                        lastWidth = screenRes->modes[screenModeID].width;
                        lastHeight = screenRes->modes[screenModeID].height;
                        break;
                    }
                }
            }
            std::cout << "\n";
            
            m_monitors.emplace_back(monitorIndex, 
                                    Rect(monitorInfo[monitorIndex].x,
                                         monitorInfo[monitorIndex].y,
                                         monitorInfo[monitorIndex].x + monitorInfo[monitorIndex].width,
                                         monitorInfo[monitorIndex].y + monitorInfo[monitorIndex].height),
                                    std::move(modes),
                                    std::move(outputs));
            
            XRRFreeOutputInfo (outputInfo);
        }
    }
    
    XRRFreeMonitors(monitorInfo);
    XRRFreeScreenResources(screenRes);
}

//-----------------------------------------------------------------------------
// Name : getMonitorsModes ()
//-----------------------------------------------------------------------------
std::vector<std::vector<Mode1>> LinuxGameWin::getMonitorsModes() const
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
// Name : createWindow ()
//-----------------------------------------------------------------------------
bool LinuxGameWin::createWindow(int width, int height ,GLXFBConfig bestFbc)
{
    if (!bestFbc)
        return false;
    
    // Get a visual
    XVisualInfo *vi = glXGetVisualFromFBConfig( m_display, bestFbc );
    std::cout << "Chosen visual ID = 0x" << std::hex << vi->visualid << "\n";
    std::cout << std::dec;
    
    int samp_buf, samples;
    glXGetFBConfigAttrib( m_display, bestFbc, GLX_SAMPLE_BUFFERS, &samp_buf );
    glXGetFBConfigAttrib( m_display, bestFbc, GLX_SAMPLES       , &samples  );
    std::cout << "Choosen sample buffers = " << samp_buf << " and samples = " << samples << "\n";
    
    std::cout << "Creating colormap\n";
    XSetWindowAttributes swa;
    //Colormap cmap;
    swa.colormap = cmap = XCreateColormap( m_display, RootWindow( m_display, vi->screen ), vi->visual, AllocNone );
    swa.background_pixmap = None ;
    swa.border_pixel      = 0;
    swa.event_mask        = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask |
                            ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask;
    //swa.override_redirect = true;
    
    
                            
    std::cout << "Creating window\n";    
    m_win = XCreateWindow( m_display,
                           RootWindow( m_display, vi->screen ),
                           0,
                           0,
                           width,
                           height,
                           0,
                           vi->depth,
                           InputOutput,
                           vi->visual,
                           CWBorderPixel|CWColormap|CWEventMask,
                           &swa );
    
    getMonitorsInfo();
    
    XMapWindow(m_display, m_win);
    // add window hint to disable compositing
    Atom bypassCompositor = XInternAtom(m_display, "_NET_WM_BYPASS_COMPOSITOR", False);
    int hint = 1;
    XChangeProperty(m_display, m_win, bypassCompositor,
                    XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&hint, 1);
    
    moveWindowToMonitor(m_primaryMonitorIndex);
        
//     std::cout << "screen sizes!!!!!!!!1\n";
//     for (int i = 0; i < sizes; i++)
//         std::cout << screenSize[i].width << "X" << screenSize[i].height << "\n";
//     
//     m_hDpi = screenSize->mwidth ? std::round((screenSize->width *10 * 25.4f) / screenSize->mwidth) : 0.0f;
//     m_vDpi = screenSize->mheight ? std::round((screenSize->height *10 * 25.4f) / screenSize->mheight) : 0.0f;

    if ( !m_win )
    {
        std::cout << "Failed to create window.\n";
        return false;
    }
  
    // Done with the visual info data
    XFree( vi );

    XStoreName( m_display, m_win, "GL 3.3 Window" );
    
    std::cout << "Mapping window\n";
    XMapWindow( m_display, m_win );
    
    return true;
}

//-----------------------------------------------------------------------------
// Name : createOpenGLContext ()
//-----------------------------------------------------------------------------
bool LinuxGameWin::createOpenGLContext(GLXFBConfig bestFbc)
{
    // Get the default screen's GLX extension list
    const char *glxExts = glXQueryExtensionsString( m_display,
                                                  DefaultScreen( m_display ) );

    // NOTE: It is not necessary to create or make current to a context before
    // calling glXGetProcAddressARB
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
    
    GLXContext ctx = 0;

    // Install an X error handler so the application won't exit if GL 3.0
    // context allocation fails.
    //
    // Note this error handler is global.  All display connections in all threads
    // of a process use the same error handler, so be sure to guard against other
    // threads issuing X commands while this code is running.
    // NOTE: this might be an issue if there is more than 1 thread issuing x commands
    int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

    // Check for the GLX_ARB_create_context extension string and the function.
    // If either is not present, use GLX 1.3 context creation method.
    if ( !isExtensionSupported( glxExts, "GLX_ARB_create_context" ) || !glXCreateContextAttribsARB )
    {
        std::cout << "glXCreateContextAttribsARB() not found" << " ... using old-style GLX context\n";
        ctx = glXCreateNewContext( m_display, bestFbc, GLX_RGBA_TYPE, 0, True );
    }
    // If it does, try to get a GL 3.3 context!
    else
    {
        int context_attribs[] =
        {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 3,
            //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            None
        };

        std::cout << "Creating context\n";
        ctx = glXCreateContextAttribsARB( m_display, bestFbc, 0, True, context_attribs );

        // Sync to ensure any errors generated are processed.
        XSync( m_display, False );
        if ( !ctxErrorOccurred && ctx )
            std::cout << "Created GL 3.3 context\n";
        else
        {
            std::cout << "Failed to create GL 3.3 context";
            return false;
        }
    }
    
    // Sync to ensure any errors generated are processed.
    XSync( m_display, False );

    // Restore the original error handler
    XSetErrorHandler( oldHandler );

    if ( ctxErrorOccurred || !ctx )
    {
        std::cout << "Failed to create an OpenGL context\n";
        return false;
    }

    // Verifying that context is a direct context
    if ( ! glXIsDirect ( m_display, ctx ) )
    {
        std::cout << "Indirect GLX rendering context obtained\n";
    }
    else
    {
        std::cout << "Direct GLX rendering context obtained\n"; 
    }

    std::cout << "Making context current\n";
    glXMakeCurrent( m_display, m_win, ctx );
    
    std::cout << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << "\n";
        
    return true;
}

//-----------------------------------------------------------------------------
// Name : glSwapBuffers ()
//-----------------------------------------------------------------------------
void LinuxGameWin::glSwapBuffers()
{
	glxSwapBuffers(m_display, m_win);
}

//-----------------------------------------------------------------------------
// Name : setFullScreenMode ()
//-----------------------------------------------------------------------------
void LinuxGameWin::setFullScreenMode(bool fullscreen)
{
    Atom wm_state = XInternAtom(m_display, "_NET_WM_STATE", False);
    Atom fullscreenAtom = XInternAtom(m_display, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev;
    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = m_win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    if (fullscreen)
        xev.xclient.data.l[0] = 1;
    else
        xev.xclient.data.l[0] = 0;
    xev.xclient.data.l[1] = fullscreenAtom;
    xev.xclient.data.l[2] = 0;
    
    XMapWindow(m_display, m_win);

    XSendEvent(m_display,
               DefaultRootWindow(m_display),
               False,
               SubstructureRedirectMask | SubstructureNotifyMask,
               &xev);
     
    XSync(m_display, m_win);
    XFlush(m_display);
}

//-----------------------------------------------------------------------------
// Name : setMonitorResolution ()
//-----------------------------------------------------------------------------
bool LinuxGameWin::setMonitorResolution(int monitorIndex, Resolution newResolution)
{
    if (monitorIndex > m_monitors.size())
        return false;
    
    RRMode modeID;
    bool IDfound = false;

    std::vector<Mode1> monitorModes;
    for (MonitorInfo::Mode mode : m_monitors[monitorIndex].modes)
    {
        if (mode.width == newResolution.width && mode.height == newResolution.height)
        {
            modeID = mode.ID;
            IDfound = true;
            break;
        }
    }
    
    
    if (IDfound)
    {
        XRRScreenResources* screenRes = XRRGetScreenResources(m_display, m_win);
        
        Status status = XRRSetCrtcConfig(m_display,
                                         screenRes,
                                         screenRes->crtcs[monitorIndex],
                                         CurrentTime,
                                         m_monitors[monitorIndex].positionRect.left,
                                         m_monitors[monitorIndex].positionRect.top,
                                         modeID,
                                         1,
                                         m_monitors[monitorIndex].outputs.data(),
                                         m_monitors[monitorIndex].outputs.size());
    
        XRRFreeScreenResources(screenRes);
        
        if (!status)
            return true;
        else
            return false;
    }
    else
        return false;
}

//-----------------------------------------------------------------------------
// Name : setWindowPosition ()
//-----------------------------------------------------------------------------
void LinuxGameWin::setWindowPosition(int x, int y)
{
    XWindowChanges xWinChanges = {0};
    xWinChanges.x = x;
    xWinChanges.y = y;
    XConfigureWindow(m_display, m_win, CWX|CWY, &xWinChanges);
    XSync(m_display, m_win);
    XFlush(m_display);
}

//-----------------------------------------------------------------------------
// Name : moveWindowToMonitor ()
//-----------------------------------------------------------------------------
void LinuxGameWin::moveWindowToMonitor(int monitorIndex)
{
    Point windowPosition = getWindowPosition();
    if (!m_monitors[monitorIndex].positionRect.isPointInRect(windowPosition))
    {
        setWindowPosition(m_monitors[monitorIndex].positionRect.left,
                          m_monitors[monitorIndex].positionRect.top);
    }
}
    
//-----------------------------------------------------------------------------
// Name : initOpenGL ()
//-----------------------------------------------------------------------------
bool LinuxGameWin::initOpenGL(int width, int height)
{
    int err;
    std::cout << "InitOpenGL started\n";

    GLXFBConfig bestFbc = getBestFBConfig();
    if (!createWindow(width, height, bestFbc))
        return false; 
        
    if(!createOpenGLContext(bestFbc))
        return false;
    
    // register interest in the delete window message
    wmDeleteMessage = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_display, m_win, &wmDeleteMessage, 1);

    glewInit();

    //------------------------------------
    // Create empty cursor pixmap
    //------------------------------------
    XColor color = { 0 };
    const char data[] = { 0 };

    Pixmap pixmap = XCreateBitmapFromData(m_display, m_win, data, 1,1);
    emptyCursorPixmap = XCreatePixmapCursor(m_display, pixmap, pixmap, &color, &color, 0, 0);

    XFreePixmap(m_display,pixmap);

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
    reshape(width,height);
    
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
bool LinuxGameWin::isExtensionSupported(const char *extList, const char *extension)
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
// Name : ctxErrorHandler ()
//-----------------------------------------------------------------------------
int LinuxGameWin::ctxErrorHandler( Display *dpy, XErrorEvent *ev)
{
    ctxErrorOccurred = true;
    return 0;
}

//-----------------------------------------------------------------------------
// Name : sendEventToXWindow ()
// Desc : sends a SelectionNotify event to X Window with the given data
//        if type is None it means there was no data that could be sent
//-----------------------------------------------------------------------------
void LinuxGameWin::sendEventToXWindow(XSelectionRequestEvent *sev, Atom type, int bitsPerDataElement, unsigned char *data, int dataLength)
{
    XSelectionEvent ssev;

    ssev.type = SelectionNotify;
    ssev.requestor = sev->requestor;
    ssev.selection = sev->selection;
    ssev.target = sev->target;
    ssev.property = sev->property;
    ssev.time = sev->time;

    if (type != None)
    {
        XChangeProperty( s_clipboardDisplay, sev->requestor, sev->property, type, bitsPerDataElement, PropModeReplace,
                        data, dataLength);
        ssev.property = sev->property;
    }
    else
        // send "nope" to requesting client
        // can't convert clipboard data to client desired type
        ssev.property = None;

    XSendEvent( s_clipboardDisplay, sev->requestor, False, NoEventMask, (XEvent *)&ssev);
}

//-----------------------------------------------------------------------------
// Name : sendClipboardLoop ()
//-----------------------------------------------------------------------------
void LinuxGameWin::sendClipboardLoop(Window clipboardWindow)
{
    while(1)
    {
        XEvent ev;
        XSelectionRequestEvent *sev;

        XNextEvent( s_clipboardDisplay, &ev);
        std::cout <<"got event :" << ev.type << "\n";
        switch (ev.type)
        {
            case SelectionClear:
            {
                    std::cout << "Lost selection ownership\n";
                    // close our temp window
                    XDestroyWindow( s_clipboardDisplay, clipboardWindow);
                    clipboardWindow = 0;
                    return;
            }break;

            case SelectionRequest:
                sev = (XSelectionRequestEvent*)&ev.xselectionrequest;
                if (sev->target == s_utf8)
                {
                    std::cout << "sent utf8 \n";
                    sendEventToXWindow(sev, s_utf8, 8, reinterpret_cast<unsigned char*>(const_cast<char*>(s_clipboardString.c_str())), s_clipboardString.length());
                }
                else
                    if (sev->target == s_targets)
                    {
                        std::cout << "sent targets\n";
                        Atom supportedFormats[] = {s_targets , s_utf8};
                        sendEventToXWindow(sev, XA_ATOM, 32, reinterpret_cast<unsigned char*>(supportedFormats), sizeof(Atom) * 2);
                    }
                    else
                    {
                        std::cout << "got unknown\n";
                        sendEventToXWindow(sev, None, 0, nullptr, 0);
                    }
                break;
        }
    }
}

//-----------------------------------------------------------------------------
// Name : copyToClipboard ()
//-----------------------------------------------------------------------------
void LinuxGameWin::copyToClipboard(const std::string &text)
{
    s_clipboardString = text;

    std::cout << "copy to clipboard\n";
    // check if the thread is still running
    if ( s_clipboardSender.valid())
    {
        std::future_status status = s_clipboardSender.wait_for(std::chrono::microseconds(0));
        if (status != std::future_status::ready)
        {
            return;
        }
    }

    std::cout << "finishing copying\n";
    // either first time or clipthread is not running
    Window root = RootWindow( s_clipboardDisplay, DefaultScreen( s_clipboardDisplay ));

    // We need a window to receive messages from other clients.
    s_clipboardWindow = XCreateSimpleWindow( s_clipboardDisplay, root, -10, -10, 1, 1, 0, 0, 0);
    XSelectInput( s_clipboardDisplay, s_clipboardWindow, SelectionClear | SelectionRequest);
    // Claim ownership of the clipboard.
    XSetSelectionOwner( s_clipboardDisplay, s_selection, s_clipboardWindow, CurrentTime);
    // Create a thread to send the clipboard while we are the owners of it
    s_clipboardSender = std::async(std::launch::async, GameWin::sendClipboardLoop, s_clipboardWindow );

}

//-----------------------------------------------------------------------------
// Name : PasteClipboard ()
//-----------------------------------------------------------------------------
std::string LinuxGameWin::PasteClipboard()
{
    Display * pasteDisplay = XOpenDisplay(NULL);
    Timer timer;

    if (!pasteDisplay)
    {
        std::cout << "Failed to open X display\n";
        return "";
    }

    Window root = RootWindow(pasteDisplay, DefaultScreen(pasteDisplay));

    Window owner = XGetSelectionOwner(pasteDisplay, s_selection);
    if (owner == None)
    {
        std::cout <<"'CLIPBOARD' has no owner\n";
        XCloseDisplay(pasteDisplay);
        return "";
    }

    // we are the owner of the clipboard return the saved string
    if (owner == s_clipboardWindow )
    {
        XCloseDisplay(pasteDisplay);
        return s_clipboardString;
    }

    // The selection owner will store the data in a property on this window:
    Window target_window = XCreateSimpleWindow(pasteDisplay, root, -10, -10, 1, 1, 0, 0, 0);
    XSelectInput(pasteDisplay, target_window, SelectionNotify);

    // That's the property used by the owner. Note that it's completely arbitrary.
    Atom target_property = XInternAtom(pasteDisplay, "ChessClipboard", False);

    // Request conversion to UTF-8. Not all owners will be able to fulfill that request.
    XConvertSelection(pasteDisplay, s_selection, s_utf8, target_property, target_window, CurrentTime);

    double startTime = timer.getCurrentTime();

    while (1)
    {
        XEvent ev;
        XSelectionEvent *sev;

        //too much time has passed and still no answer abort
        if (timer.getCurrentTime() - startTime > 0.1)
        {
            XDestroyWindow( pasteDisplay, target_window );
            XCloseDisplay(pasteDisplay);
            return "";
        }

        // make sure there is a message in queue to not block
        if (XPending(pasteDisplay) <= 0)
            continue;

        XNextEvent(pasteDisplay, &ev);
        switch (ev.type)
        {
            case SelectionNotify:
                sev = (XSelectionEvent*)&ev.xselection;
                if (sev->property == None)
                {
                    std::cout <<"Conversion could not be performed.\n";
                    return "";
                }
                else
                {
                    Atom da, incr, type;
                    int di;
                    unsigned long size, dul;
                    unsigned char *prop_ret = nullptr;

                    // Dummy call to get type and size.
                    XGetWindowProperty(pasteDisplay, target_window, target_property, 0, 0, False, AnyPropertyType,
                                       &type, &di, &dul, &size, &prop_ret);
                    XFree(prop_ret);

                    incr = XInternAtom(pasteDisplay, "INCR", False);
                    if (type == incr)
                    {
                        std::cout << "Data too large and INCR mechanism not implemented\n";
                        return "";
                    }

                    // Read the data in one go.
                    XGetWindowProperty(pasteDisplay, target_window, target_property, 0, size, False, AnyPropertyType,
                                       &da, &di, &dul, &dul, &prop_ret);
                    // Save the returned string
                    std::string ret  = reinterpret_cast<char*>(prop_ret);

                    // Free resourses
                    XFree(prop_ret);
                    // Signal the selection owner that we have successfully read the data.
                    XDeleteProperty(pasteDisplay, target_window, target_property);
                    // close our temp window
                    XDestroyWindow( pasteDisplay, target_window );

                    XCloseDisplay(pasteDisplay);

                    return ret;
                }
                break;
        }
    }
}

//-----------------------------------------------------------------------------
// Name : setCursorPos ()
//-----------------------------------------------------------------------------
void LinuxGameWin::setCursorPos(Point newPos)
{
    XWarpPointer(m_display, None, m_win, 0, 0, 0, 0, newPos.x, newPos.y);
}

//-----------------------------------------------------------------------------
// Name : getCursorPos ()
//-----------------------------------------------------------------------------
Point LinuxGameWin::getCursorPos()
{
    Window root,child;
    int cursorX  = 0, cursorY = 0 ,x = 0,y = 0;
    unsigned int maskRet;

    XQueryPointer(m_display, m_win, &root, &child ,&x, &y,
                  &cursorX, &cursorY, &maskRet);

    return Point(cursorX, cursorY);
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
//-----------------------------------------------------------------------------
int LinuxGameWin::BeginGame()
{
    while (gameRunning) 
    {
        XEvent event;

         // Handle all messages before rendering the next frame
        while (XPending(m_display) > 0 && gameRunning)
        {
            XNextEvent(m_display, &event);

            ModifierKeysStates modifierKeys(keysStatus[KEY_LEFTSHIFT] || keysStatus[KEY_RIGHTSHIFT],
                                   keysStatus[KEY_LEFTCTRL] ||  keysStatus[KEY_RIGHTCTRL],
                                   keysStatus[KEY_LEFTALT || keysStatus[KEY_RIGHTALT]]);

            switch(event.type)
            {
            case Expose:
            {
                XWindowAttributes gwa;

                XGetWindowAttributes(m_display, m_win, &gwa);
                reshape(gwa.width, gwa.height);
                
                //XRaiseWindow(m_display, m_win);
                //XSetInputFocus(m_display, m_win, RevertToPointerRoot, CurrentTime);
            }break;

            case FocusIn:
            {
                //XRaiseWindow(m_display, m_win);
                //XSetInputFocus(m_display, m_win, RevertToPointerRoot, CurrentTime);
            }break;
            
            case KeyPress:
            case KeyRelease:
            {
                char buf[128];
                KeySym key;

                if (event.xkey.keycode - 8 >= 0)
                    keysStatus[event.xkey.keycode - 8] = event.type == KeyPress;

                if(event.type == KeyPress)
                    std::cout << "key pressed\n";
                else
                    std::cout << "key released\n";
                
                std::cout << event.xkey.keycode << "\n";
                XLookupString(&event.xkey, buf, 128, &key, nullptr);
                std::cout << "KeySym: " << key << " " << "char: " <<  buf << "\n";
                if (buf[0] == '\0')
                {
                    std::cout << "no valid ascii\n";
                    if (key > 0xff && key <= 0xffff)
                    {
                        int temp = key - 0xff00;
                        GK_VirtualKey vKey = linuxVirtualKeysTable[temp];
                        std::cout << "Virtual key was " << (int)vKey << "\n";

                        sendVirtualKeyEvent(vKey, event.type == KeyPress, modifierKeys);
                    }
                }
                else
                {
                    sendKeyEvent(buf[0], event.type == KeyPress);
                }
            }break;
            
            case MotionNotify:
            {
                sendMouseEvent(MouseEvent(MouseEventType::MouseMoved, Point(event.xbutton.x, event.xbutton.y), false, timer.getCurrentTime(), 0), modifierKeys);
            }break;

            case ButtonPress:
            {
                if (event.xbutton.button == Button1)
                {
                    std::cout << "left button pressed\n";
                        
                    oldCursorLoc.x = event.xbutton.x;
                    oldCursorLoc.y = event.xbutton.y;
                    mouseDrag = true;
                    XDefineCursor(m_display, m_win, emptyCursorPixmap);

                    double curTime = timer.getCurrentTime();
                    if (curTime - lastLeftClickTime < s_doubleClickTime)
                    {
                        std::cout << "left button was double clicked\n";
                        sendMouseEvent(MouseEvent(MouseEventType::DoubleLeftButton, Point(event.xbutton.x, event.xbutton.y), true, timer.getCurrentTime(), 0), modifierKeys);
                    }
                    else
                        sendMouseEvent(MouseEvent(MouseEventType::LeftButton, Point(event.xbutton.x, event.xbutton.y), true, timer.getCurrentTime(), 0), modifierKeys);
                    lastLeftClickTime = curTime;
                }

                if (event.xbutton.button == Button3)
                {
                    std::cout << "right button pressed\n";

                    double curTime = timer.getCurrentTime();
                    if (curTime - lastRightClickTime < s_doubleClickTime)
                    {
                        std::cout << "right button was double clicked\n";
                        sendMouseEvent(MouseEvent(MouseEventType::DoubleRightButton, Point(event.xbutton.x, event.xbutton.y), true, timer.getCurrentTime(), 0), modifierKeys);
                    }
                    else
                        sendMouseEvent(MouseEvent(MouseEventType::RightButton, Point(event.xbutton.x, event.xbutton.y),true, timer.getCurrentTime(), 0), modifierKeys);
                    lastRightClickTime = curTime;

                }

                if (event.xbutton.button == Button4)
                {
                    std::cout << "mouse scroll up\n";
                    sendMouseEvent(MouseEvent(MouseEventType::ScrollVert, Point(event.xbutton.x, event.xbutton.y), false, timer.getCurrentTime(), 1), modifierKeys);
                }

                if (event.xbutton.button == Button5)
                {
                    std::cout << "mouse scroll down\n";
                    sendMouseEvent(MouseEvent(MouseEventType::ScrollVert, Point(event.xbutton.x, event.xbutton.y), true, timer.getCurrentTime(), -1), modifierKeys);
                }

            }break;

            case ButtonRelease:
            {
                if (event.xbutton.button == Button1)
                {
                    std::cout << "left button released\n";
                    mouseDrag = false;
                    XUndefineCursor(m_display, m_win);
                    sendMouseEvent(MouseEvent(MouseEventType::LeftButton, Point(event.xbutton.x, event.xbutton.y), false, timer.getCurrentTime(), 0), modifierKeys);
                }

                if (event.xbutton.button == Button3)
                {
                    std::cout << "right button released\n";
                    sendMouseEvent(MouseEvent(MouseEventType::RightButton, Point(event.xbutton.x, event.xbutton.y), false, timer.getCurrentTime(), 0), modifierKeys);
                }

            }break;
                        
            // got quit message, quitting the game loop
            case ClientMessage:
                std::cout << "Shutting down now!!!\n";
                gameRunning = false;
            break;

            }

        }
        
        timer.frameAdvanced();

       int err = glGetError();
       if (err != GL_NO_ERROR)
           std::cout <<"MsgLoop: ERROR bitches\n";

        if (!timer.isCap())
        {
            drawing();
        }
    }
    
    return 0;
}

//-----------------------------------------------------------------------------
// Name : Shutdown ()
//-----------------------------------------------------------------------------
bool LinuxGameWin::Shutdown()
{
    // send event to the clipboard window to make the clipboard thread to shutdown
    if ( s_clipboardWindow != 0)
    {
        XSelectionClearEvent event;
        event.type = SelectionClear;
        event.display = s_clipboardDisplay;
        event.time = CurrentTime;
        // fake that something took ownership of the clipboard
        XSendEvent(m_display, s_clipboardWindow, False, NoEventMask, (XEvent *)&event);
    }
    
    // Properly de-allocate all resources once they've outlived their purpose
    glXMakeCurrent( m_display, 0, 0 );
    glXDestroyContext( m_display, ctx );

    XFreeCursor(m_display, emptyCursorPixmap);
    XDestroyWindow( m_display, m_win );
    XFreeColormap( m_display, cmap );
    XCloseDisplay( m_display );
    XCloseDisplay( s_clipboardDisplay );
        
    return true;
    
}

//-----------------------------------------------------------------------------
// Name : getWindowPosition ()
//-----------------------------------------------------------------------------
Point LinuxGameWin::getWindowPosition()
{
        XWindowAttributes windowAttributes;
    int x,y;
    Window child;
    XTranslateCoordinates( m_display, m_win, DefaultRootWindow(m_display), 0, 0, &x, &y, &child );
    XGetWindowAttributes(m_display, DefaultRootWindow(m_display), &windowAttributes);
    
    return Point(x - windowAttributes.x, y - windowAttributes.y);
}

//-----------------------------------------------------------------------------
// Name : getWindowCurrentMonitor ()
//-----------------------------------------------------------------------------
GLuint LinuxGameWin::getWindowCurrentMonitor()
{    
    for (GLuint i = 0; i < m_monitors.size(); i++)
    {
        if (m_monitors[i].positionRect.isPointInRect(getWindowPosition()))
            return i;
    }
    
    return 0;
}
