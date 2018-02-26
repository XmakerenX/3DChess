#include "Sprite.h"

//-----------------------------------------------------------------------------
// Name : Sprite (constructor)
//-----------------------------------------------------------------------------
Sprite::Sprite()
{
    m_vertexArrayObject = 0;
    m_vertexBuffer = 0;
    m_indicesBuffer = 0;
}

//-----------------------------------------------------------------------------
// Name : Sprite (Destructor)
//-----------------------------------------------------------------------------
Sprite::~Sprite()
{

}

//-----------------------------------------------------------------------------
// Name : Init ()
//-----------------------------------------------------------------------------
bool Sprite::Init()
{
    glBindVertexArray(m_vertexArrayObject);
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexSprite) * MAX_QUADS * 4, NULL, GL_DYNAMIC_DRAW);
    glGenBuffers(1, &m_indicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * MAX_QUADS * 6, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexSprite), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSprite), (GLvoid*)offsetof(VertexSprite, diffuse));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexSprite), (GLvoid*)offsetof(VertexSprite, uv));

    glBindVertexArray(0);
}

//-----------------------------------------------------------------------------
// Name : CreateQuad ()
//-----------------------------------------------------------------------------
bool Sprite::CreateQuad(GLuint textureName, Rect& srcRect, Point quadPos, glm::vec3 tintColor, STREAMTYPE vertexType)
{
    std::vector<StreamOfVertices> vertexStream = m_vertexStreams[vertexType];
    for (GLuint i = 0; i < vertexStream.size(); i++)
    {
        if (textureName == vertexStream[i].textureName)
        {
            return true;
        }
    }
}

