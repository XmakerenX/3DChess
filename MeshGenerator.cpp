#include "MeshGenerator.h" 

const glm::vec2 MeshGenerator::uvValues[4] = {glm::vec2(1,0),
                                              glm::vec2(1,1),
                                              glm::vec2(0,0),
                                              glm::vec2(0,1)};
                                           
const glm::vec2 MeshGenerator::reverseUvValues[4] = {glm::vec2(1,1),
                                                     glm::vec2(1,0),
                                                     glm::vec2(0,1),
                                                     glm::vec2(0,0)};

const glm::vec2 MeshGenerator::reverseVerticalUvValues[4] = {glm::vec2(0,1),
                                                             glm::vec2(0,0),
                                                             glm::vec2(1,1),
                                                             glm::vec2(1,0)};
                                           
//-----------------------------------------------------------------------------
// Name : createBoardMesh
//-----------------------------------------------------------------------------
Mesh MeshGenerator::createBoardMesh(AssetManager& assetManager, const glm::vec2& scale)
{
    float stepX = 1.0f * scale.x;
    float stepZ = 1.0f * scale.y;
    
    std::vector<SubMesh> boardSubMeshes;
    boardSubMeshes.reserve(19);
    
    createBoardSubMeshes(stepX, stepZ, boardSubMeshes);
    createFrameSubMeshes(stepX, stepZ, boardSubMeshes);
        
    GLuint materialIndex = assetManager.getMaterialIndex(WHITE_MATERIAL);
    std::vector<GLuint> materials = 
    {
        materialIndex, materialIndex, materialIndex, materialIndex,
        materialIndex, materialIndex, materialIndex, materialIndex,
        materialIndex, materialIndex, materialIndex, materialIndex,
        materialIndex, materialIndex, materialIndex, materialIndex,
        materialIndex, materialIndex, materialIndex, materialIndex,
        materialIndex, materialIndex
    };
    
    std::vector<std::string> texturesPath = {"black2.png", "white.jpg", 
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
        
    return Mesh(std::move(boardSubMeshes), std::move(materials), std::move(texturesPath));
}

//-----------------------------------------------------------------------------
// Name : createBoardSubMeshes
//-----------------------------------------------------------------------------
void MeshGenerator::createBoardSubMeshes(float stepX, float stepZ, std::vector<SubMesh>& boardSubMeshes)
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
            boardSquaresVertices.emplace_back(boardPos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(tU, tV));
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
void MeshGenerator::createBoardIndices(std::vector<VertexIndex>& squareIndices, VertexIndex curIndex, int nVertX)
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
void MeshGenerator::createFrameSubMeshes(float stepX, float stepZ, std::vector<SubMesh>& boardSubMeshes)
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
// Name : createHorizontalFrameSquare
//-----------------------------------------------------------------------------
void MeshGenerator::createHorizontalFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ, int i)
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
// Name : createVerticalFrameSquare
//-----------------------------------------------------------------------------
void MeshGenerator::createVerticalFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ, int i)
{
    glm::vec3 pos = framePos;
    pos.x += i * stepX;
    createFrameSquare(pos, stepX, stepZ, uvValues, 0, frameSquaresVertices, frameIndices);
    
    pos = framePos;
    pos.x += i * stepX;
    pos.z += 9 * stepZ;
    createFrameSquare(pos, stepX, stepZ, reverseVerticalUvValues, 0, frameSquaresVertices, frameIndices);   
}

//-----------------------------------------------------------------------------
// Name : createCornersFrameSquare
//-----------------------------------------------------------------------------
void MeshGenerator::createCornersFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ)
{
    glm::vec3 pos = framePos;
    pos.z += 0 * stepZ;
    createFrameSquare(pos, stepX, stepZ, uvValues, 0, frameSquaresVertices, frameIndices);

    pos = framePos;
    pos.z += 0 * stepZ;
    pos.x += 9 * stepX;
    createFrameSquare(pos, stepX, stepZ, reverseVerticalUvValues, 2, frameSquaresVertices, frameIndices);

    pos = framePos;
    pos.z += 9 * stepZ;
    createFrameSquare(pos, stepX, stepZ, uvValues, 2, frameSquaresVertices, frameIndices);

    pos = framePos;
    pos.z += 9 * stepZ;
    pos.x += 9 * stepX;
    createFrameSquare(pos, stepX, stepZ, reverseVerticalUvValues, 0, frameSquaresVertices, frameIndices);
}

