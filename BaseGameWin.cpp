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
	gameRunning = true;
	font_ = nullptr;

	for (int i = 0; i < 256; i++)
		keysStatus[i] = false;

	mouseDrag = false;

	m_scene = nullptr;
	m_sceneInput = true;

	spriteShader = nullptr;
	spriteTextShader = nullptr;
}

//-----------------------------------------------------------------------------
// Name : BaseGameWin (destructor)
//-----------------------------------------------------------------------------
BaseGameWin::~BaseGameWin()
{

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

	ProcessInput(timer.getTimeElapsed());

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_scene)
		m_scene->Drawing(timer.getTimeElapsed());

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
	renderFPS(m_sprites[1], *font_);
	font_->renderToRect(m_sprites[1], ss.str(), Rect(0, 65, 500, 200),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	if (m_scene)
		font_->renderToRect(m_sprites[1], m_scene->getStatus(), Rect(0, 130, 2000, 200), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	renderGUI();

	m_sprites[0].Render(spriteShader);
	m_sprites[0].Clear();
	m_sprites[1].Render(spriteTextShader);
	m_sprites[1].Clear();
	m_topSprites[0].Render(spriteShader);
	m_topSprites[0].Clear();
	m_topSprites[1].Render(spriteTextShader);
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
    font.renderToRect(textSprite, std::to_string(timer.getFPS()), 
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

    if (spriteShader != nullptr)
    {
        spriteShader->Use();
        glUniform2i(glGetUniformLocation(spriteShader->Program, "screenSize"), width / 2, height / 2);
    }

    if (spriteTextShader != nullptr)
    {
        spriteTextShader->Use();
        glUniform2i(glGetUniformLocation(spriteTextShader->Program, "screenSize"), width / 2, height / 2);
    }

    onSizeChanged();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput 
//-----------------------------------------------------------------------------
void BaseGameWin::ProcessInput(double timeDelta)
{
    float X = 0.0f, Y = 0.0f;
    if (mouseDrag)
    {
        Point currentCursorPos = getCursorPos();

        X = (float)(currentCursorPos.x - oldCursorLoc.x) / 3.0f;
        Y = (float)(currentCursorPos.y - oldCursorLoc.y) / 3.0f;

        setCursorPos(oldCursorLoc);
    }

    if (m_scene && m_sceneInput)
        m_scene->processInput(timeDelta, keysStatus, X, Y);
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
