#include "GameWin.h"
#include <GL/glut.h>
#include <unistd.h>

/* Helper function to convert GLSL types to storage sizes */
size_t TypeSize(GLenum type)
{
    size_t size;

    #define CASE(Enum, Count, Type) \
    case Enum: size = Count * sizeof(Type); break
    switch (type)
    {
    CASE(GL_FLOAT,              1,  GLfloat);
    CASE(GL_FLOAT_VEC2,         2,  GLfloat);
    CASE(GL_FLOAT_VEC3,         3,  GLfloat);
    CASE(GL_FLOAT_VEC4,         4,  GLfloat);
    CASE(GL_INT,                1,  GLint);
    CASE(GL_INT_VEC2,           2,  GLint);
    CASE(GL_INT_VEC3,           3,  GLint);
    CASE(GL_INT_VEC4,           4,  GLint);
    CASE(GL_UNSIGNED_INT,       1,  GLuint);
    CASE(GL_UNSIGNED_INT_VEC2,  2,  GLuint);
    CASE(GL_UNSIGNED_INT_VEC3,  3,  GLuint);
    CASE(GL_UNSIGNED_INT_VEC4,  4,  GLuint);
    CASE(GL_BOOL,               1,  GLboolean);
    CASE(GL_BOOL_VEC2,          2,  GLboolean);
    CASE(GL_BOOL_VEC3,          3,  GLboolean);
    CASE(GL_BOOL_VEC4,          4,  GLboolean);
    CASE(GL_FLOAT_MAT2,         4,  GLfloat);
    CASE(GL_FLOAT_MAT2x3,       6,  GLfloat);
    CASE(GL_FLOAT_MAT2x4,       8,  GLfloat);
    CASE(GL_FLOAT_MAT3,         9,  GLfloat);
    CASE(GL_FLOAT_MAT3x2,       6,  GLfloat);
    CASE(GL_FLOAT_MAT3x4,       12, GLfloat);
    CASE(GL_FLOAT_MAT4,         16, GLfloat);
    CASE(GL_FLOAT_MAT4x2,       8,  GLfloat);
    CASE(GL_FLOAT_MAT4x3,       12, GLfloat);
    #undef CASE

    default:
        std::cerr << "Unknown type:" << type << "\n";
        exit(EXIT_FAILURE);
    break;
    }

    return size;
}


bool GameWin::ctxErrorOccurred = false;

