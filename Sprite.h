#ifndef  _SPRITE_H
#define _SPRITE_H

#include <vector>
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
//    float x;
//    float y;
//    float z;
//    float rhw;
    glm::vec3 diffuse;
    glm::vec2 uv;
//    float tu;
//    float tv;
};

struct StreamOfVertices
{
    GLuint textureName;

    // TODO: use vector instead..
    VertexSprite* pVertices;
    GLuint* pIndices;

    GLuint nVertices;
    GLuint nIndices;

};

struct BufferBounds
{
    GLuint textureNmae;
    GLuint bufferStartVertex;
    GLuint bufferEndVertex;
    GLuint bufferStartIndex;
    GLuint bufferEndIndex;
};

class Sprite
{
public:
    enum STREAMTYPE{BACKGROUND, REGULAR, HIGHLIGHT, TOP, STREAMTYPE_MAX};

    Sprite();
    ~Sprite();

    bool CreateQuad(GLuint textureName, Rect& srcRect, Point quadPos, glm::vec3 tintColor, STREAMTYPE vertexType);

    bool Init();
    bool Render();

private:
    const GLuint MAX_QUADS = 200;

    GLuint m_vertexArrayObject;
    GLuint m_vertexBuffer;
    GLuint m_indicesBuffer;

    std::vector<StreamOfVertices> m_vertexStreams[STREAMTYPE_MAX];

    float m_fScaleWidth;
    float m_fScaleHeight;
};

#endif  //_SPRITE_H
