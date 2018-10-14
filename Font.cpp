#include "Font.h"
#include <vector>
#include <algorithm>
#include <cmath>

//-----------------------------------------------------------------------------
// Name : mkFont (constructor)
//-----------------------------------------------------------------------------
mkFont::mkFont()
    : m_cachedTextSize(0,0)
{
    VAO = 0;
    VBO = 0;

    fontPath = "";
    m_fontSize = 0;

    m_textureAtlas = 0;
	m_textureAtlasWidth = 0;
	m_textureAtlasHeight = 0;

    m_cachedMaxBearing = 0;
    m_cachedText = "";
}

//-----------------------------------------------------------------------------
// Name : mkFont (constructor)
//-----------------------------------------------------------------------------
mkFont::mkFont(std::string fontName, bool isPath/* = false*/)
    : m_cachedTextSize(0,0)
{
    VAO = 0;
    VBO = 0;

    if (isPath)
        fontPath = fontName;
    else
        fontPath = mkFont::getFontPath(fontName);
    m_fontSize = 0;

    m_textureAtlas = 0;
	m_textureAtlasWidth = 0;
	m_textureAtlasHeight = 0;
    m_cachedMaxBearing = 0;
    m_cachedText = "";
}

//-----------------------------------------------------------------------------
// Name : mkFont (copy constructor)
//-----------------------------------------------------------------------------
mkFont::mkFont(const mkFont& toCopy)
{
    fontPath = toCopy.fontPath;
    init(toCopy.m_fontSize, 96, 96);
    m_cachedText = "";
}

//-----------------------------------------------------------------------------
// Name : mkFont (move constructor)
//-----------------------------------------------------------------------------
mkFont::mkFont(mkFont&& toMove)
    :charGlyphs(std::move(toMove.charGlyphs)), charGlyphsAtlas(std::move(toMove.charGlyphsAtlas))
{
    fontPath = toMove.fontPath;
    m_fontSize = toMove.m_fontSize;

    this->VAO = toMove.VAO;
    this->VBO = toMove.VBO;

    for (int i = 0; i < 6; i++)
        indices[i] = toMove.indices[i];

    m_maxRows = toMove.m_maxRows;
    m_avgWidth = toMove.m_avgWidth;

    m_textureAtlas = toMove.m_textureAtlas;
	m_textureAtlasWidth = toMove.m_textureAtlasWidth;
	m_textureAtlasHeight = toMove.m_textureAtlasHeight;

    toMove.fontPath = "";
    toMove.m_fontSize = 0;
    toMove.VAO = 0;
    toMove.VBO = 0;

    toMove.m_maxRows = 0;
    toMove.m_avgWidth = 0;
    toMove.m_textureAtlas = 0;
    m_cachedText = "";
}

//-----------------------------------------------------------------------------
// Name : mkFont (destructor)
//-----------------------------------------------------------------------------
mkFont::~mkFont()
{
    // free the cached textures
    // TODO: maybe free these by adding a destructor to the structs...
    for (auto it = charGlyphs.begin(); it != charGlyphs.end(); it++)
    {
        CharGlyph& cg = it->second;
        glDeleteTextures(1,&cg.TextureID);
    }

    if (m_textureAtlas != 0)
        glDeleteTextures(1, &m_textureAtlas);
}

//-----------------------------------------------------------------------------
// Name : init
//-----------------------------------------------------------------------------
int mkFont::init(int fontSize, int hDpi, int vDpi)
{
    this->m_fontSize = fontSize;
     FT_Library ft;
     // All functions return a value different than 0 whenever an error occurred
     if (FT_Init_FreeType(&ft))
     {
         std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
         return -1;
     }

     // Load font as face
     FT_Face face;
     if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
     {
         std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
         return -1;
     }

     // Set size to load glyphs as
     FT_Set_Char_Size(face, fontSize*64, fontSize*64, 96, 96);
     // Disable byte-alignment restriction
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
     // cache glyphs for rednerText
     cacheGlyth(ft, face);
     // create the Font Atlas
     createFontAtlas(ft, face);

     // Destroy FreeType once we're finished
     FT_Done_Face(face);
     FT_Done_FreeType(ft);

     // Configure VAO/VBO for texture quads
     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
     glBindVertexArray(VAO);
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindVertexArray(0);

     // set indices for quad rendering
     // first triangle
     indices[0] = 0;
     indices[1] = 2;
     indices[2] = 1;
     // secound triangle
     indices[3] = 2;
     indices[4] = 3;
     indices[5] = 1;

     return 0;
}