//-----------------------------------------------------------------------------
// Name : GameWin (constructor)
//-----------------------------------------------------------------------------
GameWin::GameWin()
    :font_("NotoMono")
{
    m_display = nullptr;
    
    gameRunning = true;
    ctx = nullptr;

    for (int i = 0; i < 256; i++)
        keysStatus[i] = false;

    mouseDrag = false;

    faceCount = -1;
    meshIndex = -1;
    hit = false;
    selectedObj = nullptr;
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
    m_display = XOpenDisplay(NULL);
    
    if (!m_display)
    {
        std::cout << "Failed to open X display\n";
        return false;
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// Name : initOpenGL ()
//-----------------------------------------------------------------------------
bool GameWin::initOpenGL(int width, int height)
{
    int err;
    std::cout << "InitOpenGL started\n";

    //------------------------------------
    // Window and OpenGL context creation
    //------------------------------------
    // Get a matching FB config
  /*static*/ int visual_attribs[] =
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
    if ( !glXQueryVersion( m_display, &glx_major, &glx_minor ) ||
        ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
    {
        std::cout << "Invalid GLX Version\n";
        return false;
    }
       
    std::cout << "Getting matching framebuffer configs\n"; 
  
    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(m_display, DefaultScreen(m_display), visual_attribs, &fbcount);
    if (!fbc)
    {
        std::cout << "Failed to retrieve a framebuffer config\n";
        return false;
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
      
            //std::cout << "  Matching fbconfig " << i << " , visual ID 0x" << std::hex << vi->visualid
            // << ": SAMPLE_BUFFERS = " << samp_buf << " SAMPLES = " << samples << "\n";
      
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

    // Get a visual
    XVisualInfo *vi = glXGetVisualFromFBConfig( m_display, bestFbc );
    std::cout << "Chosen visual ID = 0x" << std::hex << vi->visualid << "\n";

    std::cout << "Creating colormap\n";
    XSetWindowAttributes swa;
    //Colormap cmap;
    swa.colormap = cmap = XCreateColormap( m_display, RootWindow( m_display, vi->screen ), vi->visual, AllocNone );
    swa.background_pixmap = None ;
    swa.border_pixel      = 0;
    swa.event_mask        = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask |
                            ButtonPressMask | ButtonReleaseMask;
    
    std::cout << "Creating window\n";
    m_win = XCreateWindow( m_display, RootWindow( m_display, vi->screen ),
                              0, 0, width, height, 0, vi->depth, InputOutput,
                              vi->visual, 
                              CWBorderPixel|CWColormap|CWEventMask, &swa );

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
    
    // Get the default screen's GLX extension list
    const char *glxExts = glXQueryExtensionsString( m_display,
                                                  DefaultScreen( m_display ) );

    // NOTE: It is not necessary to create or make current to a context before
    // calling glXGetProcAddressARB
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
    
    //GLXContext ctx = 0;

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
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_NORMALIZE);

    //------------------------------------
    // buffers creation
    //------------------------------------
    // restore cout to print numbers in decimal base
    std::cout<<std::dec;

    glBindVertexArray(0);

    //------------------------------------
    // Shader loading
    //------------------------------------
    // complie shaders
    textShader = new Shader("text.vs", "text.frag");
    textureShader = new Shader("texture.vs", "texture.frag");
    spriteShader = new Shader("sprite.vs", "sprite.frag");
    //projShader = new Shader("shader2.vs", "shader2.frag");

    //------------------------------------
    // Init Scene
    //------------------------------------
    m_scene.InitScene();
    m_scene.InitCamera(width, height);
    selectedObj = &m_scene.GetObject(1);

    // make sure the viewport is updated
    reshape(width,height);
    // init our font
    font_.init(height);

    m_sprite.Init();

    //------------------------------------
    // Init Dialog
    //------------------------------------
    GLuint buttonTexture = m_asset.getTexture("woodGUI2.png");
    m_dialog.init(300,300, 18, "Caption!", "", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), m_asset);
    m_dialog.setLocation(50, 100);
    ButtonUI* pButton;
    m_dialog.addButton(1, "button text", 20,20, 200, 25, 0, &pButton);
    std::vector<ELEMENT_GFX> buttonGFX;
    buttonGFX.emplace_back(buttonTexture, Rect(0, 0, 84, 34));
    buttonGFX.emplace_back(buttonTexture, Rect(0, 34, 84, 68));
    pButton->setControlGFX(buttonGFX);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout <<"Init: ERROR bitches\n";
        std::cout << gluErrorString(err);
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
// Name : ctxErrorHandler ()
//-----------------------------------------------------------------------------
int GameWin::ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
    ctxErrorOccurred = true;
    return 0;
}

//-----------------------------------------------------------------------------
// Name : drawing ()
//-----------------------------------------------------------------------------
void GameWin::drawing()
{
    int err;

    ProcessInput(timer.getTimeElapsed());

    glEnable(GL_CULL_FACE);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //TODO: Move this  to some where sane
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_scene.Darwing();
	
    // Render debug text to the screen
    textShader->Use();

    std::stringstream ss;
    ss << timer.getFPS();
    ss << " ";
    //glm::vec3 obj2Pos = m_scene.GetObject(1).GetPosition();
    glm::vec3 obj2Pos = selectedObj->GetPosition();
    ss << obj2Pos.x;
    ss << " ";
    ss << obj2Pos.y;
    ss << " ";
    ss << obj2Pos.z;
    ss << " ";
    ss << faceCount;
    ss << " ";
    ss << meshIndex;
    ss << " ";
    if (hit)
        ss << "Hit!";
    else
        ss << "Miss :(";

    font_.renderText(textShader, ss.str(),0.0f, 0.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));

    int textureName = m_asset.getTexture("gold.png");

    glDisable(GL_DEPTH_TEST);

    m_dialog.OnRender(0, m_sprite, m_asset);
    m_sprite.Render(spriteShader);
    m_sprite.Clear();
    
    glEnable(GL_DEPTH_TEST);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout <<"Drawing: ERROR bitches " << err << "\n";
        std::cout << gluErrorString(err);
    }

    glXSwapBuffers (m_display, m_win);
}

