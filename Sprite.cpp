#include "Sprite.h"

//-----------------------------------------------------------------------------
// Name : StreamOfVertices (constructor)
//-----------------------------------------------------------------------------
StreamOfVertices::StreamOfVertices(GLuint textureName)
{
    this->textureName = textureName;

//    glGenVertexArrays(1, &vertexArrayObject);
//    glGenBuffers(1, &vertexBuffer);
//    glGenBuffers(1, &indicesBuffer);

//    glBindVertexArray(vertexArrayObject);
//    // bind the vertex and indices buffers to Vertex array object
//    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

//    // Set the vertex attribute pointers
//    // Vertex Positions
//    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexSprite), (GLvoid*)0);
//    glEnableVertexAttribArray(0);
//    // Vertex diffuse color
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSprite), (GLvoid*)offsetof(VertexSprite, diffuse));
//    glEnableVertexAttribArray(1);
//    // Vertex texture cordinates
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexSprite), (GLvoid*)offsetof(VertexSprite, uv));
//    glEnableVertexAttribArray(2);

//    glBindVertexArray(0);
}

//-----------------------------------------------------------------------------
// Name : addQuad
//-----------------------------------------------------------------------------
void StreamOfVertices::addQuad(Rect& spriteRect, Rect& texRect, glm::vec3 tintColor, Point scale)
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
        glGetTextureLevelParameteriv(textureName, 0, GL_TEXTURE_HEIGHT, &textureWidth);

        startU = static_cast<float>(texRect.left) / static_cast<float>(textureWidth);
        startV = static_cast<float>(texRect.top) / static_cast<float>(textureHeight);

        textureRectWidth  = static_cast<float>( texRect.right - texRect.left);
        textureRectHeight = static_cast<float>( texRect.botoom - texRect.top );

        widthU = (static_cast<float>( texRect.right - texRect.left) / textureWidth );
        heightV = (static_cast<float>( texRect.botoom - texRect.top ) / textureHeight );
//        widthU = static_cast<float>( textureRectWidth / textureWidth );
//        heightV = static_cast<float>( textureRectHeight / textureHeight );
    }

    int spriteWidth, spriteHeight;
    spriteWidth = spriteRect.right - spriteRect.left;
    spriteHeight = spriteRect.botoom - spriteRect.top;

    Point quadPos = Point(spriteRect.left, spriteRect.top);

    // creates the four vertices of our quad
    vertices.emplace_back(quadPos.x, quadPos.y, 0, tintColor,
                          startU, startV);
    vertices.emplace_back(quadPos.x + spriteWidth * scale.x, quadPos.y, 0,tintColor,
                          startU + widthU, startV);
    vertices.emplace_back(quadPos.x, quadPos.y + spriteHeight * scale.y, 0, tintColor,
                          startU, startV + heightV);
    vertices.emplace_back(quadPos.x + spriteWidth * scale.x,
                          quadPos.y + spriteHeight * scale.y, 0, tintColor,
                          startU + widthU, startV + heightV);

    GLuint vIndex = 0;
    // triangle 201
    indices.push_back(vIndex + 2); // #2 vertex  0---1 4---5
    indices.push_back(vIndex);     // #0 vertex  -   - -   -
    indices.push_back(vIndex + 1); // #1 vertex  2---3 6---7
    // triangle 213
    indices.push_back(vIndex + 2); // #2 vertex
    indices.push_back(vIndex + 1); // #1 vertex
    indices.push_back(vIndex + 3); // #3 vertex
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
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexSprite), (GLvoid*)offsetof(VertexSprite, uv));

    glBindVertexArray(0);
}

//-----------------------------------------------------------------------------
// Name : AddQuad ()
//-----------------------------------------------------------------------------
bool Sprite::AddQuad(GLuint textureName, Rect& spriteRect, Rect& texRect, glm::vec3 tintColor)
{
    return AddQuad(textureName, spriteRect, texRect, tintColor, Point(m_fScaleWidth, m_fScaleHeight));
}

