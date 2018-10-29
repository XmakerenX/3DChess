#include "ChessScene.h" 

//-----------------------------------------------------------------------------
// Name : ChessScene
//-----------------------------------------------------------------------------
ChessScene::ChessScene(DialogUI& promotionDialog) : m_promotionDialog(promotionDialog)
{
    m_rotationMode = RotationMode::Infinite;
    m_rotationAngle = 0;
    m_endAngle = 0;
    m_cameraRotation = true;
    
    boardObject = nullptr;
    frameSquareObject = nullptr;
    m_blackAttribute = -1;
    m_blueAttribute = -1;
    m_redAttribute = -1;
    m_yellowAttribute = -1;
    m_lastIndex = 0;
    
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
            pieceObjects[i][j] = -1;
    }
}

//-----------------------------------------------------------------------------
// Name : ChessScene
//-----------------------------------------------------------------------------
ChessScene::~ChessScene()
{
    if(gameBoard != nullptr)
        gameBoard->SaveBoardToFile();
}

//-----------------------------------------------------------------------------
// Name : InitScene
//-----------------------------------------------------------------------------
void ChessScene::InitScene(int width, int height, const glm::vec3& cameraPos, const glm::vec3& cameraLookat)
{
    Scene::InitScene(width, height, glm::vec3(-40, 54.5, 40.0), glm::vec3(40.0f, 0.0f, 40.0f));
}

