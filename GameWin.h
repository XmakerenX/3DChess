#ifndef  _GAMEWIN_H
#define  _GAMEWIN_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
//#include <GL/glcorearb.h>
#include<GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <iostream>
#include "timer.h"
#include "clock.h"
#include "Shader.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

class GameWin
{
public:
    GameWin           ();
    virtual ~GameWin();
    
    bool initWindow ();
    bool initOpenGL (int width, int height);
    
    void drawing(Display* display, Window win);
    void reshape(int width, int height);
    
    int  BeginGame  ();
    bool Shutdown   ();
    
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
    
    static bool ctxErrorOccurred;
    
    Clock clock;

    Shader* ourShader;
    GLuint VB0, VA0;
    GLfloat vertices[18];
};

#endif  //_GAMEWIN_H
