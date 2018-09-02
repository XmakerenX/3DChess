#include "ChessScene.h" 

//-----------------------------------------------------------------------------
// Name : ChessScene
//-----------------------------------------------------------------------------
ChessScene::ChessScene()
{
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
// Name : InitObjects
//-----------------------------------------------------------------------------
void ChessScene::InitObjects()
{
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(0.0f, 0.0f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(1.0f, 1.0f, 1.0f), // scale
                           m_assetManager.getMesh("board.gen"),
                           meshShaderPath2);
    
    boardObject = &m_objects[m_objects.size() - 1];

    std::vector<unsigned int> frameSquareAttribute;
    frameSquareAttribute.push_back(m_assetManager.getAttribute("boardTextures/frame.png", WHITE_MATERIAL, meshShaderPath2));
    
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(0.0f, 0.001f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(1.0f, 1.0f, 1.0f), // scale
                           m_assetManager.getMesh("square.gen"),
                           meshShaderPath2);
    
    m_objects[m_objects.size() - 1].SetObjectAttributes(frameSquareAttribute);
        
    curObj = &m_objects[m_objects.size() - 1];
    
    m_objects.emplace_back(m_assetManager,
                           m_camera.GetPosition(), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(200.0f, 200.0f, 200.0f), // scale
                           m_assetManager.getMesh("skybox.gen"),
                           meshShaderPath2);
        
    std::vector<unsigned int> cubeAttribute;
    cubeAttribute.push_back(m_assetManager.getAttribute("skyboxTextures/posz.jpg", WHITE_MATERIAL, meshShaderPath2));
    cubeAttribute.push_back(m_assetManager.getAttribute("skyboxTextures/posy.jpg", WHITE_MATERIAL, meshShaderPath2));
    cubeAttribute.push_back(m_assetManager.getAttribute("skyboxTextures/posx.jpg", WHITE_MATERIAL, meshShaderPath2));
    cubeAttribute.push_back(m_assetManager.getAttribute("skyboxTextures/negx.jpg", WHITE_MATERIAL, meshShaderPath2));
    cubeAttribute.push_back(m_assetManager.getAttribute("skyboxTextures/negy.jpg", WHITE_MATERIAL, meshShaderPath2));
    cubeAttribute.push_back(m_assetManager.getAttribute("skyboxTextures/negz.jpg", WHITE_MATERIAL, meshShaderPath2));
    
    m_objects[m_objects.size() - 1].SetObjectAttributes(cubeAttribute);
    
    Material black(glm::vec4(0.428f, 0.2667f, 0.18f, 1.0f),
                   glm::vec4(0.385f, 0.239f, 0.157f, 1.0f),
                   glm::vec4(0.428f, 0.2667f, 0.18f, 1.0f),
                   glm::vec4(0.385f, 0.239f, 0.157f, 1.0f), 8.0f);
    m_blackAttribute = m_assetManager.getAttribute("", black, meshShaderPath2);
    
    Material blue (glm::vec4(0.0f, 0.0f, 0.8f, 0.4f),
                   glm::vec4(0.0f, 0.0f, 0.8f, 0.4f),
                   glm::vec4(0.0f, 0.0f, 0.8f, 0.4f),
                   glm::vec4(0.0f, 0.0f, 0.8f, 0.4f), 8.0f);
    m_blueAttribute = m_assetManager.getAttribute("", blue, meshShaderPath2);
    
    Material red  (glm::vec4(0.8f, 0.0f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.0f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.0f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.0f, 0.0f, 0.4f), 8.0f);
    m_redAttribute = m_assetManager.getAttribute("", red, meshShaderPath2);
    
    Material yellow (glm::vec4(0.8f, 0.8f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.8f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.8f, 0.0f, 0.4f),
                   glm::vec4(0.8f, 0.8f, 0.0f, 0.4f), 8.0f);
    m_yellowAttribute = m_assetManager.getAttribute("", yellow, meshShaderPath2);
    
    gameBoard = new board();
    gameBoard->connectToPieceCreated(boost::bind(&ChessScene::onChessPieceCreated, this, _1));
    gameBoard->connectToPieceMoved(boost::bind(&ChessScene::onChessPieceMoved, this, _1, _2, _3));
    //gameBoard->connectToPieceKilled(boost::bind(&ChessScene::onChessPieceKilled, this, _1));
    
    gameBoard->init();
    
    m_lastIndex = m_objects.size();
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
// Name : Darwing
//-----------------------------------------------------------------------------
void ChessScene::Drawing()
{
    Scene::Drawing();
    status = gameBoard->getBoardStatus();
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
            meshPath = "models/pawn.fbx";
            break;
            
        case BISHOP:
            meshPath = "models/bishop.fbx";
            if (pPiece->getColor() == BLACK)
                pieceRotaion.y = glm::half_pi<float>();
            else
                pieceRotaion.y = -glm::half_pi<float>();
            break;
            
        case KNIGHT:
            meshPath = "models/knight.fbx";
            if (pPiece->getColor() == BLACK)
                pieceRotaion.y = glm::pi<float>();
            break;
            
        case ROOK:
            meshPath = "models/rook.fbx";
            pieceScale = pieceScale * glm::vec3(0.269f, 0.422f, 0.269f);
            piecePosition.y += 3.4f;
            break;
            
        case QUEEN:
            meshPath = "models/queen.fbx";
            break;
            
        case KING:
            meshPath = "models/king.fbx";
            break;
    }
    
    m_objects.emplace_back(m_assetManager,
                           piecePosition, // position
                           pieceRotaion, // rotation
                           pieceScale, // scale
                           m_assetManager.getMesh(meshPath),
                           meshShaderPath2);
    
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
    {
        int blah = pieceObjects[pieceNewPoint.y][pieceNewPoint.x];
        m_objects.erase(m_objects.begin() + blah);
        pieceObjects[pieceNewPoint.y][pieceNewPoint.x] = -1;
        
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                if (pieceObjects[i][j] > blah)
                    pieceObjects[i][j] = pieceObjects[i][j] - 1;
                
        m_lastIndex--;
    }
    
    pieceObjects[pieceNewPoint.y][pieceNewPoint.x] = pieceObjects[pieceOldPoint.y][pieceOldPoint.x];
    m_objects[pieceObjects[pieceNewPoint.y][pieceNewPoint.x]].SetPos(piecePosition);
    pieceObjects[pieceOldPoint.y][pieceOldPoint.x] = -1;
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
                           meshShaderPath2);
                            
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
