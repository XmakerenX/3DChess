#ifndef  _SPRITE_H
#define _SPRITE_H

#include <vector>
#include <list>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "RenderTypes.h"

struct VertexSprite
{

    VertexSprite(float fX, float fY, float fZ, const glm::vec3& diffuseCOlor, float ftu = 0.0f, float ftv = 0.0f)
    {
        pos.x = fX;
        pos.y = fY;
        pos.z = fZ;
        pos.w = 1;
        diffuse = diffuseCOlor;
        uv.x = ftu;
        uv.y = ftv;
    }

    glm::vec4 pos;
    glm::vec3 diffuse;
    glm::vec2 uv;
};

//struct Quad
//{
//    VertexSprite vertices[4];
//    GLuint indices[6];
//};

struct StreamOfVertices
{
    StreamOfVertices(GLuint textureName);

    //void addQuad(Rect &srcRect, Point quadPos, glm::vec3 tintColor);
    void addQuad(Rect& spriteRect, Rect& texRect, glm::vec3 tintColor, Point scale);

    GLuint textureName;

    std::vector<VertexSprite> vertices;
    std::vector<VertexIndex> indices;
};

//struct BufferBounds
//{
//    GLuint textureNmae;
//    GLuint bufferStartVertex;
//    GLuint bufferEndVertex;
//    GLuint bufferStartIndex;
//    GLuint bufferEndIndex;
//};

class Sprite
{
public:
    enum STREAMTYPE{BACKGROUND, REGULAR, HIGHLIGHT, TOP, STREAMTYPE_MAX};

    Sprite();
    ~Sprite();

    bool AddQuad(GLuint textureName, Rect& spriteRect, Rect& texRect, glm::vec3 tintColor);
    //bool AddQuad(GLuint textureName, Rect& srcRect, Point quadPos, glm::vec3 tintColor);
    bool AddQuad(GLuint textureName, Rect& spriteRect, Rect& texRect, glm::vec3 tintColor, Point scale);
    void Clear();

    bool Init();
    bool Render();

private:
    const GLuint MAX_QUADS = 200;

    GLuint m_vertexArrayObject;
    GLuint m_vertexBuffer;
    GLuint m_indicesBuffer;

    std::vector<StreamOfVertices> m_vertexStreams;

    float m_fScaleWidth;
    float m_fScaleHeight;
};

#endif  //_SPRITE_H
