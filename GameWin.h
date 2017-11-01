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

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

struct Point
{
    int x;
    int y;
};

struct LIGHT_PREFS
{
    LIGHT_PREFS()
        :pos(0.0f, 0.0f, 0.0f),
        dir(0.0f, 0.0f, 0.0f, 0.0f),
        attenuation(0.0f, 0.0f, 0.0f),
        ambient(0.0f, 0.0f, 0.0f, 0.0f),
        diffuse(0.0f, 0.0f, 0.0f, 0.0f),
        specular(0.0f, 0.0f, 0.0f, 0.0f),
        outerCutoff(0.0f)
    {}

    LIGHT_PREFS(glm::vec3& newPos, glm::vec4& newDir, glm::vec3& newAtten, glm::vec4& newAmbient,
                glm::vec4 newDiffuse, glm::vec4 newSpecular, float newSpecPower)
        :pos(newPos),
         dir(newDir),
         attenuation(newAtten),
         ambient(newAmbient),
         diffuse(newDiffuse),
         specular(newSpecular),
         outerCutoff(newSpecPower)
    {}

    glm::vec3 pos;
    float pad; //padding to fit the glsl struct

    glm::vec4 dir;
    glm::vec3 attenuation;
    float pad2; //padding to fit the glsl struct

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    float outerCutoff;
    float innerCutoff;
    glm::vec2 pad3; //padding to fit the glsl struct

};

class GameWin
{
public:
    GameWin             ();
    virtual ~GameWin    ();
    
    bool initWindow     ();
    bool initOpenGL     (int width, int height);
    
    void drawing        ();
    void reshape        (int width, int height);
    
    void ProcessInput   (float timeDelta);

    int  BeginGame      ();
    bool Shutdown       ();
    
    static bool isExtensionSupported    (const char *extList, const char *extension);
    static int  ctxErrorHandler         (Display *dpy, XErrorEvent *ev );
    
private:
    Display * m_display;
    Window m_win;
    
    GLuint m_winWidth;
    GLuint m_winHeight;

    Atom wmDeleteMessage;
    
    GLXContext ctx;
    Colormap cmap;
    
    Cursor emptyCursorPixmap;

    bool gameRunning;
    
    Timer timer;
    //AssetManager assetManager_;

    int height_;

    static bool ctxErrorOccurred;
    
    Scene m_scene;

    Shader* textShader;
    Shader* textureShader;
    Shader* projShader;

    bool keysStatus[256];
    Point oldCursorLoc;
    bool mouseDrag;

    mkFont font_;
};

#endif  //_GAMEWIN_H
