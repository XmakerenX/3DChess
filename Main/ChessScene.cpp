#include "ChessScene.h" 

//-----------------------------------------------------------------------------
// Name : ChessScene
//-----------------------------------------------------------------------------
ChessScene::ChessScene()
{
    
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

    std::vector<unsigned int> frameSquareAttribute;
    frameSquareAttribute.push_back(m_assetManager.getAttribute("boardTextures/frame.png", WHITE_MATERIAL, meshShaderPath2));
    
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(0.0f, 0.001f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(1.0f, 1.0f, 1.0f), // scale
                           m_assetManager.getMesh("square.gen"),
                           meshShaderPath2);
    
    m_objects[1].SetObjectAttributes(frameSquareAttribute);
    
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(5.0f, 0.001f, 5.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(8.0f, 8.0f, 8.0f), // scale
                           m_assetManager.getMesh("models/pawn.fbx"),
                           meshShaderPath2);
    
    curObj = &m_objects[1];
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
                
                m_objects[1].SetPos(glm::vec3(squarePicked.x * 10, 0.001f, squarePicked.y * 10));
                m_objects[2].SetPos(glm::vec3(squarePicked.x * 10 + 5, 0.001f, squarePicked.y * 10 + 5));
            }
            else
            {
                m_faceCount = -1;
                m_meshIndex = -1;
            }
            
            return true;
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

    if (m_objects.size()  > 0 && !m_objects[0].IsObjectHidden())
    {
        Object& obj = m_objects[0];
        
        glm::mat4x4 worldInverse = glm::inverse(obj.GetWorldMatrix());
        glm::vec3 rayObjOrigin = worldInverse * glm::vec4(rayOrigin, 1.0f);
        glm::vec3 rayObjDir = worldInverse * glm::vec4(rayDir, 0.0f);

        if (obj.GetMesh()->IntersectTriangle(rayObjOrigin, rayObjDir, faceCount, meshIndex))
        {
            //curObj = &obj;
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
Point ChessScene::pointToBoardPoint(Point pt)
{
    return Point(pt.y, nCellWide - 1 - pt .x);
}

//-----------------------------------------------------------------------------
// Name : boardPointToPoint
//-----------------------------------------------------------------------------
Point ChessScene::boardPointToPoint(Point pt)
{
    return Point(nCellWide - 1 - pt.y, pt.x);
}
