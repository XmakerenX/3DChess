#include "ChessScene.h" 

const glm::vec2 ChessScene::uvValues[4] = {glm::vec2(1,0), glm::vec2(1,1),
                                           glm::vec2(0,0), glm::vec2(0,1)};
                                           
const glm::vec2 ChessScene::reverseUvValues[4] = {glm::vec2(0,1), glm::vec2(0,0),
                                           glm::vec2(1,1), glm::vec2(1,0)};

ChessScene::ChessScene()
{
    
}

//-----------------------------------------------------------------------------
// Name : InitObjects
//-----------------------------------------------------------------------------
void ChessScene::InitObjects()
{
    //Scene::InitObjects();
    m_boardMesh = createBoardMesh(glm::vec2(10.0f, 10.0f));
    
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(0.0f, 0.0f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(1.0f, 1.0f, 1.0f), // scale
                           &m_boardMesh,
                           meshShaderPath2);
        
    curObj = &m_objects[0];
}

//-----------------------------------------------------------------------------
// Name : createBoardMesh
//-----------------------------------------------------------------------------
Mesh ChessScene::createBoardMesh(const glm::vec2& scale)
{
    float stepX = 1.0f * scale.x;
    float stepZ = 1.0f * scale.y;
    
    std::vector<SubMesh> boardSubMeshes;
    
    createBoardSubMeshes(stepX, stepZ, boardSubMeshes);
    
    createFrameSubMeshes(stepX, stepZ, boardSubMeshes);
        
    Material white;
    white.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    white.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    white.emissive = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    white.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    white.power = 1.0f;
    GLuint materialIndex = m_assetManager.getMaterialIndex(white);
            
    std::vector<GLuint> materials = 
    {
        materialIndex, materialIndex, materialIndex, materialIndex,
        materialIndex, materialIndex, materialIndex, materialIndex,
        materialIndex, materialIndex, materialIndex, materialIndex,
        materialIndex, materialIndex, materialIndex, materialIndex,
        materialIndex, materialIndex, materialIndex, materialIndex,
        materialIndex, materialIndex
    };
    
    std::vector<std::string> texturesPath = {"black2.png", "white.jpg" , 
        "boardTextures/frame1.png", "boardTextures/frame2.png",
        "boardTextures/frame3.png", "boardTextures/frame4.png", 
        "boardTextures/frame5.png", "boardTextures/frame6.png", 
        "boardTextures/frame7.png", "boardTextures/frame8.png",
        "boardTextures/frameA.png", "boardTextures/frameB.png",
        "boardTextures/frameC.png", "boardTextures/frameD.png",
        "boardTextures/frameE.png", "boardTextures/frameF.png",
        "boardTextures/frameG.png", "boardTextures/frameH.png",
        "boardTextures/frameLowerLeft.png", "boardTextures/frameLowerLeft.png",
        "boardTextures/frameLowerLeft.png", "boardTextures/frameLowerLeft.png"
    };
        
    return Mesh(boardSubMeshes, materials, texturesPath);
}

//-----------------------------------------------------------------------------
// Name : createBoardSubMeshes
//-----------------------------------------------------------------------------
void ChessScene::createBoardSubMeshes(float stepX, float stepZ, std::vector<SubMesh>& boardSubMeshes)
{
    std::vector<Vertex> boardSquaresVertices;
    
    glm::vec3 boardPos = glm::vec3(0, 0, 0);
    
    int nVertX = nCellHigh + 1;
    int nVertZ = nCellWide + 1;
    float tU = 0.0f;
    float tV = 0.0f;
    
    for (int z = 0; z < nVertZ; z++)
    {
        tU = 0.0f;
        boardPos.x  = 0;
        
        for (int x = 0; x < nVertX; x++)
        {
            boardSquaresVertices.emplace_back(boardPos, 
                                              glm::vec3(0.0f, 1.0f, 0.0f),
                                              glm::vec2(tU, tV));
            
            tU++;
            boardPos.x += stepX;
        }
                
        tV++;
        boardPos.z += stepZ;
    }
    
    std::vector<VertexIndex> blackSqureIndices;
    std::vector<VertexIndex> whiteSqureIndices;
    VertexIndex vIndex = 0;
    for (int z = 0; z < nCellWide; z++)
    {
        for (int x = 0; x < nCellHigh; x++)
        {
            if (vIndex % 2 == 0)
            {
                createBoardIndices(blackSqureIndices, vIndex, nVertX);
            }
            else
            {
                createBoardIndices(whiteSqureIndices, vIndex, nVertX);
            }
            
            vIndex++;
        }
        vIndex++;
    }
    
    boardSubMeshes.emplace_back(boardSquaresVertices, blackSqureIndices);
    boardSubMeshes.emplace_back(boardSquaresVertices, whiteSqureIndices);
}

//-----------------------------------------------------------------------------
// Name : createBoardIndices
//-----------------------------------------------------------------------------
void ChessScene::createBoardIndices(std::vector<VertexIndex>& squareIndices, VertexIndex curIndex, int nVertX)
{
    // first triangle
    squareIndices.push_back(curIndex);
    squareIndices.push_back(curIndex + nVertX);
    squareIndices.push_back(curIndex + nVertX + 1);
    
    // secound triangle
    squareIndices.push_back(curIndex);
    squareIndices.push_back(curIndex + nVertX + 1);
    squareIndices.push_back(curIndex + 1);
}