//-----------------------------------------------------------------------------
// Name : InitObjects
//-----------------------------------------------------------------------------
void ChessScene::InitObjects()
{
    // add board object
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(0.0f, 0.0f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(1.0f, 1.0f, 1.0f), // scale
                           m_assetManager.getMesh("board.gen"),
                           s_meshShaderPath2 );
    m_lastIndex++;
    
    boardObject = &m_objects[m_objects.size() - 1];
    // add frame object
    std::vector<unsigned int> frameSquareAttribute;
    frameSquareAttribute.push_back(m_assetManager.getAttribute("data/textures/board/frame.png", GL_REPEAT, WHITE_MATERIAL, s_meshShaderPath2 ));
    
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(0.0f, 0.001f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(1.0f, 1.0f, 1.0f), // scale
                           m_assetManager.getMesh("square.gen"),
                           s_meshShaderPath2 );
    m_lastIndex++;
    
    m_objects[m_objects.size() - 1].SetObjectAttributes(frameSquareAttribute);
        
    m_curObj = &m_objects[m_objects.size() - 1];
    // add skybox object
    m_objects.emplace_back(m_assetManager,
                           m_camera.GetPosition(), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(200.0f, 200.0f, 200.0f), // scale
                           m_assetManager.getMesh("skybox.gen"),
                           s_meshShaderPath2 );
    
    m_lastIndex++;
        
    std::vector<unsigned int> cubeAttribute;
    Material skyboxMaterial(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(2.0f, 2.0f, 2.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1);
    cubeAttribute.push_back(m_assetManager.getAttribute("data/textures/skybox/posz.jpg", GL_CLAMP_TO_EDGE, skyboxMaterial, s_meshShaderPath2 ));
    cubeAttribute.push_back(m_assetManager.getAttribute("data/textures/skybox/posy.jpg", GL_CLAMP_TO_EDGE, skyboxMaterial, s_meshShaderPath2 ));
    cubeAttribute.push_back(m_assetManager.getAttribute("data/textures/skybox/posx.jpg", GL_CLAMP_TO_EDGE, skyboxMaterial, s_meshShaderPath2 ));
    cubeAttribute.push_back(m_assetManager.getAttribute("data/textures/skybox/negx.jpg", GL_CLAMP_TO_EDGE, skyboxMaterial, s_meshShaderPath2 ));
    cubeAttribute.push_back(m_assetManager.getAttribute("data/textures/skybox/negy.jpg", GL_CLAMP_TO_EDGE, skyboxMaterial, s_meshShaderPath2 ));
    cubeAttribute.push_back(m_assetManager.getAttribute("data/textures/skybox/negz.jpg", GL_CLAMP_TO_EDGE, skyboxMaterial, s_meshShaderPath2 ));    
    m_objects[m_objects.size() - 1].SetObjectAttributes(cubeAttribute);
        
    // get attribute needed for the board pawns and highlighted squares
    Material black(glm::vec4(0.428f, 0.2667f, 0.18f, 1.0f),
                   glm::vec4(0.385f, 0.239f, 0.157f, 1.0f),
                   glm::vec4(0.428f, 0.2667f, 0.18f, 1.0f),
                   glm::vec4(0.385f, 0.239f, 0.157f, 1.0f), 8.0f);
    m_blackAttribute = m_assetManager.getAttribute("", GL_REPEAT ,black, s_meshShaderPath2 );
    
    Material blue (glm::vec4(0.0f, 0.0f, 0.8f, 0.4f),
                   glm::vec4(0.0f, 0.0f, 0.8f, 0.4f),
                   glm::vec4(0.0f, 0.0f, 0.8f, 0.4f),
                   glm::vec4(0.0f, 0.0f, 0.8f, 0.4f), 8.0f);
    m_blueAttribute = m_assetManager.getAttribute("", GL_REPEAT, blue, s_meshShaderPath2 );
    
    Material red  (glm::vec4(0.8f, 0.0f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.0f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.0f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.0f, 0.0f, 0.4f), 8.0f);
    m_redAttribute = m_assetManager.getAttribute("", GL_REPEAT, red, s_meshShaderPath2 );
    
    Material yellow (glm::vec4(0.8f, 0.8f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.8f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.8f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.8f, 0.0f, 0.4f), 8.0f);
    m_yellowAttribute = m_assetManager.getAttribute("", GL_REPEAT, yellow, s_meshShaderPath2 );
    
    gameBoard = new board();
    gameBoard->connectToPieceCreated(boost::bind(&ChessScene::onChessPieceCreated, this, _1));
    gameBoard->connectToPieceMoved(boost::bind(&ChessScene::onChessPieceMoved, this, _1, _2, _3));
    
    gameBoard->init();
    
    m_promotionDialog.getButton(IDC_KNIGHT)->connectToClick( boost::bind(&ChessScene::onPromotionSelected, this, _1));
    m_promotionDialog.getButton(IDC_BISHOP)->connectToClick( boost::bind(&ChessScene::onPromotionSelected, this, _1));
    m_promotionDialog.getButton(IDC_ROOK)->connectToClick( boost::bind(&ChessScene::onPromotionSelected, this, _1));
    m_promotionDialog.getButton(IDC_QUEEN)->connectToClick( boost::bind(&ChessScene::onPromotionSelected, this, _1));
}

//-----------------------------------------------------------------------------
// Name : newGame
//-----------------------------------------------------------------------------
void ChessScene::newGame()
{
    m_objects.erase(m_objects.begin() + 3, m_objects.end());
    
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
            pieceObjects[i][j] = -1;
    }
    
    m_lastIndex = 3;
    gameBoard->resetGame();
}

