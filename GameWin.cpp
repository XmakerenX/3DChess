#include "GameWin.h"
#include <GL/glut.h>

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
    :font_("NotoMono"),
    vertices2{{0.0f,100.0f,0.0f,0.0f}, {0.0f,228.0f,0.0f,1.0f}, {128.0f,228.0f,1.0f,1.0f}, {128.0f,100.0f,1.0f,0.0f}}
{
    display = nullptr;
    
    gameRunning = true;
    ctx = nullptr;

    // point 1  xyz
    vertices[0] = -0.5f;
    vertices[1] = -0.5f;
    vertices[2] = 0.0f;
    // point 1 rgb
    vertices[3] = 1.0f;
    vertices[4] = 0.0f;
    vertices[5] = 0.0f;
    // point 2 xyz
    vertices[6] = 0.5f;
    vertices[7] = -0.5f;
    vertices[8] = 0.0f;
    // point 2 rgb
    vertices[9] = 0.0f;
    vertices[10] = 1.0f;
    vertices[11] = 0.0f;
    // point 3 xyz
    vertices[12] = 0.0f;
    vertices[13] = 0.5f;
    vertices[14] = 0.0f;
    // point 3 rgb
    vertices[15] = 0.0f;
    vertices[16] = 0.0f;
    vertices[17] = 1.0f;

    // set indices for quad rendering
    // first triangle
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    // secound triangle
    indices[3] = 0;
    indices[4] = 3;
    indices[5] = 2;

    for (int i = 0; i < 256; i++)
        keysStatus[i] = false;

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
    display = XOpenDisplay(NULL);
    
    if (!display)
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
    std::cout << "InitOpenGL started\n";

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
 
    if (display == nullptr)
        std::cout << "No X display! but it did open it....\n";
    
    // FBConfigs were added in GLX version 1.3.
    if ( !glXQueryVersion( display, &glx_major, &glx_minor ) ||  
        ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
    {
        std::cout << "Invalid GLX Version\n";
        return false;
    }
       
    std::cout << "Getting matching framebuffer configs\n"; 
  
    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
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
        XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
        if ( vi )
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
            glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );
      
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
    XVisualInfo *vi = glXGetVisualFromFBConfig( display, bestFbc );
    std::cout << "Chosen visual ID = 0x" << std::hex << vi->visualid << "\n";

    std::cout << "Creating colormap\n";
    XSetWindowAttributes swa;
    //Colormap cmap;
    swa.colormap = cmap = XCreateColormap( display, RootWindow( display, vi->screen ), vi->visual, AllocNone );
    swa.background_pixmap = None ;
    swa.border_pixel      = 0;
    swa.event_mask        = StructureNotifyMask | ExposureMask;
    
    std::cout << "Creating window\n";
    win = XCreateWindow( display, RootWindow( display, vi->screen ), 
                              0, 0, width, height, 0, vi->depth, InputOutput,
                              vi->visual, 
                              CWBorderPixel|CWColormap|CWEventMask, &swa );

    if ( !win )
    {
        std::cout << "Failed to create window.\n";
        return false;
    }
  
    // Done with the visual info data
    XFree( vi );

    XStoreName( display, win, "GL 3.3 Window" );
    
    std::cout << "Mapping window\n";
    XMapWindow( display, win );  
    
    // Get the default screen's GLX extension list
    const char *glxExts = glXQueryExtensionsString( display,
                                                  DefaultScreen( display ) );

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
        ctx = glXCreateNewContext( display, bestFbc, GLX_RGBA_TYPE, 0, True );
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
        ctx = glXCreateContextAttribsARB( display, bestFbc, 0, True, context_attribs );

        // Sync to ensure any errors generated are processed.
        XSync( display, False );
        if ( !ctxErrorOccurred && ctx )
            std::cout << "Created GL 3.3 context\n";
        else
        {
            std::cout << "Failed to create GL 3.3 context";
            return false;
        }
    }
    
    XSelectInput(display, win, KeyPressMask | KeyReleaseMask);

    // Sync to ensure any errors generated are processed.
    XSync( display, False );

    // Restore the original error handler
    XSetErrorHandler( oldHandler );

    if ( ctxErrorOccurred || !ctx )
    {
        std::cout << "Failed to create an OpenGL context\n";
        return false;
    }

    // Verifying that context is a direct context
    if ( ! glXIsDirect ( display, ctx ) )
    {
        std::cout << "Indirect GLX rendering context obtained\n";
    }
    else
    {
        std::cout << "Direct GLX rendering context obtained\n"; 
    }

    std::cout << "Making context current\n";
    glXMakeCurrent( display, win, ctx );
    
    // register interest in the delete window message
    wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, win, &wmDeleteMessage, 1);

    glewInit();

    // restore cout to print numbers in decimal base
    std::cout<<std::dec;

    glGenVertexArrays(1, &VA0);
    glGenBuffers(1, &VB0);
    glBindVertexArray(VA0);

    glBindBuffer(GL_ARRAY_BUFFER, VB0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &VA1);
    glGenBuffers(1, &VB1);
    glBindVertexArray(VA1);

    glBindBuffer(GL_ARRAY_BUFFER, VB1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, vertices2, GL_STATIC_DRAW );

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // complie shaders
    meshShader = new Shader("shader.vs", "shader.frag");
    textShader = new Shader("text.vs", "text.frag");
    textureShader = new Shader("texture.vs", "texture.frag");
    projShader = new Shader("shader2.vs", "shader2.frag");

    reshape(width,height);

    font_.init(height);

    uboIndex = glGetUniformBlockIndex(meshShader->Program, "Material");

    glGetActiveUniformBlockiv(meshShader->Program, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);

    buffer = operator new(uboSize);

    const char* names[NumUniforms] = {"diffuse", "ambient", "specular", "emissive", "power"};

    glGetUniformIndices(meshShader->Program, NumUniforms, names, uboIndices);
    glGetActiveUniformsiv(meshShader->Program, NumUniforms, uboIndices, GL_UNIFORM_OFFSET, offset);
    glGetActiveUniformsiv(meshShader->Program, NumUniforms, uboIndices, GL_UNIFORM_SIZE, Size);
    glGetActiveUniformsiv(meshShader->Program, NumUniforms, uboIndices, GL_UNIFORM_TYPE, type);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_NORMALIZE);

    int err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout <<"Init: ERROR bitches\n";
        std::cout << gluErrorString(err);
    }
    
    Mesh* pMesh = assetManager_.loadObjMesh("porsche.obj");
    //Mesh* pMesh = assetManager_.loadObjMesh("cube.obj");
    obj.AttachMesh(pMesh);
    obj.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));

    std::vector<GLuint>& materials = pMesh->getDefaultMaterials();
    std::vector<unsigned int> objAtteributes;

    if (materials.size() != 0)
    {
        for (GLuint i : materials)
        {
            objAtteributes.push_back(assetManager_.getAttribute("", i, "shader"));
        }
    }
    else
    {
        Material matt;
        matt.ambient = glm::vec4(0.3f, 0.3f, 0.3f, 1.0);
        matt.diffuse = glm::vec4(0.3f, 0.3f, 0.3f, 1.0);
        matt.emissive = glm::vec4(0.3f, 0.3f, 0.3f, 1.0);
        matt.specular = glm::vec4(0.3f, 0.3f, 0.3f, 1.0);
        matt.power = 1.0f;
        GLuint i = assetManager_.getMaterialIndex(matt);
        objAtteributes.push_back(assetManager_.getAttribute("", i, "shader"));
    }

    obj.SetObjectAttributes(objAtteributes);

    glGenBuffers(1, &ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex, ubo);

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
void GameWin::drawing(Display* display, Window win)
{
    int err;
	   
    //clock.draw();
    glEnable(GL_CULL_FACE);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //TODO: Move this  to some where sane
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    meshShader->Use();
	glUniformMatrix4fv(glGetUniformLocation(meshShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	
	glm::vec4 testy(-0.5f,-0.5f, 0.0f, 1.0);
	
	testy = projection * testy;
	
	testy.x = 0.5f;
	testy.y = -0.5f;
	testy.z = 0;
	testy.w = 1.0f;
	testy = projection * testy;
	
	testy.x = 0.0f;
	testy.y = 0.5f;
	testy.z = 0.0;
	testy.w = 1.0f;
	testy = projection * testy;
	
	testy.x = 10.0f;
	testy.y = -10.0f;
	testy.z = -10.0;
	testy.w = 1.0f;
	testy = projection * testy;
	
	testy.x = 10.0f;
	testy.y = -10.0f;
	testy.z = 10.0f;
	testy.w = 1.0f;
	testy = projection * testy;
	
	testy.x = 10.0f;
	testy.y = 10.0f;
	testy.z = 10.0f;
	testy.w = 1.0f;
	testy = projection * testy;
	
	testy.x = 10.0f;
	testy.y = 10.0f;
	testy.z = -10.0f;
	testy.w = 1.0f;
	testy = projection * testy;
	
	projShader->Use();
	//projection = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(projShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	
	
    glBindVertexArray(VA0);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    //glUniformMatrix4fv(glGetUniformLocation(textShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//     glUniformMatrix4fv(glGetUniformLocation(meshShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
// 	
// 	
// 	projShader->Use();
//     glUniformMatrix4fv(glGetUniformLocation(projShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

//	textShader->Use();
	
//    std::stringstream ss;
//    ss << timer.getFPS();

//    font_.renderText(textShader, ss.str(),0.0f, 0.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
    //font_.renderText(textShader, "ajbcdefghijklomnpqwtyusxzv",40.0f, 50.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
    //font_.renderTextBatched(textShader, "ajbcdefghijklomnpqwtyusxzv",40.0f, 50.0f, 1.0f, glm::vec3(1.0f,0.0f,0.0f));
    //font_.renderText(textShader, "ajbcdefghijklomnpqwtyusxzv",40.0f, 200.0f, 2.0f, glm::vec3(0.0f,1.0f,0.0f));
    //font_.renderTextBatched(textShader, "ajbcdefghijklomnpqwtyusxzv",40.0f, 200.0f, 2.0f, glm::vec3(1.0f,0.0f,0.0f));
    //font_.renderTextBatched(textShader, "jj",0.0f, 400.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
    //font_.renderText(textShader, "wwwwwwwwwwwwwwwwwwwwwwwwww",0.0f, 550.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
    //font_.renderTextBatched(textShader, "wwwwwwwwwwwwwwwwwwwwwwwwww",0.0f, 526.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
    //font_.renderText(textShader, "awwwwwwwwwwwwww",0.0f, 550.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
    //font_.renderTextBatched(textShader, "awwwwwwwwwwwwww",0.0f, 526.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
//    font_.renderText(textShader, "this is so awesome thingee",0.0f, 550.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
//    font_.renderText(textShader, "I wanna be the very best L",0.0f, 500.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
//    font_.renderText(textShader, "gendlin gendlin gendlin nn",0.0f, 450.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
//    font_.renderText(textShader, "gendmon gendmon gendmon ge",0.0f, 400.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));
//    font_.renderText(textShader, "make me fucking pround tyu",0.0f, 350.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));

//    font_.renderText(textShader, "this is so awesome thingee",0.0f, 50.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f),height_);
//    font_.renderText(textShader, "I wanna be the very best L",0.0f, 100.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f),height_);
//    font_.renderText(textShader, "gendlin gendlin gendlin nn",0.0f, 150.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f),height_);
//    font_.renderText(textShader, "gendmon gendmon gendmon ge",0.0f, 200.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f),height_);
//    font_.renderText(textShader, "make me fucking proud bitc",0.0f, 250.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f),height_);


//    font_.renderTextBatched(textShader, "this ws so awesome thingee",0.0f, 526.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f), height_);
//    font_.renderTextBatched(textShader, "I wanna be the very best L",0.0f, 442.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f), height_);
//    font_.renderTextBatched(textShader, "gendlin gendlin gendlin nn",0.0f, 358.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f), height_);
//    font_.renderTextBatched(textShader, "gendmon gendmon gendmon ge",0.0f, 274.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f), height_);
//    font_.renderTextBatched(textShader, "make me fucking pround tyu",0.0f, 176.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f), height_);

    //font_.RenderText(ourShader, "woot!!!", 0.s0f, 0.0f, 1.0f, glm::vec3(0.3, 0.7f, 0.9f));

    textureShader->Use();
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("YorLogo.png"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("symbol.png"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("gold.png"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("square.bmp"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("king_white.bmp"));
    glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("negx.bmp"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("yor.bmp"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("pawn2.bmp"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("yor.png"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("space.jpg"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("T1.jpg"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("logo.jpg"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("CIVV.jpg"));

    //glBindTexture(GL_TEXTURE_2D, 120);
    glBindVertexArray(VA1);
    glBindBuffer(GL_ARRAY_BUFFER, VB1);
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, indices);

    std::vector<Vertex> vertices;
    std::vector<GLushort> indices;
    //std::vector<Texture> textures;
    
	Vertex vertex1(glm::vec3( 1,-1,-1),  glm::vec3(0,0,0), glm::vec2(0,0));
    //Vertex vertex1 = {glm::vec3( 1,-1,-1),  glm::vec3(0,0,0), glm::vec2(0,0)};
    Vertex vertex2 = {glm::vec3( 1,-1, 1),  glm::vec3(0,0,0), glm::vec2(1,0)};
    Vertex vertex3 = {glm::vec3( 1, 1, 1),  glm::vec3(0,0,0), glm::vec2(1,1)};
	
// 	Vertex vertex1 = {glm::vec3( -0.5f,-0.5f,0),  glm::vec3(0,0,0), glm::vec2(0,0)};
//     Vertex vertex2 = {glm::vec3( 0.5,-0.5, 0),  glm::vec3(0,0,0), glm::vec2(1,0)};
//     Vertex vertex3 = {glm::vec3( 0, 0.5, 0),  glm::vec3(0,0,0), glm::vec2(1,1)};


    Vertex vertex4 = {glm::vec3( 1, 1,-1),  glm::vec3(0,0,0), glm::vec2(0,1)};    
    Vertex vertex5 = {glm::vec3(-1,-1, 1),  glm::vec3(0,0,0), glm::vec2(0,0)};
    Vertex vertex6 = {glm::vec3(-1,-1,-1),  glm::vec3(0,0,0), glm::vec2(1,0)};
    Vertex vertex7 = {glm::vec3(-1, 1,-1),  glm::vec3(0,0,0), glm::vec2(1,1)};
    Vertex vertex8 = {glm::vec3(-1, 1, 1),  glm::vec3(0,0,0), glm::vec2(0,1)};
	
// 	Vertex vertex1 = {glm::vec3( 5,-5,-5),  glm::vec3(0,0,0), glm::vec2(0,0)};
//     Vertex vertex2 = {glm::vec3( 5,-5, 5),  glm::vec3(0,0,0), glm::vec2(1,0)};
//     Vertex vertex3 = {glm::vec3( 5, 5, 5),  glm::vec3(0,0,0), glm::vec2(1,1)};
//     Vertex vertex4 = {glm::vec3( 5, 5,-5),  glm::vec3(0,0,0), glm::vec2(0,1)};
//     
//     Vertex vertex5 = {glm::vec3(-5,-5, 5),  glm::vec3(0,0,0), glm::vec2(0,0)};
//     Vertex vertex6 = {glm::vec3(-5,-5,-5),  glm::vec3(0,0,0), glm::vec2(1,0)};
//     Vertex vertex7 = {glm::vec3(-5, 5,-5),  glm::vec3(0,0,0), glm::vec2(1,1)};
//     Vertex vertex8 = {glm::vec3(-5, 5, 5),  glm::vec3(0,0,0), glm::vec2(0,1)};
    
    vertices.push_back(Vertex(vertex1));
    vertices.push_back(Vertex(vertex2));
    vertices.push_back(Vertex(vertex3));
    vertices.push_back(Vertex(vertex4));
    vertices.push_back(Vertex(vertex5));
    vertices.push_back(Vertex(vertex6));
    vertices.push_back(Vertex(vertex7));
    vertices.push_back(Vertex(vertex8));
    
    // first squre
//     indices.push_back(0);
//     indices.push_back(1);
//     indices.push_back(2);
//     indices.push_back(0);
//     indices.push_back(3);
//     indices.push_back(2);
	
	indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
	indices.push_back(3);
    indices.push_back(0);
	
	indices.push_back(6);
	indices.push_back(5);
    indices.push_back(0);
    indices.push_back(1);
    
    // secound squre
//     indices.push_back(4);
//     indices.push_back(5);
//     indices.push_back(6);
    //indices.push_back(4);
    //indices.push_back(7);
    //indices.push_back(6);
    
    // third squre
// 	indices.push_back(1);
//     indices.push_back(5);
//     indices.push_back(3);
//     indices.push_back(1);
//     indices.push_back(4);
//     indices.push_back(5);
	
	
	// third squre
//     indices.push_back(6);
//     indices.push_back(3);
//     indices.push_back(2);
//     indices.push_back(6);
//     indices.push_back(7);
//     indices.push_back(2);
//     
//     // fourth squre
//     indices.push_back(4);
//     indices.push_back(1);
//     indices.push_back(0);
//     indices.push_back(4);
//     indices.push_back(5);
//     indices.push_back(0);
//     
//     // fifth squre
//     indices.push_back(5);
//     indices.push_back(0);
//     indices.push_back(3);
//     indices.push_back(5);
//     indices.push_back(6);
//     indices.push_back(3);
//     
//     // sixth squre
//     indices.push_back(1);
//     indices.push_back(4);
//     indices.push_back(7);
//     indices.push_back(1);
//     indices.push_back(2);
//     indices.push_back(7);
    
    //Texture T = {assetManager_.getTexture("negx.bmp"), "texture_diffuse" ,"negx.bmp"};
    
    //textures.push_back(T);
    
    //Mesh M(vertices, indices);
    
    auto attribVector = assetManager_.getAttributeVector();

    for (GLuint i = 0; i < attribVector.size(); i++)
    {
        Attribute& attrib = attribVector[i];
        SetAttribute(attrib);

        obj.Draw(meshShader, i, projection);
    }

    projShader->Use();
	
	glm::mat4 matt(1.0f);
	matt = glm::translate(matt, glm::vec3(10.0f, 10.0f, 10.0f));
	float yy = matt[3][2];
	
	//glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("negx.bmp"));
    
    //M.Draw();

    textShader->Use();

    std::stringstream ss;
    ss << timer.getFPS();

    font_.renderText(textShader, ss.str(),0.0f, 0.0f, 1.0f, glm::vec3(0.0f,1.0f,0.0f));

    
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout <<"Drawing: ERROR bitches " << err << "\n";
        std::cout << gluErrorString(err);
    }

    //glFlush();
    glXSwapBuffers (display, win);
}

//-----------------------------------------------------------------------------
// Name : reshape ()
//-----------------------------------------------------------------------------
void GameWin::reshape(int width, int height)
{
    float left,right,bottom,top;
    float AR;
	
    std::cout <<"reshape called\n";

    // define our ortho
    left=-1.5;
    right=1.5;
    top=1.5;
    bottom=-1.5;

    // 1)update viewport
    glViewport(0, 0, width, height);
    // 2) clear the transformation matrices (load identity)
    //glLoadIdentity();
    // 3) compute the aspect ratio
    AR = (width / (float)height);
	
    // 4) if AR>=1 update left, right
    if (AR >= 1)
    {
        left *= AR;
        right *= AR;
    }
    // 5) else i.e. AR<1 update the top, bottom
    else
    {
        top /= AR;
        bottom /= AR;
    }
	
    height_ = height;

    font_.setScreenHeight(height);

    // 6) defining the boundary of the model using gluOrtho2D
    textShader->Use();
    glUniform2i( glGetUniformLocation(textShader->Program, "screenSize"), width / 2, height / 2);
    textureShader->Use();
    glUniform2i( glGetUniformLocation(textureShader->Program, "screenSize"), width / 2, height / 2);
    //projection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
    float aspect = (float)width / (float)height;
    projection = glm::perspective(90.0f, aspect, 1.0f, 100.0f);
	//projection = glm::lookAt(glm::vec3(-10,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0)) * projection;
	//projection = projection * glm::lookAt(glm::vec3(0,0,50), glm::vec3(0,0,0), glm::vec3(0,1,0));
    //projection = projection * glm::lookAt(glm::vec3(-50,20,-70), glm::vec3(0,0,0), glm::vec3(0,1,0));
    projection = projection * glm::lookAt(glm::vec3(50,20,-70), glm::vec3(0,0,0), glm::vec3(0,1,0));
    //projection = projection * glm::lookAt(glm::vec3(-1,0,-1), glm::vec3(0,0,0), glm::vec3(0,1,0));
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
        while (XPending(display) > 0)
        {
            XNextEvent(display, &event);
            // quit event
            if ( (event.type == ClientMessage) && (event.xclient.data.l[0] == wmDeleteMessage) ) 
            {
                std::cout << "Shutting down now!!!" << std::endl;
                gameRunning = false;
                break;
            }
            if (event.type == KeyPress)
            {
                char buf[128];
                KeySym key;

                std::cout << "key pressed\n";
                std::cout << event.xkey.keycode << "\n";
                XLookupString(&event.xkey, buf, 128, &key, nullptr);
                std::cout << "KeySym: " << key << " " << "char: " <<  buf <<"\n";

                keysStatus[event.xkey.keycode] = true;
            }
            if (event.type == KeyRelease)
            {
                std::cout << "key released\n";
                std::cout << event.xkey.keycode << "\n";

                keysStatus[event.xkey.keycode] = false;
            }
            // window size was changed / need to be reRend!ered(always reRender anyway...)
            if (event.type == Expose)
            {
                XWindowAttributes gwa;

                XGetWindowAttributes(display, win, &gwa);
                reshape(gwa.width, gwa.height);
            }
        }
        
        timer.frameAdvanced();

        int err = glGetError();
        if (err != GL_NO_ERROR)
            std::cout <<"MsgLoop: ERROR bitches\n";

        if (!timer.isCap())
        {
            drawing(display, win);
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
    glDeleteVertexArrays(1, &VA0);
    glDeleteBuffers(1, &VB0);

    delete meshShader;
    delete textShader;

    glXMakeCurrent( display, 0, 0 );
    glXDestroyContext( display, ctx );

    XDestroyWindow( display, win );
    XFreeColormap( display, cmap );
    XCloseDisplay( display );
    
    return true;
}

//TODO: searching each time for pointers is slow
//      consider saving them with the attribute indexes
//-----------------------------------------------------------------------------
// Name : SetAttribute ()
//-----------------------------------------------------------------------------
void GameWin::SetAttribute(Attribute& attrib)
{
    // set the attributes that have changed
    // this check might be too cotly as this is in the render loop
    if (attrib.shaderIndex != curAttribute.shaderIndex)
    {
        Shader* pShader = assetManager_.getShader(attrib.shaderIndex);
        pShader->Use();
    }

    if (attrib.texIndex != curAttribute.texIndex)
    {
        glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture(attrib.texIndex));
    }

    if (attrib.matIndex != curAttribute.matIndex)
    {

        Material& mat = assetManager_.getMaterial(attrib.matIndex);

        GLint ii = offset[Diffuse];
        // copy the material to the buffer

        memcpy(buffer + offset[Diffuse], &mat.diffuse, Size[Diffuse] *
               TypeSize(type[Diffuse]));
        memcpy(buffer + offset[Ambient], &mat.ambient, Size[Ambient] *
               TypeSize(type[Ambient]));
        memcpy(buffer + offset[Specular], &mat.specular, Size[Specular] *
               TypeSize(type[Specular]));
        memcpy(buffer + offset[Emissive], &mat.emissive, Size[Emissive] *
               TypeSize(type[Emissive]));
        memcpy(buffer + offset[Power], &mat.power, Size[Power] *
               TypeSize(type[Power]));

        GLfloat* ff = (GLfloat*)buffer;

        for (int ii = 0; ii < uboSize/4; ii++ )
        {
            float x = ff[ii];
            x += 2;
        }

        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, uboSize, buffer, GL_DYNAMIC_DRAW);
        //glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex, ubo);
    }

}
