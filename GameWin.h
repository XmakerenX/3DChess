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
//#include "clock.h"
#include "AssetManager.h"
#include "Shader.h"
#include "Font.h"
#include "Mesh.h"
#include "Object.h"
#include "FreeCam.h"

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

#define GK_Z 52
#define GK_X 53

#define GK_Q 24
#define GK_E 26

#define GK_SPACE 35

struct Point
{
    int x;
    int y;
};

//struct LIGHT_PREFS
//{
//    LIGHT_PREFS()
//        :pos(0.0f, 0.0f, 0.0f),
//        dir(0.0f, 0.0f, 0.0f, 0.0f),
//        attenuation(0.0f, 0.0f, 0.0f),
//        ambient(0.0f, 0.0f, 0.0f, 0.0f),
//        diffuse(0.0f, 0.0f, 0.0f, 0.0f),
//        specular(0.0f, 0.0f, 0.0f, 0.0f),
//        spotPower(0.0f)
//    {}

//    LIGHT_PREFS(glm::vec3& newPos, glm::vec4& newDir, glm::vec3& newAtten, glm::vec4& newAmbient,
//                glm::vec4 newDiffuse, glm::vec4 newSpecular, float newSpecPower)
//        :pos(newPos),
//         dir(newDir),
//         attenuation(newAtten),
//         ambient(newAmbient),
//         diffuse(newDiffuse),
//         specular(newSpecular),
//         spotPower(newSpecPower)
//    {}

//    glm::vec3 pos;
//    glm::vec4 dir;
//    glm::vec3 attenuation;

//    glm::vec4 ambient;
//    glm::vec4 diffuse;
//    glm::vec4 specular;

//    float spotPower;

//};

struct LIGHT_PREFS
{
    LIGHT_PREFS()
        :pos(0.0f, 0.0f, 0.0f),
        dir(0.0f, 0.0f, 0.0f, 0.0f),
        attenuation(0.0f, 0.0f, 0.0f),
        ambient(0.0f, 0.0f, 0.0f, 0.0f),
        diffuse(0.0f, 0.0f, 0.0f, 0.0f),
        specular(0.0f, 0.0f, 0.0f, 0.0f),
        spotPower(0.0f)
    {}

    LIGHT_PREFS(glm::vec3& newPos, glm::vec4& newDir, glm::vec3& newAtten, glm::vec4& newAmbient,
                glm::vec4 newDiffuse, glm::vec4 newSpecular, float newSpecPower)
        :pos(newPos),
         dir(newDir),
         attenuation(newAtten),
         ambient(newAmbient),
         diffuse(newDiffuse),
         specular(newSpecular),
         spotPower(newSpecPower)
    {}

    glm::vec3 pos;
    float pad; //padding to fit the glsl struct

    glm::vec4 dir;
    glm::vec3 attenuation;
    float pad2; //padding to fit the glsl struct

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    float spotPower;
    glm::vec3 pad3; //padding to fit the glsl struct

};

enum {Diffuse, Ambient, Specular, Emissive, Power, NumUniforms};
enum {Position, Direction, Attenuation, LightAmbient, LightDiffuse, LightSpecular, SpotPower, NumUlights};

class GameWin
{
public:
    GameWin             ();
    virtual ~GameWin    ();
    
    bool initWindow     ();
    bool initOpenGL     (int width, int height);
    
    void drawing        (Display* display, Window win);
    void reshape        (int width, int height);
    
    void ProcessInput   (float timeDelta);

    int  BeginGame      ();
    bool Shutdown       ();
    
    void SetAttribute   (Attribute& attrib);

    static bool isExtensionSupported    (const char *extList, const char *extension);
    static int  ctxErrorHandler         (Display *dpy, XErrorEvent *ev );

    glm::vec4 shaderTest(glm::vec3 pos, glm::vec3 normal);
    
private:
    Display *display;
    Window win;
    
    GLuint m_winWidth;
    GLuint m_winHeight;

    Atom wmDeleteMessage;
    
    GLXContext ctx;
    Colormap cmap;
    
    Cursor emptyCursor;

    bool gameRunning;
    
    Timer timer;
    AssetManager assetManager_;

    int height_;

    static bool ctxErrorOccurred;
    
    //Clock clock;

    FreeCam m_camera;

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

    LIGHT_PREFS light;
    GLuint ublightIndex;
    GLint  ublightSize;
    GLuint ubLight;
    GLvoid * lightBuffer;
    GLuint ubLightIndices[NumUlights];
    GLint  lightSize[NumUlights];
    GLint  lightOffset[NumUlights];
    GLint  lightType[NumUlights];

    bool keysStatus[256];
    Point oldCursorLoc;
    bool mouseDrag;

    GLfloat vertices[18];

    GLfloat vertices2[4][4];
    GLushort indices[6];

    mkFont font_;

    Object obj;
    Object obj2;
    Object obj3;

    glm::mat4 projection;
};

#endif  //_GAMEWIN_H
