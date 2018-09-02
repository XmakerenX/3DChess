#ifndef  _MESHGENERATOR_H
#define  _MESHGENERATOR_H

#include "AssetManager.h"
#include "Mesh.h"

class MeshGenerator
{
public:
    static Mesh createBoardMesh(AssetManager& assetManager, const glm::vec2& scale);
    static Mesh createSquareMesh(AssetManager& assetManager, const glm::vec2& scale);
    static Mesh createSkyBoxMesh();
    
private:
    static void createBoardSubMeshes(float stepX, float stepZ, std::vector<SubMesh>& boardSubMeshes);
    static void createBoardIndices(std::vector<VertexIndex>& squareIndices, VertexIndex curIndex, int nVertX);
    
    static void createFrameSubMeshes(float stepX, float stepZ, std::vector<SubMesh>& boardSubMeshes);
    static void createHorizontalFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ, int i);
    static void createVerticalFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ, int i);
    static void createCornersFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ);
    static void createFrameSquare(glm::vec3 pos, float stepX, float stepZ, const glm::vec2 uvValues[4],int startValue,std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices);
    static void createSquareIndices(std::vector<VertexIndex>& indices);
    
    static const int nCellHigh = 8;
    static const int nCellWide = 8;
    static const glm::vec2 uvValues[4];
    static const glm::vec2 reverseUvValues[4];
};

#endif  //_MESHGENERATOR_H