//-----------------------------------------------------------------------------
// Name : createFrameSubMeshes
//-----------------------------------------------------------------------------
void ChessScene::createFrameSubMeshes(float stepX, float stepZ, std::vector<SubMesh>& boardSubMeshes)
{
    int nVertX = nCellHigh + 1;
    int nVertZ = nCellWide + 1;
    
    glm::vec3 boardPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 framePos = glm::vec3(boardPos.x - stepX, boardPos.y, boardPos.z);    
    for (int i = 1; i < nVertZ; i++)
    {
        std::vector<Vertex> frameVertices;
        std::vector<VertexIndex> frameIndices;
        
        createVerticalFrameSquare(frameVertices, frameIndices, framePos, stepX, stepZ, i);
        
        boardSubMeshes.emplace_back(frameVertices, frameIndices); 
    }
    
    for (int i = 1; i < nVertX; i++)
    {
        std::vector<Vertex> frameVertices;
        std::vector<VertexIndex> frameIndices;
        
        createHorizontalFrameSquare(frameVertices, frameIndices, framePos, stepX, stepZ, i);
        
        boardSubMeshes.emplace_back(frameVertices, frameIndices);
    }
    
    std::vector<Vertex> frameCornerVertices;
    std::vector<VertexIndex> frameCornerIndices;
    createCornersFrameSquare(frameCornerVertices, frameCornerIndices,framePos, stepX, stepZ);
    boardSubMeshes.emplace_back(frameCornerVertices, frameCornerIndices);
}

//-----------------------------------------------------------------------------
// Name : createVerticalFrameSquare
//-----------------------------------------------------------------------------
void ChessScene::createVerticalFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ, int i)
{
    glm::vec3 pos = framePos;
    pos.x += i * stepX;
    createFrameSquare(pos, stepX, stepZ, uvValues, 0, frameSquaresVertices, frameIndices);
    
    pos = framePos;
    pos.x += i * stepX;
    pos.z += 9 * stepZ;
    createFrameSquare(pos, stepX, stepZ, reverseUvValues, 0, frameSquaresVertices, frameIndices);
}

//-----------------------------------------------------------------------------
// Name : createHorizontalFrameSquare
//-----------------------------------------------------------------------------
void ChessScene::createHorizontalFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ, int i)
{
    glm::vec3 pos = framePos;
    pos.z += i * stepZ;
    createFrameSquare(pos, stepX, stepZ, uvValues, 0, frameSquaresVertices, frameIndices);

    pos = framePos;
    pos.z += i * stepZ;
    pos.x += 9 * stepX;
    createFrameSquare(pos, stepX, stepZ, reverseUvValues, 2, frameSquaresVertices, frameIndices);
}

//-----------------------------------------------------------------------------
// Name : createCornersFrameSquare
//-----------------------------------------------------------------------------
void ChessScene::createCornersFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ)
{
    glm::vec3 pos = framePos;
    pos.z += 0 * stepZ;
    createFrameSquare(pos, stepX, stepZ, uvValues, 0, frameSquaresVertices, frameIndices);

    pos = framePos;
    pos.z += 0 * stepZ;
    pos.x += 9 * stepX;
    createFrameSquare(pos, stepX, stepZ, reverseUvValues, 2, frameSquaresVertices, frameIndices);

    pos = framePos;
    pos.z += 9 * stepZ;
    createFrameSquare(pos, stepX, stepZ, uvValues, 2, frameSquaresVertices, frameIndices);

    pos = framePos;
    pos.z += 9 * stepZ;
    pos.x += 9 * stepX;
    createFrameSquare(pos, stepX, stepZ, reverseUvValues, 0, frameSquaresVertices, frameIndices);
}

//-----------------------------------------------------------------------------
// Name : createFrameSquare
//-----------------------------------------------------------------------------
void ChessScene::createFrameSquare(glm::vec3 pos, float stepX, float stepZ, const glm::vec2 uvValues[4],int startValue,std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices)
{
    glm::vec2 temp = uvValues[startValue % 4];
    frameSquaresVertices.emplace_back(pos,
                                      glm::vec3(0.0f, 1.0f, 0.0f),
                                      temp);
    
    pos.x += stepX;
    temp = uvValues[(startValue + 1) % 4];
    frameSquaresVertices.emplace_back(pos,
                                      glm::vec3(0.0f, 1.0f, 0.0f),
                                      temp);
    
    pos.x -= stepX;
    pos.z -= stepZ;
    temp = uvValues[(startValue + 2) % 4];
    frameSquaresVertices.emplace_back(pos,
                                      glm::vec3(0.0f, 1.0f, 0.0f),
                                      temp);
    
    pos.x += stepX;
    temp = uvValues[(startValue + 3) % 4];
    frameSquaresVertices.emplace_back(pos,
                                      glm::vec3(0.0f, 1.0f, 0.0f),
                                      temp);
    
    VertexIndex vIndex = (frameIndices.size() / 6) * 4;
    // first triangle
    frameIndices.push_back(vIndex);
    frameIndices.push_back(vIndex + 1 + 1 + 1);
    frameIndices.push_back(vIndex + 1 + 1);
     
    // secound triangle
    frameIndices.push_back(vIndex);
    frameIndices.push_back(vIndex + 1);
    frameIndices.push_back(vIndex + 1 + 1 + 1);
}
