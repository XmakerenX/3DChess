#include "BaseGame.h"

//-----------------------------------------------------------------------------
// Name : BaseGame (constructor)
//-----------------------------------------------------------------------------
BaseGame::BaseGame()
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
// Name : BaseGame (destructor)
//-----------------------------------------------------------------------------
BaseGame::~BaseGame()
{

}

//-----------------------------------------------------------------------------
// Name : BeginGame 
//-----------------------------------------------------------------------------
int BaseGame::BeginGame()
{
    while (m_gameRunning && m_window->isRunning())
    {
        m_window->pumpMessages();
            
        m_timer.frameAdvanced();

        int err = glGetError();
        if (err != GL_NO_ERROR)
            std::cout <<"MsgLoop: ERROR bitches\n";

        if (!m_timer.isCap())
        {
            drawing();
        }
    }
    
    return 0;
}

//-----------------------------------------------------------------------------
// Name : Shutdown 
//-----------------------------------------------------------------------------
bool BaseGame::Shutdown()
{
    if (m_window)
    {
        bool ret = m_window->closeWindow();
        delete m_window;
        m_window = nullptr;
        return ret;
    }
    else
        return true;
}

//-----------------------------------------------------------------------------
// Name : initGame 
//-----------------------------------------------------------------------------
bool BaseGame::initGame(BaseWindow* window,int width, int height)
{
    std::cout << "InitGame started\n";

    if (window == nullptr)
        return false;
    
    m_window = window;
    m_window->setTimer(&m_timer);
    
    m_window->connectToSizeChangedEvent(boost::bind(&BaseGame::reshape, this, _1, _2));
    m_window->connectToKeyEvent(boost::bind(&BaseGame::sendKeyEvent, this, _1, _2));
    m_window->connectToVritaulKeyEvent(boost::bind(&BaseGame::sendVirtualKeyEvent, this, _1, _2, _3));
    m_window->connectToMouseEvent(boost::bind(&BaseGame::sendMouseEvent, this, _1, _2));
    
    if(!m_window->platformInit(width, height))
        return false;
    
    glewInit();
    
    m_spriteShader = m_asset.getShader("data/shaders/sprite");
    m_spriteTextShader = m_asset.getShader("data/shaders/spriteText");

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
    m_window->reshape(width,height);
    
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
void BaseGame::setRenderStates()
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
void BaseGame::drawing()
{
    int err;

    if (m_scene && m_sceneInput)
    {
        MouseDrift mouseDrift = m_window->processInput();
        m_scene->processInput(m_timer.getTimeElapsed(), m_window->getKeyStatus(), mouseDrift.x, mouseDrift.y);
    }

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

    m_window->glSwapBuffers();
}

//-----------------------------------------------------------------------------
// Name : renderFPS 
//-----------------------------------------------------------------------------
void BaseGame::renderFPS(Sprite& textSprite, mkFont& font)
{
    font.renderToRect(textSprite, std::to_string( m_timer.getFPS()), 
                      Rect(0, 0, 200, 60), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

//-----------------------------------------------------------------------------
// Name : reshape 
//-----------------------------------------------------------------------------
void BaseGame::reshape(int width, int height)
{
    if (m_scene)
        m_scene->reshape(width, height);
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
void BaseGame::ProcessInput(double timeDelta, bool keyStatus[256], float X, float Y)
{
    if (m_scene && m_sceneInput)
        m_scene->processInput(timeDelta, keyStatus, X, Y);
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent 
//-----------------------------------------------------------------------------
bool BaseGame::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : sendMouseEvent ()
//-----------------------------------------------------------------------------
void BaseGame::sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    handleMouseEvent(event, modifierStates);
    if (m_scene != nullptr && m_sceneInput)
        m_scene->handleMouseEvent(event, modifierStates);
}
