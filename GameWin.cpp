#include "GameWin.h"
#include <GL/glut.h>
#include <unistd.h>

//const char* meshShaderPath = "shader";
//const char* meshShaderPath = "shader3";
//const char* meshShaderPath = "objectShader";
const char* meshShaderPath = "objectShader2";

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

glm::vec3 myReflect(glm::vec3 i, glm::vec3 n)
{
    //return (i - (2 * n * glm::dot(i,n)));
    return (i - (2 * glm::dot(i,n)) * n);
}

glm::vec4 GameWin::shaderTest(glm::vec3 pos, glm::vec3 normal)
{
    glm::vec4 temp = glm::inverse(obj2.GetWorldMatrix()) * glm::vec4(normal, 0.0f);
    glm::vec3 normalW = glm::vec3(temp.x, temp.y, temp.z);

    glm::vec4 posW = obj2.GetWorldMatrix() * glm::vec4(pos, 1.0);

    glm::vec4 outColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    glm::vec3 toEye = glm::normalize(m_camera.GetPosition() - glm::vec3(posW.x, posW.y, posW.z));

    {
        Material& mat = assetManager_.getMaterial(curAttribute.matIndex);
        //glm::vec4 temp4 = glm::cross(mat.ambient, light.ambient);
        glm::vec4 temp5 = mat.ambient * light.ambient;
        glm::vec3 lightAmbient = glm::vec3(temp5.x, temp5.y, temp5. z);
        //glm::vec3 lightAmbient = glm::vec3(temp4.x, temp4.y, temp4. z);

        float s;
        glm::vec3 r;
        float A;
        float spot;

        if (light.pos.x != 0 || light.pos.y || light.pos.z)
        {
            glm::vec3 lightVecW = glm::normalize(light.pos -glm::vec3(posW.x, posW.y, posW.z));

            r = myReflect(-lightVecW, normalW);
            s = std::max( glm::dot(normalW, lightVecW), 0.0f);

            float d = glm::distance(light.pos, glm::vec3(posW.x, posW.y, posW.z));
            d = d / 4;

            A = light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d;

            if (light.spotPower != 0)
                spot = std::pow( std::max( glm::dot(glm::vec4(-lightVecW, 0.0f), light.dir), 0.0f), light.spotPower);
            else
                spot = 1;
        }
        else
        {
            glm::vec3 ddd = glm::vec3 (light.dir.x, light.dir.y, light.dir.z);
            r = myReflect(-ddd, normalW);
            s = std::max( glm::dot( light.dir, glm::vec4(normalW, 0.0)), 0.0f);
            A = 1;
            spot = 1;
        }

        glm::vec4 temp2 = s * (mat.diffuse * light.diffuse);
        glm::vec3 diffuseLight = glm::vec3(temp2.x, temp2.y, temp2.z);

        float t = std::pow( std::max( glm::dot(r, toEye), 0.0f), mat.power);
        glm::vec4 temp3 = t * (mat.specular * light.specular);
        glm::vec3 spec = glm::vec3(temp3.x, temp3.y, temp.z);

        diffuseLight = (diffuseLight + spec) / A;
        lightAmbient = lightAmbient + diffuseLight;
        glm::vec3 color = spot * lightAmbient;

        glm::vec4 lightColor = glm::vec4(color, mat.diffuse.a);

        outColor = lightColor;
    }

    return outColor;
}


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

    mouseDrag = false;

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
    swa.event_mask        = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask |
                            ButtonPressMask | ButtonReleaseMask;
    
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
    
    //XSelectInput(display, win, KeyPressMask | KeyReleaseMask);

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

    //------------------------------------
    // empty cursor init
    //------------------------------------
    XColor color = { 0 };
    const char data[] = { 0 };

    Pixmap pixmap = XCreateBitmapFromData(display, win, data, 1,1);
    emptyCursor = XCreatePixmapCursor(display, pixmap, pixmap, &color, &color, 0, 0);

    XFreePixmap(display,pixmap);

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

    //------------------------------------
    // Shader loading
    //------------------------------------
    // complie shaders meshShaderPath
    //meshShader = assetManager_.getShader("shader");
    meshShader = assetManager_.getShader(meshShaderPath);
    //meshShader = assetManager_.getShader("ss");
    //meshShader = assetManager_.getShader("objectShader");
    //meshShader = new Shader("shader.vs", "shader.frag");
    textShader = new Shader("text.vs", "text.frag");
    textureShader = new Shader("texture.vs", "texture.frag");
    projShader = new Shader("shader2.vs", "shader2.frag");

    //------------------------------------
    // Material unifrom buffer init
    //------------------------------------
    uboIndex = glGetUniformBlockIndex(meshShader->Program, "Material");

    glGetActiveUniformBlockiv(meshShader->Program, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);

    buffer = operator new(uboSize);

    //const char* names[NumUniforms] = {"diffuse", "ambient", "specular", "emissive", "power"};
    const char* names[NumUniforms] = {"mDiffuse", "mAmbient", "mSpecular", "mEmissive", "mPower"};

    glGetUniformIndices(meshShader->Program, NumUniforms, names, uboIndices);
    glGetActiveUniformsiv(meshShader->Program, NumUniforms, uboIndices, GL_UNIFORM_OFFSET, offset);
    glGetActiveUniformsiv(meshShader->Program, NumUniforms, uboIndices, GL_UNIFORM_SIZE, Size);
    glGetActiveUniformsiv(meshShader->Program, NumUniforms, uboIndices, GL_UNIFORM_TYPE, type);