//-----------------------------------------------------------------------------
// Name : renderText
//-----------------------------------------------------------------------------
//TODO: better generalize this function so it can be used for rendering text not only to a screen.
void mkFont::renderText(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    shader->Use();

    glUniform3f(glGetUniformLocation(shader->Program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // find the max height of the text
    std::string::const_iterator c = text.begin();
    CharGlyph ch = charGlyphs[*c];
    float maxCharHeight = ch.Size.y;
    //TODO: cache this crap somehow...
    for (c = text.begin() + 1; c != text.end(); c++)
    {
        CharGlyph& ch = charGlyphs[*c];

        if (ch.Size.y > maxCharHeight)
            maxCharHeight = ch.Size.y;
    }

    // Iterate through all characters
    for (c = text.begin(); c != text.end(); c++)
    {
        CharGlyph ch = charGlyphs[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y + (ch.Size.y - ch.Bearing.y) * scale + (maxCharHeight - ch.Size.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // Update VBO for each character
        GLfloat vertices[4][4] = {
            { xpos,            ypos,   0.0, 0.0 },
            { xpos + w,        ypos,       1.0, 0.0 },
            { xpos, ypos + h,       0.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 1.0 }
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        int err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cout <<"Font: ERROR bitches " << *c << "\n";
            return;
        }

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Be sure to use glBufferSubData and not glBufferData
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Render quad
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, indices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
// Name : renderTextAtlas
//-----------------------------------------------------------------------------
void mkFont::renderTextAtlas(Sprite& sprite, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color)
{
    if (text.empty())
        return;

    // Iterate through all characters
    for (auto c = text.begin(); c != text.end(); c++)
    {
        CharGlyphAtlas charGlyph = charGlyphsAtlas[*c];
        GLfloat xpos = x + charGlyph.Bearing.x * scale;
        GLfloat ypos = y - charGlyph.Bearing.y  * scale;

        GLfloat w = charGlyph.Size.x * scale;
        GLfloat h = charGlyph.Size.y * scale;

		Texture atlasTexture(m_textureAtlas, m_textureAtlasWidth, m_textureAtlasHeight);
        sprite.AddTintedTexturedQuad(Rect(xpos, ypos, xpos + w, ypos + h), color, atlasTexture, charGlyph.textureRect);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        x += (charGlyph.Advance >> 6) * scale;
    }
}

//-----------------------------------------------------------------------------
// Name : calcTextRect
//-----------------------------------------------------------------------------
Point mkFont::calcTextRect(std::string text)
{
    m_cachedText = text;

    int offset = 0;
    int textWidth = 0;
    GLuint maxTextHeight;
    m_cachedMaxBearing = -100;
    int maxUnderLineHeight = -100;
    // calcuate textWidth that fits the Rect
    // calculate text height by finding the max bearing value(above text line height)
    // and max height value below the text line
    for (std::string::iterator c = text.begin(); c != text.end(); c++)
    {
        CharGlyph& ch = charGlyphs[*c];

        textWidth += (ch.Advance >> 6);
        offset = (ch.Advance >> 6) - (ch.Bearing.x + ch.Size.x);

        m_cachedMaxBearing = std::max(ch.Bearing.y, m_cachedMaxBearing);
        if (ch.Bearing.y != 0)
            maxUnderLineHeight = std::max(maxUnderLineHeight, ch.Size.y - ch.Bearing.y);
    }

    // remove the last charachter advance value
    // unless it is space so space will count in the text Width
    if (text.back() != ' ')
        textWidth = textWidth - offset;

    maxTextHeight = m_cachedMaxBearing + maxUnderLineHeight;

    m_cachedTextSize = Point(textWidth, maxTextHeight);
    return m_cachedTextSize;
}

//-----------------------------------------------------------------------------
// Name : clipTextToRect
// Desc : clips the given string to the given rect
// Note : textInRect is updated to the clipped string
//        returns the new text size after clipping
//-----------------------------------------------------------------------------
Point mkFont::clipTextToRect(const Rect& rc, std::string& textInRect, int maxTextHeight)
{
    int textWidth = 0;
    int lastAdvance = 0;
    std::string::iterator c;
    for (c = textInRect.begin(); c != textInRect.end(); c++)
    {
        CharGlyph& ch = charGlyphs[*c];

        textWidth += (ch.Advance >> 6);
        lastAdvance = ch.Advance >> 6;
        int offset = (ch.Advance >> 6) - (ch.Bearing.x + ch.Size.x);

        if ( (textWidth - offset) > rc.getWidth())
        {
            // clip the string so it fits the Rect
            textInRect = std::string(textInRect.begin(), c);
            break;
        }
    }
    //textWidth -= lastAdvance;

    if (maxTextHeight > rc.getHeight())
    {
        textInRect = "";
        return Point(0,0);
    }

    return Point(textWidth, maxTextHeight);
}

//-----------------------------------------------------------------------------
// Name : renderToRect
//-----------------------------------------------------------------------------
void mkFont::renderToRect(Sprite& sprite, std::string text, Rect rc, glm::vec4 color, TextFormat format/* = TextFormat::Left*/, bool clipText/* = true*/)
{
    Point textSize;
    // check if this text rect was already cached
    // by checking if textInRect appeares in the start of text
    if (m_cachedText.empty() || text.compare(0, m_cachedText.size(), m_cachedText) != 0)
        textSize = calcTextRect(text);
    else
        textSize = m_cachedTextSize;

    std::string textInRect = text;
    if (clipText)
    {
        textSize = clipTextToRect(rc, textInRect, textSize.y);
    }
    else
        textInRect = text;

    int x = rc.left;
    int y = rc.top;

    // shift the x posistion based on the given format
    switch(format)
    {
    case TextFormat::Left:
    {
        y = rc.top + m_cachedMaxBearing;
    }break;

    case TextFormat::Center:
    {
        //x = rc.left + (rc.getWidth() - textWidth) / 2;
        x = rc.left + (rc.getWidth() - textSize.x) / 2;
        y = rc.top + rc.getHeight() / 2 + charGlyphsAtlas['c'].Bearing.y / 2;
    }break;

    case TextFormat::HorizCenter:
    {
        x = rc.left + (rc.getWidth() - textSize.x) / 2;
    }break;

    case TextFormat::VerticalCenter:
    {
        y = rc.top + rc.getHeight() / 2 + charGlyphsAtlas['c'].Bearing.y / 2;
    }break;

    case TextFormat::Right:
    {
        x = rc.right - textSize.x;
        y = rc.top + m_cachedMaxBearing;
    }break;

    case TextFormat::RightVerticalCenter:
    {
        x = rc.right - textSize.x;
        y = rc.top + rc.getHeight() / 2 + charGlyphsAtlas['c'].Bearing.y / 2;
    }break;

    }

    renderTextAtlas(sprite,textInRect, x, y, 1.0f, color);
}

//-----------------------------------------------------------------------------
// Name : cacheGlyth
//-----------------------------------------------------------------------------
void mkFont::cacheGlyth(FT_Library ft, FT_Face face)
{
    m_maxRows = 0;
    int sumWidth = 0;

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // Generate the glyph texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        sumWidth += face->glyph->bitmap.width;
        m_maxRows  = std::max(m_maxRows, face->glyph->bitmap.rows);

        //std::cout << (int)c << "! " << c << ": Width" <<face->glyph->bitmap.width << "\n";
        //std::cout << (int)c << "! " << c << ": Height" <<face->glyph->bitmap.rows << "\n";

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        // Now store character for later use
        CharGlyph charGlyph = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        charGlyphs.insert(std::pair<GLchar, CharGlyph>(c, charGlyph));
    }

    // Clear the current texture
    glBindTexture(GL_TEXTURE_2D, 0);

    m_avgWidth = sumWidth / 128;
}

//-----------------------------------------------------------------------------
// Name : CreateFontAtlas
//-----------------------------------------------------------------------------
void mkFont::createFontAtlas(FT_Library ft, FT_Face face)
{
    GLuint i = 6;
    GLuint j = 6;
    while (i <= 11 || j <= 11)
    {
        int bitmapPerWidth = std::pow(2, i) / m_avgWidth;
        int bitmapPerRow = std::pow(2, j) / m_maxRows;

        if ((bitmapPerWidth * bitmapPerRow) > 128)
        {
            std::cout << "texture useage will be " << (128 / (float)(bitmapPerWidth * bitmapPerRow))*100 << "%\n";
            break;
        }
        else
        {
            i++;
            //bitmapPerWidth = std::pow(2, i) / maxWidth;
            bitmapPerWidth = std::pow(2, i) / m_avgWidth;
            if ((bitmapPerWidth * bitmapPerRow) >= 128)
            {
                std::cout << "texture useage will be " << (128 / (float)(bitmapPerWidth * bitmapPerRow))*100 << "%\n";
                break;
            }
            else
            {
                bitmapPerRow = std::pow(2, j) / m_maxRows;
                j++;
            }
        }
    }

    std::cout << "texture size should be " << std::pow(2, i) << "X" << std::pow(2, j) << "\n";

    int textureWidth = std::pow(2, i);
    int textureHeight = std::pow(2, j);

    unsigned char * textureData = new unsigned char[textureWidth * textureHeight];

    for (GLuint i = 0; i < textureWidth*textureHeight; i++)
        textureData[i] = 0;

    int copiedWidth = 0;
    int textureNum = 0;
    int textureWidthOffset = 0;
    int textureHeightOffset = 0;

    int bitmapPerWidth = std::pow(2, i) / m_avgWidth;
    int bitmapPerRow = std::pow(2, j) / m_maxRows;

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        if (copiedWidth + face->glyph->bitmap.width > textureWidth)
        {
            copiedWidth = 0;
            textureWidthOffset = 0;
            textureHeightOffset += this->m_maxRows;
            textureNum = 0;
        }

        int row = 0;
        for (int j = face->glyph->bitmap.rows - 1; j >= 0; j--)
        {
            for (GLuint i = 0; i < face->glyph->bitmap.width; i++)
            {
                textureData[(row + textureHeightOffset)*textureWidth + i + textureWidthOffset] = face->glyph->bitmap.buffer[j*face->glyph->bitmap.width + i];
            }
            row++;
        }

        // Now store character for later use
        CharGlyphAtlas charGlyph = {
            Rect(  textureWidthOffset, textureHeight - (textureHeightOffset + face->glyph->bitmap.rows), textureWidthOffset + face->glyph->bitmap.width, textureHeight - textureHeightOffset),
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        charGlyphsAtlas.insert(std::pair<GLchar, CharGlyphAtlas>(c, charGlyph));

        copiedWidth += face->glyph->bitmap.width;
        textureNum++;
        textureWidthOffset += face->glyph->bitmap.width;
    }

    // Generate the glyph texture
    GLuint textureName;
    glGenTextures(1, &textureName);
    glBindTexture(GL_TEXTURE_2D, textureName);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        textureWidth,
        textureHeight,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        textureData
    );

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Clear the current texture
    glBindTexture(GL_TEXTURE_2D, 0);

    m_textureAtlas = textureName;
	m_textureAtlasWidth = textureWidth;
	m_textureAtlasHeight = textureHeight;
}

//-----------------------------------------------------------------------------
// Name : getFontSize
//-----------------------------------------------------------------------------
GLuint mkFont::getFontSize()
{
    return m_fontSize;
}

//-----------------------------------------------------------------------------
// Name : getFontPath
//-----------------------------------------------------------------------------
std::string mkFont::getFontPath(std::string fontName)
{
	std::string path = "C:/Windows/Fonts/times.ttf";
    return path;
}

//-----------------------------------------------------------------------------
// Name : getFontNameFromPath
//-----------------------------------------------------------------------------
std::string mkFont::getFontNameFromPath(std::string path)
{
    std::size_t found = path.find_last_of("/");
    return path.substr(found+1);
}

//-----------------------------------------------------------------------------
// Name : renderFontAtlas
//-----------------------------------------------------------------------------
GLuint mkFont::renderFontAtlas(Sprite& sprite, const Rect& rc)
{
    CharGlyph ch = charGlyphs['A'];
	Texture atlasTexture(m_textureAtlas, m_textureAtlasWidth, m_textureAtlasHeight);
    sprite.AddTexturedQuad(rc, atlasTexture, Rect(0, 0, 0, 0));
    return ch.TextureID;
}
