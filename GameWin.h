#ifndef  _GAMEWIN_H
#define  _GAMEWIN_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
//#include <GL/glcorearb.h>
#include<GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

#include "timer.h"
#include "clock.h"
#include "AssetManager.h"
#include "Shader.h"
#include "Font.h"
#include "Mesh.h"
#include "Object.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

//Keys scancodes
#define GK_UP    111
#define GK_DOWN  116
#define GK_LEFT  113
#define GK_RIGHT 114

#define GK_A 38
#define GK_S 39
#define GK_D 40
#define GK_W 25

#define GK_SPACE 35

enum {Diffuse, Ambient, Specular, Emissive, Power, NumUniforms};

class GameWin
{
public:
    GameWin             ();
    virtual ~GameWin    ();
    
    bool initWindow     ();
    bool initOpenGL     (int width, int height);
    
    void drawing        (Display* display, Window win);
    void reshape        (int width, int height);
    
    int  BeginGame      ();
    bool Shutdown       ();
    
    void SetAttribute   (Attribute& attrib);

    static bool isExtensionSupported    (const char *extList, const char *extension);
    static int  ctxErrorHandler         (Display *dpy, XErrorEvent *ev );
    
private:
    Display *display;
    Window win;
    
    Atom wmDeleteMessage;
    
    GLXContext ctx;
    Colormap cmap;
    
    bool gameRunning;
    
    Timer timer;
    AssetManager assetManager_;

    int height_;

    static bool ctxErrorOccurred;
    
    Clock clock;

    Attribute curAttribute;

    Shader* meshShader;
    Shader* textShader;
    Shader* textureShader;
    Shader* projShader;

    GLuint VB0, VA0;
    GLuint VB1, VA1;

    GLuint uboIndex;
    GLint  uboSize;
    GLuint ubo;
    GLvoid * buffer;
    GLuint uboIndices[NumUniforms];
    GLint Size[NumUniforms];
    GLint offset[NumUniforms];
    GLint type[NumUniforms];

    bool keysStatus[256];

    GLfloat vertices[18];

    GLfloat vertices2[4][4];
    GLushort indices[6];

    mkFont font_;

    Object obj;

    glm::mat4 projection;
};

#endif  //_GAMEWIN_H