//-----------------------------------------------------------------------------
// Name : createFrameSquare
//-----------------------------------------------------------------------------
void MeshGenerator::createFrameSquare(glm::vec3 pos, float stepX, float stepZ, const glm::vec2 uvValues[4],int startValue,std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices)
{
    glm::vec2 UV = uvValues[startValue % 4];
    frameSquaresVertices.emplace_back(pos, glm::vec3(0.0f, 1.0f, 0.0f), UV);
    
    pos.x += stepX;
    UV = uvValues[(startValue + 1) % 4];
    frameSquaresVertices.emplace_back(pos, glm::vec3(0.0f, 1.0f, 0.0f), UV);
    
    pos.x -= stepX;
    pos.z -= stepZ;
    UV = uvValues[(startValue + 2) % 4];
    frameSquaresVertices.emplace_back(pos, glm::vec3(0.0f, 1.0f, 0.0f), UV);
    
    pos.x += stepX;
    UV = uvValues[(startValue + 3) % 4];
    frameSquaresVertices.emplace_back(pos, glm::vec3(0.0f, 1.0f, 0.0f), UV);
    
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

//-----------------------------------------------------------------------------
// Name : createSquareMesh
//-----------------------------------------------------------------------------
Mesh MeshGenerator::createSquareMesh(AssetManager& assetManager, const glm::vec2& scale)
{
    std::vector<Vertex> squareVertices;
    std::vector<VertexIndex> squareIndices;
 
    float stepX = 1.0f * scale.x;
    float stepZ = 1.0f * scale.y;
        
    squareVertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f),   glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
    squareVertices.emplace_back(glm::vec3(stepX, 0.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
    squareVertices.emplace_back(glm::vec3(0, 0.0f, stepZ),     glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
    squareVertices.emplace_back(glm::vec3(stepX, 0.0f, stepZ), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    
    createSquareIndices(squareIndices);
    
    std::vector<SubMesh> squareSubMesh;
    squareSubMesh.emplace_back(squareVertices, squareIndices);
    
    GLuint materialIndex = assetManager.getMaterialIndex(WHITE_MATERIAL);
    std::vector<GLuint> squareMatrial = {materialIndex};
    std::vector<std::string> squareTexture = {""};
    
    return Mesh(std::move(squareSubMesh), std::move(squareMatrial), std::move(squareTexture));
}

//-----------------------------------------------------------------------------
// Name : createSquareIndices
//-----------------------------------------------------------------------------
void MeshGenerator::createSquareIndices(std::vector<VertexIndex>& indices, GLuint vIndex/* = 0*/)
{
    // first triangle
    indices.push_back(vIndex);
    indices.push_back(vIndex + 2);
    indices.push_back(vIndex + 3);
     
    // secound triangle
    indices.push_back(vIndex);
    indices.push_back(vIndex + 3);
    indices.push_back(vIndex + 1);
}

//-----------------------------------------------------------------------------
// Name : createSkyBoxMesh
//-----------------------------------------------------------------------------
Mesh MeshGenerator::createSkyBoxMesh()
{
    std::vector<Vertex> cubeVertices;
    std::vector<VertexIndex> cubeIndices;
    
    std::vector<SubMesh> cubeFaces;
    cubeFaces.reserve(6);
    
    // front face    
    cubeVertices.emplace_back(glm::vec3(10.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(10.0f, -10.0f, -10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(-10.0f, -10.0f, -10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f));
    createSquareIndices(cubeIndices);
    
    cubeFaces.emplace_back(cubeVertices, cubeIndices);
    cubeVertices.clear();
    cubeIndices.clear();
    
    // up face
    cubeVertices.emplace_back(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(10.0f, 10.0f, -10.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f,0.0f));
    cubeVertices.emplace_back(glm::vec3(-10.0f, 10.0f, 10.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
    createSquareIndices(cubeIndices);
    
    cubeFaces.emplace_back(cubeVertices, cubeIndices);
    cubeVertices.clear();
    cubeIndices.clear();

    // right face
    cubeVertices.emplace_back(glm::vec3(10.0f, -10.0f, 10.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(10.0f, -10.0f, -10.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(10.0f, 10.0f, -10.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
    createSquareIndices(cubeIndices);
    
    cubeFaces.emplace_back(cubeVertices, cubeIndices);
    cubeVertices.clear();
    cubeIndices.clear();
    
    // left face
    cubeVertices.emplace_back(glm::vec3(-10.0f, 10.0f, 10.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(-10.0f, -10.0f, -10.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
    createSquareIndices(cubeIndices);
    
    cubeFaces.emplace_back(cubeVertices, cubeIndices);
    cubeVertices.clear();
    cubeIndices.clear();
    
    // down face
    cubeVertices.emplace_back(glm::vec3(10.0f, -10.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(10.0f, -10.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(-10.0f, -10.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
    createSquareIndices(cubeIndices);
    
    cubeFaces.emplace_back(cubeVertices, cubeIndices);
    cubeVertices.clear();
    cubeIndices.clear();
    
    // back face
    cubeVertices.emplace_back(glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(10.0f, -10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(-10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));
    createSquareIndices(cubeIndices);
    
    cubeFaces.emplace_back(cubeVertices, cubeIndices);
    cubeVertices.clear();
    cubeIndices.clear();
        
    std::vector<GLuint> cubeMatrial;
    std::vector<std::string> cubeTexture;
    
    return Mesh(std::move(cubeFaces), std::move(cubeMatrial), std::move(cubeTexture));
}

//-----------------------------------------------------------------------------
// Name : createCubeMesh
//-----------------------------------------------------------------------------
Mesh MeshGenerator::createCubeMesh()
{
    std::vector<Vertex> cubeVertices;
    std::vector<VertexIndex> cubeIndices;
    
    std::vector<SubMesh> cubeFaces;
    float cubeSize = 0.25f;
    
    // down face    
    cubeVertices.emplace_back(glm::vec3(cubeSize, -cubeSize, -cubeSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(-cubeSize, -cubeSize, -cubeSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)); 
    cubeVertices.emplace_back(glm::vec3(cubeSize, -cubeSize, cubeSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)); 
    cubeVertices.emplace_back(glm::vec3(-cubeSize, -cubeSize, cubeSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)); 
    createSquareIndices(cubeIndices, 0);
        
    // up face
    createSquareIndices(cubeIndices, cubeVertices.size());
    cubeVertices.emplace_back(glm::vec3(cubeSize, cubeSize, -cubeSize), glm::vec3(0.0f ,1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(cubeSize, cubeSize, cubeSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(-cubeSize, cubeSize, -cubeSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f,0.0f));
    cubeVertices.emplace_back(glm::vec3(-cubeSize, cubeSize, cubeSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
    
    // right face
    createSquareIndices(cubeIndices, cubeVertices.size());
    cubeVertices.emplace_back(glm::vec3(cubeSize, -cubeSize, -cubeSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(cubeSize, -cubeSize, cubeSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(cubeSize, cubeSize, -cubeSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(cubeSize, cubeSize, cubeSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    
    // left face
    createSquareIndices(cubeIndices, cubeVertices.size());
    cubeVertices.emplace_back(glm::vec3(-cubeSize, -cubeSize, -cubeSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(-cubeSize, cubeSize, -cubeSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(-cubeSize, -cubeSize, cubeSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(-cubeSize, cubeSize, cubeSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
       
    // Front face
    createSquareIndices(cubeIndices, cubeVertices.size());
    cubeVertices.emplace_back(glm::vec3(cubeSize, cubeSize, cubeSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(cubeSize, -cubeSize, cubeSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(-cubeSize, cubeSize, cubeSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(-cubeSize, -cubeSize, cubeSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f));
    
    // back face
    createSquareIndices(cubeIndices, cubeVertices.size());
    cubeVertices.emplace_back(glm::vec3(cubeSize, cubeSize, -cubeSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(-cubeSize, cubeSize, -cubeSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f));
    cubeVertices.emplace_back(glm::vec3(cubeSize, -cubeSize, -cubeSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f));
    cubeVertices.emplace_back(glm::vec3(-cubeSize, -cubeSize, -cubeSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f));
    
    cubeFaces.emplace_back(cubeVertices, cubeIndices);
        
    std::vector<GLuint> cubeMatrial;
    std::vector<std::string> cubeTexture;
    
    return Mesh(std::move(cubeFaces), std::move(cubeMatrial), std::move(cubeTexture));
}
