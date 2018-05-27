#ifndef  _GAMEWIN_H
#define  _GAMEWIN_H

// must be included first becuase of conflict between x11 fbxsdk.h
#include "AssetManager.h"

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
#include <X11/extensions/Xrandr.h>

#include <sstream>

#include "timer.h"
#include "Shader.h"
#include "Font.h"
#include "Mesh.h"
#include "Object.h"
#include "Scene.h"
#include "FreeCam.h"
#include "input.h"
#include "Scene.h"
#include "Sprite.h"
#include "GUI/DialogUI.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

class GameWin
{
public:
    GameWin             ();
    virtual ~GameWin    ();
    
    bool initWindow     ();
    bool initOpenGL     (int width, int height);
    
    void drawing        ();
    void reshape        (int width, int height);
    
    void ProcessInput   (double timeDelta);

    int  BeginGame      ();
    bool Shutdown       ();
    
    static bool isExtensionSupported    (const char *extList, const char *extension);
    static int  ctxErrorHandler         (Display *dpy, XErrorEvent *ev );
    
private:
    Display * m_display;
    Window m_win;
    
    GLuint m_winWidth;
    GLuint m_winHeight;

    int faceCount;
    int meshIndex;
    bool hit;

    Object* selectedObj;

    Atom wmDeleteMessage;
    
    GLXContext ctx;
    Colormap cmap;
    
    Cursor emptyCursorPixmap;

    bool gameRunning;
    
    Timer timer;
    //AssetManager assetManager_;

    int height_;
    float m_hDpi;
    float m_vDpi;

    static bool ctxErrorOccurred;
    
    Scene m_scene;

    Shader* textShader;
    Shader* textureShader;
    Shader* spriteShader;
    Shader* spriteTextShader;
    //Shader* projShader;

    bool keysStatus[256];
    Point oldCursorLoc;
    bool mouseDrag;

    AssetManager m_asset;
    mkFont* font_;
    Sprite m_sprite;
    Sprite m_textSprite;
    DialogUI m_dialog;

    //crap to delete
    GLuint VAO;
    GLuint VBO;
    VertexIndex indices[6];

};

#endif  //_GAMEWIN_H
