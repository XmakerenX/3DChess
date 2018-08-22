#ifndef _CHESSSCENE_H_
#define _CHESSSCENE_H_

#include "../Scene.h"

class ChessScene : public Scene
{
public:
    ChessScene();
    
    virtual void InitObjects();
    Mesh createBoardMesh(const glm::vec2& scale);
    void createBoardSubMeshes(float stepX, float stepZ, std::vector<SubMesh>& boardSubMeshes);
    void createBoardIndices(std::vector<VertexIndex>& squareIndices, VertexIndex curIndex, int nVertX);
    
    void createFrameSubMeshes(float stepX, float stepZ, std::vector<SubMesh>& boardSubMeshes);
    void createHorizontalFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ, int i);
    void createVerticalFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ, int i);
    void createCornersFrameSquare(std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices, const glm::vec3& framePos, float stepX, float stepZ);
    
    void createFrameSquare(glm::vec3 pos, float stepX, float stepZ, const glm::vec2 uvValues[4],int startValue,std::vector<Vertex>& frameSquaresVertices, std::vector<VertexIndex>& frameIndices);
    
private:
    Mesh m_boardMesh;
    static const int nCellHigh = 8;
    static const int nCellWide = 8;
    static const glm::vec2 uvValues[4];
    static const glm::vec2 reverseUvValues[4];

};

#endif // _CHESSSCENE_H_ 
