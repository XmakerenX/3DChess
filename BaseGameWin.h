#ifndef  _BASEGAMEWIN_H
#define  _BASEGAMEWIN_H

#include "AssetManager.h"

#include <iostream>
#include <sstream>

#include <GL/gl.h>
#include <GL/glew.h>

#include "timer.h"
#include "Scene.h"
#include "input.h"
#include "Sprite.h"

struct Resolution
{
    Resolution(GLuint _width, GLuint _height)
        :width(_width), height(_height)
    {}

    GLuint width;
    GLuint height;
};

std::ostream& operator<<(std::ostream& os, const Resolution res);
std::istream& operator>>(std::istream& is, Resolution res);

struct Mode1
{
    Mode1(GLuint _width, GLuint _height)
        :width(_width), height(_height)
    {}

    GLuint width;
    GLuint height;
};

class BaseGameWin
{
public:
    BaseGameWin();
    virtual ~BaseGameWin();

    virtual bool initWindow() = 0;
    virtual bool initOpenGL(int width, int height) = 0;

    void setRenderStates();

    void drawing();
    void renderFPS(Sprite& textSprite, mkFont& font);

    void reshape(int width, int height);
    void ProcessInput(double timeDelta);

    virtual int  BeginGame() = 0;
    virtual bool Shutdown() = 0;

    virtual void setFullScreenMode(bool fullscreen) = 0;
    virtual bool setMonitorResolution(int monitorIndex, Resolution newResolution) = 0;
    virtual void setWindowPosition(int x, int y) = 0;
    virtual void moveWindowToMonitor(int monitorIndex) = 0;

    virtual Point getCursorPos() = 0;
    virtual void setCursorPos(Point newPos) = 0;

    virtual Point getWindowPosition() = 0;
    virtual GLuint getWindowCurrentMonitor() = 0;

    virtual std::vector<std::vector<Mode1>> getMonitorsModes() const = 0;

protected:
    virtual void glSwapBuffers() = 0;
    virtual void getMonitorsInfo() = 0;

    virtual void initGUI() {};
    virtual void renderGUI() {};
    virtual bool handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual void sendKeyEvent(unsigned char key, bool down) {};
    virtual void sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates) {};
    virtual void sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual void onSizeChanged() {};

    GLuint m_winWidth;
    GLuint m_winHeight;

    bool m_gameRunning;

    Timer m_timer;

    Scene* m_scene;
    bool m_sceneInput;
    Object* selectedObj;

    bool m_keysStatus[256];
    Point m_oldCursorLoc;
    bool m_mouseDrag;

    AssetManager m_asset;
    mkFont* m_font;
    Sprite m_sprites[2];
    Sprite m_topSprites[2];

    Shader* m_spriteShader;
    Shader* m_spriteTextShader;
};

#endif  //_BASEGAMEWIN_H
