#include "Sprite.h"
#include <iostream>

//-----------------------------------------------------------------------------
// Name : StreamOfVertices (constructor)
//-----------------------------------------------------------------------------
StreamOfVertices::StreamOfVertices(GLuint textureName)
{
    this->textureName = textureName;
}

//-----------------------------------------------------------------------------
// Name : addQuad
//-----------------------------------------------------------------------------
void StreamOfVertices::addQuad(Rect spriteRect, Rect texRect, glm::vec3 tintColor, Point scale)
//bool Sprite::AddQuad(GLuint textureName, Rect& spriteRect, Rect& texRect, glm::vec3 tintColor, Point scale = Point(1,1))
{
    float textureRectWidth, textureRectHeight;
    int textureWidth, textureHeight;
    float startU, startV, widthU, heightV;
    startU = startV = widthU = heightV = 0;
    // calculate how much to scale  the UV coordinates of the texture to fit the texRect
    if (textureName != 0)
    {
        // get the texture width and height
        glGetTextureLevelParameteriv(textureName, 0, GL_TEXTURE_WIDTH, &textureWidth);
        glGetTextureLevelParameteriv(textureName, 0, GL_TEXTURE_HEIGHT, &textureHeight);

        startU = static_cast<float>(texRect.left) / static_cast<float>(textureWidth);
        startV = static_cast<float>(texRect.top) / static_cast<float>(textureHeight);

        textureRectWidth  = static_cast<float>( texRect.right - texRect.left);
        textureRectHeight = static_cast<float>( texRect.botoom - texRect.top );

        widthU = (static_cast<float>( texRect.right - texRect.left) / textureWidth );
        heightV = (static_cast<float>( texRect.botoom - texRect.top ) / textureHeight );
    }

    int spriteWidth, spriteHeight;
    spriteWidth = spriteRect.right - spriteRect.left;
    spriteHeight = spriteRect.botoom - spriteRect.top;

    Point quadPos = Point(spriteRect.left, spriteRect.top);

    GLuint vIndex = vertices.size();

    // creates the four vertices of our quad
    vertices.emplace_back(quadPos.x, quadPos.y, 0, tintColor,
                          startU, startV + heightV);
    vertices.emplace_back(quadPos.x + spriteWidth * scale.x, quadPos.y, 0,tintColor,
                          startU + widthU, startV + heightV);
    vertices.emplace_back(quadPos.x, quadPos.y + spriteHeight * scale.y, 0, tintColor,
                          startU, startV);
    vertices.emplace_back(quadPos.x + spriteWidth * scale.x,
                          quadPos.y + spriteHeight * scale.y, 0, tintColor,
                          startU + widthU, startV);

    // triangle 201
//    indices.push_back(vIndex + 2); // #2 vertex  0---1 4---5
//    indices.push_back(vIndex);     // #0 vertex  -   - -   -
//    indices.push_back(vIndex + 1); // #1 vertex  2---3 6---7
//    // triangle 213
//    indices.push_back(vIndex + 2); // #2 vertex
//    indices.push_back(vIndex + 1); // #1 vertex
//    indices.push_back(vIndex + 3); // #3 vertex

    // triangle 201
    indices.push_back(vIndex); // #2 vertex  0---1 4---5
    indices.push_back(vIndex + 2);     // #0 vertex  -   - -   -
    indices.push_back(vIndex + 1); // #1 vertex  2---3 6---7
    // triangle 213
    indices.push_back(vIndex + 2); // #2 vertex
    indices.push_back(vIndex + 3); // #1 vertex
    indices.push_back(vIndex + 1); // #3 vertex
}

//-----------------------------------------------------------------------------
// Name : Sprite (constructor)
//-----------------------------------------------------------------------------
Sprite::Sprite()
{
    m_vertexArrayObject = 0;
    m_vertexBuffer = 0;
    m_indicesBuffer = 0;

    m_fScaleWidth = 1;
    m_fScaleHeight = 1;
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
    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexSprite) * MAX_QUADS * 4, NULL, GL_DYNAMIC_DRAW);
    glGenBuffers(1, &m_indicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(VertexIndex) * MAX_QUADS * 6, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexSprite), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSprite), (GLvoid*)offsetof(VertexSprite, diffuse));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexSprite), (GLvoid*)offsetof(VertexSprite, uv));

    glBindVertexArray(0);
}

//-----------------------------------------------------------------------------
// Name : AddTintedQuad ()
//-----------------------------------------------------------------------------
bool Sprite::AddTintedQuad(Rect&& spriteRect, glm::vec3 tintColor)
{
    return AddQuad(std::move(spriteRect), tintColor, NO_TEXTURE, std::move(EMPTY_RECT), Point(m_fScaleWidth, m_fScaleHeight));
}

//-----------------------------------------------------------------------------
// Name : AddTexturedQuad ()
//-----------------------------------------------------------------------------
bool Sprite::AddTexturedQuad(Rect&& spriteRect, GLuint textureName, Rect&& texRect)
{
    return AddQuad(std::move(spriteRect), WHITE_COLOR, textureName, std::move(texRect), Point(m_fScaleWidth, m_fScaleHeight));
}

//-----------------------------------------------------------------------------
// Name : AddTintedTexturedQuad ()
//-----------------------------------------------------------------------------
bool Sprite::AddTintedTexturedQuad(Rect&& spriteRect, glm::vec3 tintColor, GLuint textureName, Rect&& texRect)
{
    return AddQuad(std::move(spriteRect), tintColor, textureName, std::move(texRect), Point(m_fScaleWidth, m_fScaleHeight));
}

//-----------------------------------------------------------------------------
// Name : AddQuad ()
//-----------------------------------------------------------------------------
bool Sprite::AddQuad(const Rect&& spriteRect, glm::vec3 tintColor, GLuint textureName, const Rect&& texRect,Point scale = Point(1,1))
{
    if (m_vertexStreams.size() == 0 || textureName != m_vertexStreams[m_vertexStreams.size() - 1].textureName)
        m_vertexStreams.emplace_back(textureName);

    m_vertexStreams[m_vertexStreams.size() - 1].addQuad(spriteRect, texRect, tintColor, scale);

    return true;
}

//-----------------------------------------------------------------------------
// Name : Render ()
//-----------------------------------------------------------------------------
bool Sprite::Render(Shader* shader)
{
    int err;

    shader->Use();

    for (StreamOfVertices& vertexStream : m_vertexStreams)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexSprite) * vertexStream.vertices.size(),
                        vertexStream.vertices.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(VertexIndex) * vertexStream.indices.size(),
                        vertexStream.indices.data());

        if (vertexStream.textureName != 0)
        {
            glUniform1i( glGetUniformLocation(shader->Program, "textured"), 1);
            glBindTexture(GL_TEXTURE_2D, vertexStream.textureName);
        }
        else
            glUniform1i( glGetUniformLocation(shader->Program, "textured"), 0);

        glBindVertexArray(m_vertexArrayObject);
        glDrawElements(GL_TRIANGLES, vertexStream.indices.size() , GL_UNSIGNED_INT, 0);

        //glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Name : Render ()
//-----------------------------------------------------------------------------
void Sprite::Clear()
{
    m_vertexStreams.clear();
    m_fScaleWidth  = 1;
    m_fScaleHeight = 1;
}