//-----------------------------------------------------------------------------
// Name : AddQuad ()
//-----------------------------------------------------------------------------
//bool Sprite::AddQuad(GLuint textureName, Rect& srcRect, Point quadPos, glm::vec3 tintColor)
bool Sprite::AddQuad(GLuint textureName, Rect& spriteRect, Rect& texRect, glm::vec3 tintColor, Point scale = Point(1,1))
{
    if (m_vertexStreams.size() == 0 || textureName != m_vertexStreams[m_vertexStreams.size() - 1].textureName)
        m_vertexStreams.emplace_back(textureName);

    m_vertexStreams[m_vertexStreams.size() - 1].addQuad(spriteRect, texRect, tintColor, scale);
    //m_vertexStreams[m_vertexStreams.size() - 1].addQuad(spriteRect, quadPos, tintColor);

    return true;
}

//-----------------------------------------------------------------------------
// Name : Render ()
//-----------------------------------------------------------------------------
bool Sprite::Render()
{
    for (StreamOfVertices& vertexStream : m_vertexStreams)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexSprite) * vertexStream.vertices.size(),
                        vertexStream.vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, m_indicesBuffer);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(VertexIndex) * vertexStream.indices.size(),
                        vertexStream.indices.data());

        glBindTexture(GL_TEXTURE_2D, vertexStream.textureName);
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

//-----------------------------------------------------------------------------
// Name : CreateQuad ()
// Desc : Create the vertex and indices data for a new quad
// @param textureName (in)
// @param srcRect (in)
// @param quadPos (in)
// @param tintColor (in)
// @param quadVertices (inout) array to be filled with the vertex data
// @param indices (inout) array to be filled with the indices data
//-----------------------------------------------------------------------------
//void Sprite::CreateQuad(GLuint textureName, Rect& srcRect, Point quadPos, glm::vec3 tintColor,
//                        VertexSprite quadVertices[4], GLuint indices[6])
//{
//    // sainty check
//    if (quadVertices == nullptr || indices == nullptr)
//        return;

//    VertexSprite quadVertices[4];

//    float textureWidth, textureHeight;
//    int texWidth, texHeight;
//    float scaleU, scaleV;
//    float startU, startV;
//    // calculate how much to scale  the UV coordinates of the texture to fit the srcRect
//    if (textureName)
//    {
//        // get texture width and height
//        glGetTextureLevelParameteriv(textureName, 0, GL_TEXTURE_WIDTH, &texWidth);
//        glGetTextureLevelParameteriv(textureName, 0, GL_TEXTURE_HEIGHT, &texWidth);

//        textureWidth  = static_cast<float>( srcRect.right - srcRect.left);
//        textureHeight = static_cast<float>( srcRect.botoom - srcRect.top );

//        scaleU = static_cast<float>( (srcRect.right - srcRect.left) / texWidth );
//        scaleV = static_cast<float>( (srcRect.botoom - srcRect.top) / texHeight );

//        startU = static_cast<float>(srcRect.right - srcRect.left) / static_cast<float>(texWidth);
//        startV = static_cast<float>(srcRect.botoom - srcRect.top / static_cast<float>(texHeight);
//    }
//    else
//    {
//        textureWidth = textureHeight = 0;
//        scaleU = scaleV = 0;
//        startU = startV = 0;
//    }

//    // creates the four vertices of our quad
//    quadVertices[0] = VertexSprite(quadPos.x, quadPos.y, 0, tintColor, startU, startV);
//    quadVertices[1] = VertexSprite(quadPos.x + textureWidth * m_fScaleWidth, quadPos.y, 0, tintColor, startU + 1 * scaleU, startV);
//    quadVertices[2] = VertexSprite(quadPos.x, quadPos.y + textureHeight, 0, tintColor, startU, startV + 1 * scaleV);
//    quadVertices[3] = VertexSprite(quadPos.x + textureHeight * m_fScaleHeight, quadPos.y + textureHeight, 0, tintColor, startU + 1 * scaleU, startV + 1 * scaleV);

//    GLuint indices[6];
//    GLuint vIndex = 0;
//    // triangle 201
//    indices[0] = vIndex + 2; // #2 vertex  0---1 4---5
//    indices[1] = vIndex;     // #0 vertex  -   - -   -
//    indices[2] = vIndex + 1; // #1 vertex  2---3 6---7
//    // triangle 213
//    indices[3] = vIndex + 2; // #2 vertex
//    indices[4] = vIndex + 1; // #1 vertex
//    indices[5] = vIndex + 3; // #3 vertex
//}