//-----------------------------------------------------------------------------
// Name : reshape ()
//-----------------------------------------------------------------------------
void GameWin::reshape(int width, int height)
{
    float left,right,bottom,top;
    float AR;
	
    if(m_winWidth != width || m_winHeight != height)
    {
        std::cout <<"reshape called\n";
        m_winWidth = width;
        m_winHeight = height;

        height_ = height;

        font_.setScreenHeight(height);

        m_scene.reshape(m_winWidth,m_winHeight);

        textShader->Use();
        glUniform2i( glGetUniformLocation(textShader->Program, "screenSize"), width / 2, height / 2);
        textureShader->Use();
        glUniform2i( glGetUniformLocation(textureShader->Program, "screenSize"), width / 2, height / 2);
        spriteShader->Use();
        glUniform2i( glGetUniformLocation(spriteShader->Program, "screenSize"), width / 2, height / 2);
    }
    else
        std::cout <<"reshape ignored\n";
}

//-----------------------------------------------------------------------------
// Name : ProcessInput ()
//-----------------------------------------------------------------------------
void GameWin::ProcessInput(float timeDelta)
{
    float X = 0.0f,Y = 0.0f;
    if (mouseDrag)
    {
        int cursorX;
        int cursorY;
        //float X = 0.0f,Y = 0.0f;
        bool ret;

        Window root,child;
        int x,y;
        unsigned int maskRet;


        ret = XQueryPointer(m_display, m_win, &root, &child ,&x, &y,
                            &cursorX, &cursorY, &maskRet);

        std::cout <<"process input:\n";
        std::cout << "x = " << cursorX << " y = " << cursorY << "\n";

        X = (float)(cursorX - oldCursorLoc.x) / 3.0f;
        Y = (float)(cursorY - oldCursorLoc.y) / 3.0f;

        XWarpPointer(m_display, None, m_win, 0, 0, 0, 0, oldCursorLoc.x, oldCursorLoc.y);
        XFlush(m_display);
    }
    m_scene.processInput(timeDelta, keysStatus, X, Y);
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
//-----------------------------------------------------------------------------
int GameWin::BeginGame()
{
    while (gameRunning) 
    {
        XEvent event;

         // Handle all messages before rendering the next frame
        while (XPending(m_display) > 0 && gameRunning)
        {
            XNextEvent(m_display, &event);

            switch(event.type)
            {

            case Expose:
            {
                XWindowAttributes gwa;

                XGetWindowAttributes(m_display, m_win, &gwa);
                reshape(gwa.width, gwa.height);
            }break;

            case KeyPress:
            {
                char buf[128];
                KeySym key;

                std::cout << "key pressed\n";
                std::cout << event.xkey.keycode << "\n";
                XLookupString(&event.xkey, buf, 128, &key, nullptr);
                std::cout << "KeySym: " << key << " " << "char: " <<  buf <<"\n";

                keysStatus[event.xkey.keycode] = true;
            }break;

            case KeyRelease:
            {
                std::cout << "key released\n";
                std::cout << event.xkey.keycode << "\n";

                keysStatus[event.xkey.keycode] = false;
            }break;

            case ButtonPress:
            {
                if (event.xbutton.button == Button1)
                {
                    std::cout << "left button pressed\n";
                    oldCursorLoc.x = event.xbutton.x;
                    oldCursorLoc.y = event.xbutton.y;
                    mouseDrag = true;
                    int ret = XDefineCursor(m_display, m_win, emptyCursorPixmap);
                    ret++;

//                    std::cout <<"loop:\n";
//                    std::cout << "x = " << event.xbutton.x << " y = " << event.xbutton.y << "\n";
                }

                if (event.xbutton.button == Button3)
                {
                    std::cout << "right button pressed\n";
                    Point cursorPoint;
                    cursorPoint.x = event.xbutton.x;
                    cursorPoint.y = event.xbutton.y;
                    Object* pObj = m_scene.PickObject(cursorPoint,faceCount, meshIndex);
                    if (pObj != nullptr)
                    {
                        selectedObj = pObj;
                        hit = 1;
                    }
                    else
                        hit = 0;
                }

            }break;

            case ButtonRelease:
            {
                if (event.xbutton.button == Button1)
                {
                    std::cout << "left button released\n";
                    mouseDrag = false;
                    XUndefineCursor(m_display, m_win);
                }
                else
                    std::cout << "mouse released\n";
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
bool GameWin::Shutdown()
{
    // Properly de-allocate all resources once they've outlived their purpose
    //delete meshShader;
    delete textShader;

    glXMakeCurrent( m_display, 0, 0 );
    glXDestroyContext( m_display, ctx );

    XFreeCursor(m_display, emptyCursorPixmap);
    XDestroyWindow( m_display, m_win );
    XFreeColormap( m_display, cmap );
    XCloseDisplay( m_display );
    
    return true;
}