//    err = glGetError();
//    if (err != GL_NO_ERROR)
//    {
//        std::cout <<"Init: ERROR bitches\n";
//        std::cout << gluErrorString(err);
//    }

    glGenBuffers(1, &ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex, ubo);
    // no idea why 1 is the correct number...
    glUniformBlockBinding(meshShader->Program, uboIndex, uboIndex);

    //------------------------------------
    // Mesh creation
    //------------------------------------
    Mesh* pMesh = assetManager_.loadObjMesh("porsche.obj");
    pMesh->CalcVertexNormals(90.0f);
    //Mesh* pMesh = assetManager_.loadObjMesh("bah2.obj");
    //Mesh* pMesh = assetManager_.loadObjMesh("cube.obj");
    obj.AttachMesh(pMesh);
    obj.SetPos(glm::vec3(1.0f, 1.0f, 1.0f));
    obj.SetScale(glm::vec3(0.5f,0.5f,0.5f));

    std::vector<GLuint>& materials = pMesh->getDefaultMaterials();
    std::vector<unsigned int> objAtteributes;

    if (materials.size() != 0)
    {
        for (GLuint i : materials)
        {
            //objAtteributes.push_back(assetManager_.getAttribute("", i, "shader"));
            objAtteributes.push_back(assetManager_.getAttribute("", i, meshShaderPath));
        }
    }
    else
    {
        Material matt;
        matt.ambient = glm::vec4(0.0f, 0.3f, 0.0f, 1.0);
        matt.diffuse = glm::vec4(0.0f, 0.3f, 0.0f, 1.0);
        matt.emissive = glm::vec4(0.0f, 0.3f, 0.0f, 1.0);
        matt.specular = glm::vec4(0.0f, 0.3f, 0.0f, 1.0);
        matt.power = 1.0f;
        GLuint i = assetManager_.getMaterialIndex(matt);
        //objAtteributes.push_back(assetManager_.getAttribute("", i, "shader"));
        objAtteributes.push_back(assetManager_.getAttribute("", i, meshShaderPath));
    }

    obj.SetObjectAttributes(objAtteributes);

    pMesh = assetManager_.loadObjMesh("cube.obj");
    obj2.AttachMesh(pMesh);
    //obj2.SetPos(glm::vec3(0.0f, 30.0f, 0.0f));
    obj2.SetPos(glm::vec3(18.5f, -0.5f, 0.0f));
    obj2.SetScale(glm::vec3(1.0f,1.0f,1.0f));

    std::vector<GLuint>& materials2 = pMesh->getDefaultMaterials();
    std::vector<unsigned int> objAtteributes2;

    if (materials2.size() != 0)
    {
        for (GLuint i : materials2)
        {
            //objAtteributes2.push_back(assetManager_.getAttribute("", i, "shader"));
            objAtteributes2.push_back(assetManager_.getAttribute("", i, meshShaderPath));
        }
    }
    else
    {
        Material matt;
        matt.ambient = glm::vec4(0.0f, 0.3f, 0.0f, 1.0);
        matt.diffuse = glm::vec4(0.0f, 0.3f, 0.0f, 1.0);
        matt.emissive = glm::vec4(0.0f, 0.3f, 0.0f, 1.0);
        matt.specular = glm::vec4(0.0f, 0.3f, 0.0f, 1.0);
        matt.power = 1.0f;
        GLuint i = assetManager_.getMaterialIndex(matt);
        //objAtteributes2.push_back(assetManager_.getAttribute("", i, "shader"));
        objAtteributes2.push_back(assetManager_.getAttribute("", i, meshShaderPath));
    }

    obj2.SetObjectAttributes(objAtteributes2);

    pMesh = assetManager_.loadObjMesh("cube.obj");
    obj3.AttachMesh(pMesh);
    //obj2.SetPos(glm::vec3(0.0f, 30.0f, 0.0f));
    obj3.SetPos(glm::vec3(20.6f, 1.5f, 0.0f));
    obj3.SetScale(glm::vec3(0.6f,0.6f,0.6f));

    std::vector<GLuint>& materials3 = pMesh->getDefaultMaterials();
    std::vector<unsigned int> objAtteributes3;

    if (materials3.size() != 0)
    {
        for (GLuint i : materials3)
        {
            //objAtteributes3.push_back(assetManager_.getAttribute("", i, "shader"));
            objAtteributes3.push_back(assetManager_.getAttribute("", i, meshShaderPath));
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
        //objAtteributes3.push_back(assetManager_.getAttribute("", i, "shader"));
        objAtteributes3.push_back(assetManager_.getAttribute("", i, meshShaderPath));
    }

    obj3.SetObjectAttributes(objAtteributes2);


    //------------------------------------
    // Camera  Init
    //------------------------------------
    m_camera.SetFOV(60.0f);
    m_camera.SetViewPort(0.0f, 0.0f, width, height, 1.0f, 1000.0f);
    m_camera.SetPostion(glm::vec3(0, 20, -70));
    m_camera.SetLookAt(glm::vec3(0.0f, 0.0f, 0.0f));

    //------------------------------------
    // lights  Init
    //------------------------------------
    //light.dir = glm::vec4(1.0f, 0.8f, 0.4f, 0.0f);
    light.dir = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
    //light.dir = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    light.ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f) * 0.1f;
    //light.ambient = glm::vec4(0.4f, 0.4f, 0.4f, 0.4f); //* 0.4f;
    //light.diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    light.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    light.specular = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    //light.pos = glm::vec3(0.0f, 30.0f, 0.0f);
    //light.spotPower = 2;
    //light.attenuation = glm::vec3(1.0f, 1.0f, 1.0f);

    //light.dir = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
    //light.dir = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    //light.ambient = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) * 0.1f;
    //light.diffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    //light.specular = glm::vec4(0.3f, 0.0f, 0.0f, 1.0f);
    light.pos = glm::vec3 (1.6f, 20.0f, 0.0f);
    //light.pos = glm::vec3 (25.0f, 1.0f, 0.0f);
    light.attenuation = glm::vec3(1.0f, 0.007f, 0.002f);
    light.spotPower = 0;

    ublightIndex = glGetUniformBlockIndex(meshShader->Program, "lightBlock");

    glGetActiveUniformBlockiv(meshShader->Program, ublightIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &ublightSize);

    glGenBuffers(1, &ubLight);
    glBindBufferBase(GL_UNIFORM_BUFFER, ublightIndex, ubLight);
    glUniformBlockBinding(meshShader->Program, ublightIndex, ublightIndex);

    //glBufferData(GL_UNIFORM_BUFFER, ublightSize, lightBuffer, GL_STATIC_DRAW);
    //glBufferData(GL_UNIFORM_BUFFER, sizeof(LIGHT_PREFS), lightBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, ubLight);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LIGHT_PREFS), &light, GL_STATIC_DRAW);

    //glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex, ubo);

    meshShader->Use();
    glUniform1i(glGetUniformLocation(meshShader->Program, "nActiveLights"), 1);

    //glUniform1i(glGetUniformLocation(meshShader->Program, "nActiveLights"), 1);


    // make sure the viewport is updated
    reshape(width,height);
    // init our font
    font_.init(height);

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
void GameWin::drawing(Display* display, Window win)
{
    int err;

    curAttribute.matIndex = -1;
    curAttribute.shaderIndex = "";
    curAttribute.texIndex = "";

    ProcessInput(timer.getTimeElapsed());

    meshShader->Use();
    glm::vec3 eye = m_camera.GetPosition();
    glUniform3f(glGetUniformLocation(meshShader->Program, "vecEye"), eye.x, eye.y, eye.z);

    //clock.draw();
    glEnable(GL_CULL_FACE);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //TODO: Move this  to some where sane
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //meshShader->Use();
    //glUniformMatrix4fv(glGetUniformLocation(meshShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	
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
    //glUniformMatrix4fv(glGetUniformLocation(projShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	
	
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

    //textureShader->Use();

    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("YorLogo.png"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("symbol.png"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("gold.png"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("square.bmp"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("king_white.bmp"));

    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("negx.bmp"));

    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("yor.bmp"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("pawn2.bmp"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("yor.png"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("space.jpg"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("T1.jpg"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("logo.jpg"));
    //glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("CIVV.jpg"));

    //glBindTexture(GL_TEXTURE_2D, 120);

    //glBindVertexArray(VA1);
    //glBindBuffer(GL_ARRAY_BUFFER, VB1);
    //glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, indices);

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
        {
        Attribute& attrib = attribVector[i];
        SetAttribute(attrib);

        //glm::mat4x4 matt = m_camera.GetProjMatrix() * m_camera.GetViewMatrix();
//        glm::mat4x4 temp = m_camera.GetViewMatrix() * m_camera.GetProjMatrix();
//        glm::mat4x4 temp2 = m_camera.GetProjMatrix() * m_camera.GetViewMatrix();
        //obj.Draw(meshShader, i, m_camera.GetViewMatrix() * m_camera.GetProjMatrix());
        obj.Draw(meshShader, i, m_camera.GetProjMatrix() * m_camera.GetViewMatrix());
        obj2.Draw(meshShader, i, m_camera.GetProjMatrix() * m_camera.GetViewMatrix());
        obj3.Draw(meshShader, i, m_camera.GetProjMatrix() * m_camera.GetViewMatrix());

        //glm::vec4 colorVec = shaderTest(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        if (i == 5)
        {
            glm::vec4 colorVec = shaderTest(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            //glm::vec4 colorVec = shaderTest(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            //glm::vec4 colorVec = shaderTest(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            colorVec.x++;
        }
        //obj.Draw(meshShader, i, projection);
        }
    }

//    projShader->Use();
	
//	glm::mat4 matt(1.0f);
//	matt = glm::translate(matt, glm::vec3(10.0f, 10.0f, 10.0f));
//	float yy = matt[3][2];
	
	//glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture("negx.bmp"));
    
    //M.Draw();

    textShader->Use();

    std::stringstream ss;
    ss << timer.getFPS();
    ss << " ";
    glm::vec3 obj2Pos = obj2.GetPosition();
    ss << obj2Pos.x;
    ss << " ";
    ss << obj2Pos.y;
    ss << " ";
    ss << obj2Pos.z;


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
	
    if(m_winWidth != width || m_winHeight != height)
    {
        std::cout <<"reshape called\n";
        m_winWidth = width;
        m_winHeight = height;

        // define our ortho
        left=-1.5;
        right=1.5;
        top=1.5;
        bottom=-1.5;

        // 1)update viewport
        //glViewport(0, 0, width, height);
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

        m_camera.SetViewPort(0.0f, 0.0f, m_winWidth, m_winHeight, 1.0f, 1000.0f);

        // 6) defining the boundary of the model using gluOrtho2D
        textShader->Use();
        glUniform2i( glGetUniformLocation(textShader->Program, "screenSize"), width / 2, height / 2);
        textureShader->Use();
        glUniform2i( glGetUniformLocation(textureShader->Program, "screenSize"), width / 2, height / 2);
        //projection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
        float aspect = (float)width / (float)height;
        projection = glm::perspective(2.2f, aspect, 1.0f, 100.0f);
        //projection = glm::lookAt(glm::vec3(-10,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0)) * projection;
        //projection = projection * glm::lookAt(glm::vec3(0,0,50), glm::vec3(0,0,0), glm::vec3(0,1,0));
        //projection = projection * glm::lookAt(glm::vec3(-50,20,-70), glm::vec3(0,0,0), glm::vec3(0,1,0));
        projection = projection * glm::lookAt(glm::vec3(50,20,-70), glm::vec3(0,0,0), glm::vec3(0,1,0));
        //projection = projection * glm::lookAt(glm::vec3(-1,0,-1), glm::vec3(0,0,0), glm::vec3(0,1,0));
    }
    else
        std::cout <<"reshape ignored\n";
}

//-----------------------------------------------------------------------------
// Name : ProcessInput ()
//-----------------------------------------------------------------------------
void GameWin::ProcessInput(float timeDelta)
{
    GLuint direction = 0;

    if (keysStatus[GK_UP])
    {
        direction |= Camera::DIR_FORWARD;
    }

    if (keysStatus[GK_DOWN])
    {
        direction |= Camera::DIR_BACKWARD;
    }

    if (keysStatus[GK_LEFT])
    {
        direction |= Camera::DIR_LEFT;
    }

    if (keysStatus[GK_RIGHT])
    {
        direction |= Camera::DIR_RIGHT;
    }

    m_camera.Move(direction, 10* timeDelta);

    float x = 0,y = 0,z = 0;
    if (keysStatus[GK_D])
    {
        //x += 0.001f;
        x -= 10.0f;
    }

    if (keysStatus[GK_A])
    {
        //x -= 0.001f;
        x += 10.0f;
    }

    if (keysStatus[GK_W])
    {
        //y += 0.001f;
        y += 10.0f;
    }

    if (keysStatus[GK_S])
    {
        //y -= 0.001f;
        y -= 10.0f;
    }

    if (keysStatus[GK_Z])
    {
        //z += 0.001f;
        z += 10.0f;
    }

    if (keysStatus[GK_X])
    {
        //z -= 0.001f;
        z -= 10.0f;
    }

    //obj.TranslatePos(x * timeDelta,y * timeDelta,z * timeDelta);
    obj2.TranslatePos(x * timeDelta,y * timeDelta,z * timeDelta);

    if (keysStatus[GK_Q])
    {
        obj.Rotate(-(glm::pi<float>() / 4) * timeDelta, 0.0f, 0.0f);
    }

    if (keysStatus[GK_E])
    {
        obj.Rotate(glm::pi<float>() / 4 * timeDelta, 0.0f, 0.0f);
    }


    if (mouseDrag)
    {
        int cursorX;
        int cursorY;
        float X = 0.0f,Y = 0.0f;
        bool ret;

        Window root,child;
        int x,y;
        unsigned int maskRet;


        ret = XQueryPointer(display, win, &root, &child ,&x, &y,
                            &cursorX, &cursorY, &maskRet);

        std::cout <<"process input:\n";
        std::cout << "x = " << cursorX << " y = " << cursorY << "\n";

        X = (float)(cursorX - oldCursorLoc.x) / 3.0f;
        Y = (float)(cursorY - oldCursorLoc.y) / 3.0f;

        if (X != 0.0f || Y != 0.0f)
            if (X || Y)
                m_camera.Rotate(-Y, -X, 0.0f);

        XWarpPointer(display, None, win, 0, 0, 0, 0, oldCursorLoc.x, oldCursorLoc.y);
        XFlush(display);
    }


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
        while (XPending(display) > 0 && gameRunning)
        {
            XNextEvent(display, &event);

            switch(event.type)
            {

            case Expose:
            {
                XWindowAttributes gwa;

                XGetWindowAttributes(display, win, &gwa);
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
                    int ret = XDefineCursor(display, win, emptyCursor);
                    ret++;

//                    std::cout <<"loop:\n";
//                    std::cout << "x = " << event.xbutton.x << " y = " << event.xbutton.y << "\n";

                }

                if (event.xbutton.button == Button3)
                    std::cout << "right button pressed\n";

            }break;

            case ButtonRelease:
            {
                if (event.xbutton.button == Button1)
                {
                    std::cout << "left button released\n";
                    mouseDrag = false;
                    XUndefineCursor(display, win);
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

    XFreeCursor(display, emptyCursor);
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
        //glUniform1i(glGetUniformLocation(pShader->Program, "nActiveLights"), 1);
    }

    if (attrib.texIndex != curAttribute.texIndex)
    {
        glBindTexture(GL_TEXTURE_2D, assetManager_.getTexture(attrib.texIndex));
    }

    if (attrib.matIndex != curAttribute.matIndex)
    {

        Material& mat = assetManager_.getMaterial(attrib.matIndex);

        // copy the material to the buffer
//        memcpy(buffer + offset[Diffuse], &mat.diffuse, Size[Diffuse] *
//               TypeSize(type[Diffuse]));
//        memcpy(buffer + offset[Ambient], &mat.ambient, Size[Ambient] *
//               TypeSize(type[Ambient]));
//        memcpy(buffer + offset[Specular], &mat.specular, Size[Specular] *
//               TypeSize(type[Specular]));
//        memcpy(buffer + offset[Emissive], &mat.emissive, Size[Emissive] *
//               TypeSize(type[Emissive]));
//        memcpy(buffer + offset[Power], &mat.power, Size[Power] *
//               TypeSize(type[Power]));


        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, uboSize, &mat, GL_DYNAMIC_DRAW);
        //glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex, ubo);
        curAttribute = attrib;
    }

}