//-----------------------------------------------------------------------------
// Name : loadGame
//-----------------------------------------------------------------------------
void ChessScene::loadGame()
{
    m_objects.erase(m_objects.begin() + 3, m_objects.end());
    
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
            pieceObjects[i][j] = -1;
    }
    
    m_lastIndex = 3;
    gameBoard->LoadBoardFromFile();
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent
//-----------------------------------------------------------------------------
bool ChessScene::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    switch(event.type)
    {
        case MouseEventType::MouseMoved:
        {
            Object* temp = PickObject(event.cursorPos, m_faceCount, m_meshIndex);
            if (temp != nullptr && m_meshIndex < 2)
            {
                Point squarePicked = getPickedSquare(m_faceCount, m_meshIndex);
                
                //if(frameSquareObject)
                    m_objects[1].SetPos(glm::vec3(squarePicked.x * 10, 0.001f, squarePicked.y * 10));
            }
            else
            {
                m_faceCount = -1;
                m_meshIndex = -1;
            }
            
            return true;
        }break;
               
        case MouseEventType::LeftButton:
        case MouseEventType::DoubleLeftButton:
        {
            if (event.down)
            {
                Object* temp = PickObject(event.cursorPos, m_faceCount, m_meshIndex);
                if (temp != nullptr && m_meshIndex < 2)
                {
                    Point squarePicked = getPickedSquare(m_faceCount, m_meshIndex);
                    if(frameSquareObject)
                        frameSquareObject->SetPos(glm::vec3(squarePicked.x * 10, 0.001f, squarePicked.y * 10));
                    
                    gameBoard->processPress(pointToBoardPoint((squarePicked)));
                    highLightSquares();
                    
                    if (gameBoard->isUnitPromotion())
                        m_promotionDialog.setVisible(true);
                }
                else
                {
                    m_faceCount = -1;
                    m_meshIndex = -1;
                }
                
                return true;
            }
        }break;
        
        default:
            return false;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : PickObject
//-----------------------------------------------------------------------------
Object * ChessScene::PickObject(Point& cursor, int& faceCount, int &meshIndex)
{
    const glm::mat4x4& matPorj = m_camera.GetProjMatrix();

    glm::vec3 v;
    v.x = ( ( ( 2.0f * cursor.x) / m_camera.GetViewport().width) - 1) / matPorj[0][0];
    v.y = -( ( ( 2.0f * cursor.y) / m_camera.GetViewport().height) - 1) / matPorj[1][1];
    v.z = -1.0f;

    glm::mat4x4 view = m_camera.GetViewMatrix();
    view = glm::inverse(view);

    glm::vec3 rayDir(
          v.x * view[0][0] + v.y * view[1][0] + v.z * view[2][0],   // x
          v.x * view[0][1] + v.y * view[1][1] + v.z * view[2][1],   // y
          v.x * view [0][2] + v.y * view [1][2] + v.z * view[2][2]);// z

    glm::vec3 rayOrigin(view[3][0], view[3][1], view [3][2]);

    if (m_objects.size() > 0  && !m_objects[0].IsObjectHidden())
    {
        Object& obj = m_objects[0];
        
        glm::mat4x4 worldInverse = glm::inverse(obj.GetWorldMatrix());
        glm::vec3 rayObjOrigin = worldInverse * glm::vec4(rayOrigin, 1.0f);
        glm::vec3 rayObjDir = worldInverse * glm::vec4(rayDir, 0.0f);

        if (obj.GetMesh()->IntersectTriangle(rayObjOrigin, rayObjDir, faceCount, meshIndex))
        {
            return &obj;
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Name : RotateCamera
//-----------------------------------------------------------------------------
void ChessScene::RotateCamera(float frameTimeDelta)
{
    if (m_rotationMode != RotationMode::Stoped)
    {
        float eyeX = -80*glm::cos(m_rotationAngle) - glm::sin(m_rotationAngle);
        float eyeZ = -80*sin(m_rotationAngle) - cos(m_rotationAngle);
        eyeX += 40;
        eyeZ += 40;            
        glm::vec3 temp = m_camera.GetPosition();
        
        m_camera.SetPostion(glm::vec3(eyeX, temp.y ,eyeZ));
        m_camera.SetLookAt(glm::vec3(40.0f, 0.0f, 40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    switch(m_rotationMode)
    {
        case RotationMode::Infinite:
        {       
            m_rotationAngle += 0.01746f * 20 * frameTimeDelta;
            if (m_rotationAngle > (glm::pi<float>() * 2))
              m_rotationAngle -= glm::pi<float>() * 2;           
        }break;
        
        case RotationMode::ReturnToWhite:
        {
            // return to angle 0 through the fastest direction
            if (m_rotationAngle < glm::pi<float>())
            {
                m_rotationMode = RotationMode::RotateCCW;
                m_endAngle = 0;                
            }
            else
            {
                m_rotationMode = RotationMode::RotateCW;
                m_endAngle = glm::pi<float>() * 2;
            }
        }break;
        
        case RotationMode::ReturnToBlack:
        {
            // return to angle pi through the fastest direction
            if (m_rotationAngle < glm::pi<float>())
            {           
                m_rotationMode = RotationMode::RotateCW;
                m_endAngle = glm::pi<float>();
            }
            else
            {   
                m_rotationMode = RotationMode::RotateCCW;
                m_endAngle = glm::pi<float>();
            }
        }break;
        
        case RotationMode::RotateCW:
        {
            if (m_rotationAngle < m_endAngle)
                m_rotationAngle += 0.01746f * 100 * frameTimeDelta;
            else
                m_rotationMode = RotationMode::Stoped;
        }break;
        
        case RotationMode::RotateCCW:
        {
            if (m_rotationAngle > m_endAngle)
                m_rotationAngle -= 0.01746f * 100 * frameTimeDelta;
            else
                m_rotationMode = RotationMode::Stoped;
        }break;
        
        case RotationMode::Stoped:
        {
            if (m_endAngle == (glm::pi<float>() * 2))
                m_endAngle = 0;
                m_rotationAngle = m_endAngle;
        }break;
    }
}

//-----------------------------------------------------------------------------
// Name : Darwing
//-----------------------------------------------------------------------------
void ChessScene::Drawing(double frameTimeDelta)
{
    RotateCamera(frameTimeDelta);
    Scene::Drawing(frameTimeDelta);
    
    m_status = gameBoard->getBoardStatus();
    glm::vec3 cameraPos = m_camera.GetPosition();
    std::stringstream ss;
    ss << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z;
    m_status = ss.str();
}

//-----------------------------------------------------------------------------
// Name : setCameraRotationMode
//-----------------------------------------------------------------------------
void ChessScene::setCameraRotationMode(RotationMode newMode)
{
    if (!m_cameraRotation)
        return;
    
    if (newMode == RotationMode::ReturnToWhite)
    {
        if (gameBoard->getCurrentPlayer() == WHITE)
            m_rotationMode = RotationMode::ReturnToWhite;
        else
            m_rotationMode = RotationMode::ReturnToBlack;
    }
    else
        m_rotationMode = newMode;
}

//-----------------------------------------------------------------------------
// Name : onChessPieceCreated
//-----------------------------------------------------------------------------
void ChessScene::setCameraRotaion(bool enabled)
{
    if (enabled)
    {
        m_cameraRotation = true;
        m_rotationMode = RotationMode::Infinite;
    }
    else
    {
        m_cameraRotation = false;
        m_rotationMode = RotationMode::ReturnToWhite;
    }
}

//-----------------------------------------------------------------------------
// Name : onChessPieceCreated
//-----------------------------------------------------------------------------
void ChessScene::onChessPieceCreated(piece* pPiece)
{
    std::string meshPath;
    
    Point pieceBoardPoint = boardPointToPoint(pPiece->getPosition());
    glm::vec3 piecePosition = glm::vec3(pieceBoardPoint.x * 10 + 5, 0.001f, pieceBoardPoint.y * 10 + 5);
    
    glm::vec3 pieceScale = glm::vec3(8.0f, 8.0f, 8.0f);
    glm::vec3 pieceRotaion = glm::vec3(0.0f, 0.0f, 0.0f);
    switch (pPiece->getType())
    {
        case PAWN:
            meshPath = "data/models/pawn.fbx";
            break;
            
        case BISHOP:
            meshPath = "data/models/bishop.fbx";
            if (pPiece->getColor() == BLACK)
                pieceRotaion.y = glm::half_pi<float>();
            else
                pieceRotaion.y = -glm::half_pi<float>();
            break;
            
        case KNIGHT:
            meshPath = "data/models/knight.fbx";
            if (pPiece->getColor() == BLACK)
                pieceRotaion.y = glm::pi<float>();
            break;
            
        case ROOK:
            meshPath = "data/models/rook.fbx";
            pieceScale = pieceScale * glm::vec3(0.269f, 0.422f, 0.269f);
            piecePosition.y += 3.4f;
            break;
            
        case QUEEN:
            meshPath = "data/models/queen.fbx";
            break;
            
        case KING:
            meshPath = "data/models/king.fbx";
            break;
    }
    
    if (pieceObjects[pieceBoardPoint.y][pieceBoardPoint.x] != -1)
        deletePieceObject(pieceBoardPoint);
    
    m_objects.emplace_back(m_assetManager,
                           piecePosition, // position
                           pieceRotaion, // rotation
                           pieceScale, // scale
                           m_assetManager.getMesh(meshPath),
                             s_meshShaderPath2 );
    m_lastIndex++;
    
    pieceObjects[pieceBoardPoint.y][pieceBoardPoint.x] = m_objects.size() - 1;
    
    
    if (pPiece->getColor() == BLACK)
    {
        std::vector<unsigned int> blackAttribute;
        blackAttribute.push_back(m_blackAttribute);
        m_objects[m_objects.size() - 1].SetObjectAttributes(blackAttribute);
    }
}

//-----------------------------------------------------------------------------
// Name : onChessPieceMoved
//-----------------------------------------------------------------------------
void ChessScene::onChessPieceMoved(piece* pPiece, BOARD_POINT pieceOldBoardPoint, BOARD_POINT pieceNewBoardPoint)
{
     Point pieceOldPoint = boardPointToPoint(pieceOldBoardPoint);
     Point pieceNewPoint = boardPointToPoint(pieceNewBoardPoint);
    glm::vec3 piecePosition = glm::vec3(pieceNewPoint.x * 10 + 5, 0.001f, pieceNewPoint.y * 10 + 5);
    
    if (pPiece->getType() == ROOK)
        piecePosition.y += 3.4f;
    
    if (pieceObjects[pieceNewPoint.y][pieceNewPoint.x] != -1)
        deletePieceObject(pieceNewPoint);
    
    pieceObjects[pieceNewPoint.y][pieceNewPoint.x] = pieceObjects[pieceOldPoint.y][pieceOldPoint.x];
    m_objects[pieceObjects[pieceNewPoint.y][pieceNewPoint.x]].SetPos(piecePosition);
    pieceObjects[pieceOldPoint.y][pieceOldPoint.x] = -1;
    
    if (!gameBoard->isUnitPromotion())
        setCameraRotationMode(RotationMode::ReturnToWhite);
}

//-----------------------------------------------------------------------------
// Name : onChessPieceKilled
//-----------------------------------------------------------------------------
void ChessScene::onChessPieceKilled(piece* pPiece)
{
    Point pieceBoardPoint = boardPointToPoint(pPiece->getPosition());
    int blah = pieceObjects[pieceBoardPoint.y][pieceBoardPoint.x];
    m_objects.erase(m_objects.begin() + blah);
    pieceObjects[pieceBoardPoint.y][pieceBoardPoint.x] = -1;
    
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (pieceObjects[i][j] > blah)
                pieceObjects[i][j] = pieceObjects[i][j] - 1;
}

//-----------------------------------------------------------------------------
// Name : onPromotionSelected
//-----------------------------------------------------------------------------
void ChessScene::onPromotionSelected(ButtonUI* selectedPieceButton)
{
    m_objects.erase(m_objects.begin() + m_lastIndex, m_objects.end());
    m_lastIndex = m_objects.size();
    
    switch(selectedPieceButton->getID())
    {
        case IDC_KNIGHT:
            gameBoard->PromoteUnit(KNIGHT);
            break;
            
        case IDC_BISHOP:
            gameBoard->PromoteUnit(BISHOP);
            break;
            
        case IDC_ROOK:
            gameBoard->PromoteUnit(ROOK);
            break;
            
        case IDC_QUEEN:
            gameBoard->PromoteUnit(QUEEN);
            break;
            
        default:
            break;
    }
    
    m_promotionDialog.setVisible(false);
    setCameraRotationMode(RotationMode::ReturnToWhite);
    highLightSquares();
}

//-----------------------------------------------------------------------------
// Name : highLightSquares
//-----------------------------------------------------------------------------
void ChessScene::highLightSquares()
{
    const std::vector<BOARD_POINT>& moveSquares = gameBoard->getMoveSquares();
    const std::vector<BOARD_POINT>& attackSquares = gameBoard->getAttackSquares();
                    
    m_objects.erase(m_objects.begin() + m_lastIndex, m_objects.end());
    m_lastIndex = m_objects.size();
                    
    const BOARD_POINT& selectedBoardSquare = gameBoard->getSelectedSquare();
    if (selectedBoardSquare.row != -1 && selectedBoardSquare.col != -1)
        hightlightBoardSquare(boardPointToPoint(selectedBoardSquare), m_yellowAttribute);
                    
    const BOARD_POINT& threatBoardSquare = gameBoard->getThreatSquare();
    if (threatBoardSquare.row != -1 && threatBoardSquare.col != -1)
        hightlightBoardSquare(boardPointToPoint(threatBoardSquare), m_redAttribute);
                    
    for (const BOARD_POINT& square : moveSquares)
        hightlightBoardSquare(boardPointToPoint(square), m_blueAttribute);
                    
    for (const BOARD_POINT& square : attackSquares)
        hightlightBoardSquare(boardPointToPoint(square), m_redAttribute);
}

//-----------------------------------------------------------------------------
// Name : hightlightBoardSquare
//-----------------------------------------------------------------------------
void ChessScene::hightlightBoardSquare(Point squareToHightlight, GLuint attributeID)
{
    glm::vec3 squarePosition = glm::vec3(squareToHightlight.x * 10, 0.002f, squareToHightlight.y * 10);
    
    m_objects.emplace_back(m_assetManager,
                           squarePosition, // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(1.0f, 1.0f, 1.0f), // scale
                           m_assetManager.getMesh("square.gen"),
                             s_meshShaderPath2 );
                            
    std::vector<GLuint> m_squareAttribute;
    m_squareAttribute.push_back(attributeID);
    m_objects[m_objects.size() - 1].SetObjectAttributes(m_squareAttribute);
}

//-----------------------------------------------------------------------------
// Name : getPickedSquare
//-----------------------------------------------------------------------------
Point ChessScene::getPickedSquare(int facePicked, int meshPickedIndex)
{
    int row, col;
    col = facePicked / 8;
    row = (facePicked / 2) - (((facePicked / 2) / 4) * 4 );
    if ( (meshPickedIndex == 0 && col % 2 == 0) ||
        (meshPickedIndex == 1 && col % 2 != 0))
        row = row * 2;
    else
        row = row * 2 + 1;
    
    return Point(row, col);
}

//-----------------------------------------------------------------------------
// Name : deletePieceObject
//-----------------------------------------------------------------------------
void ChessScene::deletePieceObject(Point pieceBoardPoint)
{
    int index = pieceObjects[pieceBoardPoint.y][pieceBoardPoint.x];
    m_objects.erase(m_objects.begin() + index);
    pieceObjects[pieceBoardPoint.y][pieceBoardPoint.x] = -1;
        
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (pieceObjects[i][j] > index)
                pieceObjects[i][j] = pieceObjects[i][j] - 1;
                
    m_lastIndex--;
}

//-----------------------------------------------------------------------------
// Name : pointToBoardPoint
//-----------------------------------------------------------------------------
BOARD_POINT ChessScene::pointToBoardPoint(Point pt)
{
    return BOARD_POINT(pt.y, nCellWide - 1 - pt .x);
}

//-----------------------------------------------------------------------------
// Name : boardPointToPoint
//-----------------------------------------------------------------------------
Point ChessScene::boardPointToPoint(BOARD_POINT pt)
{
    return Point(nCellWide - 1 - pt.col, pt.row);
}
