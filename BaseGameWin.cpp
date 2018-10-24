#include "BaseGameWin.h"

std::ostream& operator<<(std::ostream& os, const Resolution res)
{
    os << res.width << " " << res.height;
    return os;
}

std::istream& operator>>(std::istream& is, Resolution res)
{
    is >> res.width;
    is >> res.height;
    return is;
}

//-----------------------------------------------------------------------------
// Name : BaseGameWin (constructor)
//-----------------------------------------------------------------------------
BaseGameWin::BaseGameWin()
{
    m_gameRunning = true;
    m_font = nullptr;

    for (int i = 0; i < 256; i++)
        m_keysStatus[i] = false;

    m_mouseDrag = false;

    m_scene = nullptr;
    m_sceneInput = true;

    m_spriteShader = nullptr;
    m_spriteTextShader = nullptr;
}

//-----------------------------------------------------------------------------
// Name : BaseGameWin (destructor)
//-----------------------------------------------------------------------------
BaseGameWin::~BaseGameWin()
{

}

//-----------------------------------------------------------------------------
// Name : initGame 
//-----------------------------------------------------------------------------
bool BaseGameWin::initGame(int width, int height)
{
    std::cout << "InitGame started\n";
    
    if(!platformInit(width, height))
        return false;
    
    glewInit();
    
    m_spriteShader = m_asset.getShader("sprite");
    m_spriteTextShader = m_asset.getShader("spriteText");

    m_sprites[0].Init();
    m_sprites[1].Init();
    m_topSprites[0].Init();
    m_topSprites[1].Init();

    setRenderStates();
    
    initGUI();
    
    if (m_scene)
        m_scene->InitScene(width, height);
    
    // init our font
    m_font = m_asset.getFont("NotoMono", 40);
    // make sure the viewport is updated
    reshape(width,height);
    
    int err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout <<"Init: ERROR bitches\n";
        std::cout << gluErrorString(err) << "\n";
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// Name : setRenderStates 
//-----------------------------------------------------------------------------
void BaseGameWin::setRenderStates()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//-----------------------------------------------------------------------------
// Name : drawing 
//-----------------------------------------------------------------------------
void BaseGameWin::drawing()
{
    int err;

    ProcessInput( m_timer.getTimeElapsed());

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_scene)
        m_scene->Drawing( m_timer.getTimeElapsed());

    std::stringstream ss;
    if (m_scene != nullptr)
    {
        ss << m_scene->getMeshIndex() << " " << m_scene->getFaceCount() << " | ";
        int faceCount = m_scene->getFaceCount();
        int temp = (faceCount / 8);
        int square = (faceCount / 2) - (((faceCount / 2) / 4) * 4);
        if ((m_scene->getMeshIndex() == 0 && temp % 2 == 0) ||
            (m_scene->getMeshIndex() == 1 && temp % 2 != 0))
        {
            ss << square * 2;
            square = square * 2;
        }
        else
        {
            ss << square * 2 + 1;
            square = square * 2 + 1;
        }
        
        ss << " " << temp << " | " << temp << " " << 7 - square;
    }

    glDisable(GL_DEPTH_TEST);
    renderFPS(m_sprites[1], *m_font );
    m_font->renderToRect(m_sprites[1], ss.str(), Rect(0, 65, 500, 200),
                        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    if (m_scene)
        m_font->renderToRect(m_sprites[1], m_scene->getStatus(), Rect(0, 130, 2000, 200), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    renderGUI();

    m_sprites[0].Render( m_spriteShader );
    m_sprites[0].Clear();
    m_sprites[1].Render( m_spriteTextShader );
    m_sprites[1].Clear();
    m_topSprites[0].Render( m_spriteShader );
    m_topSprites[0].Clear();
    m_topSprites[1].Render( m_spriteTextShader );
    m_topSprites[1].Clear();

    glEnable(GL_DEPTH_TEST);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Drawing: ERROR bitches " << err << " :";
        std::cout << gluErrorString(err) << "\n";
    }

    glSwapBuffers();
}

//-----------------------------------------------------------------------------
// Name : renderFPS 
//-----------------------------------------------------------------------------
void BaseGameWin::renderFPS(Sprite& textSprite, mkFont& font)
{
    font.renderToRect(textSprite, std::to_string( m_timer.getFPS()), 
                      Rect(0, 0, 200, 60), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

//-----------------------------------------------------------------------------
// Name : reshape 
//-----------------------------------------------------------------------------
void BaseGameWin::reshape(int width, int height)
{
    std::cout << "reshape called\n";
    std::cout << "New window size is " << width << "X" << height << "\n";
    m_winWidth = width;
    m_winHeight = height;

    if (m_scene)
        m_scene->reshape(m_winWidth, m_winHeight);
    else
        glViewport(0, 0, width, height);

    if ( m_spriteShader != nullptr)
    {
        m_spriteShader->Use();
        glUniform2i(glGetUniformLocation( m_spriteShader->Program, "screenSize"), width / 2, height / 2);
    }

    if ( m_spriteTextShader != nullptr)
    {
        m_spriteTextShader->Use();
        glUniform2i(glGetUniformLocation( m_spriteTextShader->Program, "screenSize"), width / 2, height / 2);
    }

    onSizeChanged();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput 
//-----------------------------------------------------------------------------
void BaseGameWin::ProcessInput(double timeDelta)
{
    float X = 0.0f, Y = 0.0f;
    if ( m_mouseDrag )
    {
        Point currentCursorPos = getCursorPos();

        X = (float)(currentCursorPos.x - m_oldCursorLoc.x) / 3.0f;
        Y = (float)(currentCursorPos.y - m_oldCursorLoc.y) / 3.0f;

        setCursorPos( m_oldCursorLoc );
    }

    if (m_scene && m_sceneInput)
        m_scene->processInput(timeDelta, m_keysStatus, X, Y);
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent 
//-----------------------------------------------------------------------------
bool BaseGameWin::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
	return false;
}

//-----------------------------------------------------------------------------
// Name : sendMouseEvent ()
//-----------------------------------------------------------------------------
void BaseGameWin::sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
	handleMouseEvent(event, modifierStates);
	if (m_scene != nullptr && m_sceneInput)
		m_scene->handleMouseEvent(event, modifierStates);
}
