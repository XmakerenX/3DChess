#include "Font.h"
#include <fontconfig/fontconfig.h>
#include <vector>
#include <algorithm>
#include <cmath>

//-----------------------------------------------------------------------------
// Name : mkFont (constructor)
//-----------------------------------------------------------------------------
mkFont::mkFont(std::string fontName)
{
    VAO = 0;
    VBO = 0;

    fontPath = getFontPath(fontName);

    maxHeight_ = 0;
    maxOffset_ = 0;

    textureAtlas = 0;
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

    for (auto it = stringTextures.begin(); it != stringTextures.end(); it++)
    {
        FontString& f = it->second;
        glDeleteTextures(1,&f.texID);
    }
}

//-----------------------------------------------------------------------------
// Name : init
//-----------------------------------------------------------------------------
int mkFont::init(int fontSize,int height, int hDpi, int vDpi)
{
     FT_Library ft;
     // All functions return a value different than 0 whenever an error occurred
     if (FT_Init_FreeType(&ft))
         std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

     // Load font as face
     FT_Face face;
     if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
         std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

     // Set size to load glyphs as
     //FT_Set_Char_Size(face, 72*64, 72*64, hDpi, vDpi);
     FT_Set_Char_Size(face, fontSize*64, fontSize*64, 96, 96);
     //FT_Set_Pixel_Sizes(face, 0, 48);

     // Disable byte-alignment restriction
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

     // cache glyphs for rednerText
     cacheGlyth(ft, face);

     createFontAtlas(ft, face);

     // cache glyphs for renderTextBatch
     cacheGlythBatched(ft, face, maxHeight_, maxOffset_);

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

     setScreenHeight(height);

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
    std::string::const_iterator c = text.begin();
    CharGlyph ch = charGlyphs[*c];
    float maxCharHeight = ch.Size.y;

    for (c = text.begin() + 1; c != text.end(); c++)
    {
        CharGlyph& ch = charGlyphs[*c];

        if (ch.Size.y > maxCharHeight)
            maxCharHeight = ch.Size.y;
    }

    // Iterate through all characters
    for (auto c = text.begin(); c != text.end(); c++)
    {
        CharGlyphAtlas charGlyph = charGlyphsAtlas[*c];
        GLfloat xpos = x + charGlyph.Bearing.x * scale;
        GLfloat ypos = y + (charGlyph.Size.y - charGlyph.Bearing.y) * scale + (maxCharHeight - charGlyph.Size.y)*scale;

        GLfloat w = charGlyph.Size.x * scale;
        GLfloat h = charGlyph.Size.y * scale;

        //sprite.AddTexturedQuad(Rect(xpos, ypos, xpos + w, ypos + h), textureAtlas, charGlyph.textureRect);
        sprite.AddTintedTexturedQuad(Rect(xpos, ypos, xpos + w, ypos + h), color, textureAtlas, charGlyph.textureRect);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        x += (charGlyph.Advance >> 6) * scale;
    }
}

//-----------------------------------------------------------------------------
// Name : renderToRect
//-----------------------------------------------------------------------------
void mkFont::renderToRect(Sprite& sprite, std::string text, Rect rc, glm::vec4 color)
{
    std::string textInRect;
    int textWidth = 0;
    int maxCharHeight = 0;

    std::string::iterator c;

    for (c = text.begin(); c != text.end(); c++)
    {
        CharGlyph& ch = charGlyphs[*c];

        textWidth += (ch.Advance >> 6);
        int offset = (ch.Advance >> 6) - (ch.Bearing.x + ch.Size.x);
        if ( (textWidth - offset) > rc.getWidth())
        {
            textInRect = std::string(text.begin(), c);
            break;
        }

        maxCharHeight = std::max(ch.Size.y, maxCharHeight);
    }

    for (auto c2 = text.begin(); c2 != c; c2++)
    {
        CharGlyph& ch = charGlyphs[*c2];
        GLfloat ypos = rc.top + (ch.Size.y - ch.Bearing.y) + (maxCharHeight - ch.Size.y);
        if ( (ypos + ch.Size.y) - rc.top > rc.getHeight() )
            return;
    }

    if ( c == text.end())
        textInRect = text;

    renderTextAtlas(sprite,textInRect, rc.left, rc.top, 1.0f, color );

}

//-----------------------------------------------------------------------------
// Name : renderTextBatched
//-----------------------------------------------------------------------------
void mkFont::renderTextBatched(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    shader->Use();
    glUniform3f(glGetUniformLocation(shader->Program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    GLfloat xpos = x;
    // TODO: test better  that maxOffset_ * scale works...
    GLfloat ypos = height_ - y - maxOffset_ * scale;
    GLfloat h = 0;

    // check if the given text is cahced in the map
    if (stringTextures.count(text) == 0)
    {
        cacheTextTexutre(text);
    }

    FontString& ff = stringTextures[text];

    h = ff.height * scale;

    // Update VBO for the string
    GLfloat vertices[4][4] = {
        { xpos,                         ypos - h,   0.0, 1.0 },
        { xpos,                         ypos,       0.0, 0.0 },
        { xpos + ff.width * scale, ypos,       1.0, 0.0 },
        { xpos + ff.width * scale, ypos - h,   1.0, 1.0 }
    };


    glBindTexture(GL_TEXTURE_2D, ff.texID);
    // Update content of VBO memory
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Be sure to use glBufferSubData and not glBufferData
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    // Render quad
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, indices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
// Name : cacheTextTexutre
//-----------------------------------------------------------------------------
bool mkFont::cacheTextTexutre(std::string text)
{
    std::string::const_iterator c;
    GLuint texWidth = 0;
    GLuint texHeight = 0;

    monoBuffer emptyBuf;

    // calc the needed texture width
    for (c = text.begin(); c!= text.end(); c++)
    {
        //NewCharacter ch = NewCharacters_[*c];
        CharGlyphBat& ch = charGlyphsBat[*c];

        // add to width num pixels before the char
        if (ch.Bearing_.x >= 0)
            texWidth += std::abs(ch.Bearing_.x);

        texWidth += ch.width_;

        // add to width num pixels after the char
        texWidth += (ch.Advance_ >> 6) - ch.width_ - ch.Bearing_.x;
    }

    // get character height which is the texture height
    c = text.begin();
    int bufferHeight = (charGlyphsBat[*c]).height_;
    // allocate the texture buffer
    monoBuffer monBuffer;
    monBuffer.allocBuffer(texWidth, bufferHeight);

    int count = 0;
    // how many empty column are left in the end of texture
    int leftOver = 0;
    int j = 0;

    // add all the chars in the text to the texture buffer
    for (c = text.begin(); c != text.end(); c++)
    {
        // get char info (glypth , size , spaces)
        //NewCharacter ch = NewCharacters_[*c];
        CharGlyphBat& ch = charGlyphsBat[*c];

        // number of empty column at the start of texture
        int extraStartPixels = 0;
        int advance = 0;
        // number of columns which are overlapping with previous char
        int nColumnBlend = 0;
        // set how many pixels are before the char
        // negative value means adding more columns before the char
        if (ch.Bearing_.x >= 0)
            extraStartPixels = std::abs(ch.Bearing_.x);
        else
           if (ch.Bearing_.x < 0)
           {
               // check if going back x columns if we are still in texture bounds
               if (j + ch.Bearing_.x >= 0)
               {
                  // move back x columns
                  j = j + ch.Bearing_.x;
                  leftOver += std::abs(ch.Bearing_.x);
                  nColumnBlend = std::abs(ch.Bearing_.x);
               }
           }

        // number of columns to add after the char glyph
        advance += (ch.Advance_ >> 6) - ch.Bearing_.x - ch.width_;

        // add the extra empty columns before the char to the buffer
        monBuffer.copyBufferVert(extraStartPixels, j, emptyBuf, nColumnBlend);

        // add the char glypth to the buffer
        monBuffer.copyBufferVert(ch.width_,j, ch, nColumnBlend);
        // add the needed empty columns after the char glyph
        monBuffer.copyBufferVert(advance,j,emptyBuf,nColumnBlend);

        count++;
    } // end for of adding all char to texture

    int nColumnBlend = 0;
    // clear the unused columns at the end of the buffer
    monBuffer.copyBufferVert(leftOver,j,emptyBuf,nColumnBlend);

    // trim all the empty rows in the buffer
    monBuffer.trimEmptyLines();

    // ask to allocate new texutre
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // craete the texture from the buffer
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        monBuffer.width_,
        monBuffer.height_,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        monBuffer.getBuffer()
    );

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // cache the texture generated in the map
    FontString ff = {textureID, monBuffer.width_,monBuffer.height_};
    stringTextures.insert( std::pair< std::string, FontString>(text,ff));
}

//-----------------------------------------------------------------------------
// Name : cacheGlyth
//-----------------------------------------------------------------------------
void mkFont::cacheGlyth(FT_Library ft, FT_Face face)
{
    maxHeight_ = 0;
    maxOffset_ = 0;

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
        maxWidth = std::max(maxWidth, face->glyph->bitmap.width);
        maxRows  = std::max(maxRows, face->glyph->bitmap.rows);

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

        // find maxOffset and maxHeight for cacheGlyphBatched
        int bearing = face->glyph->bitmap.rows - face->glyph->bitmap_top;
        if (bearing < 0)
            bearing = bearing *-1;

        int offset = face->glyph->bitmap.rows - face->glyph->bitmap_top;
        if (maxOffset_ < offset )
            maxOffset_ = offset;

        if (maxHeight_ < face->glyph->bitmap.rows + bearing + offset )
            maxHeight_ = face->glyph->bitmap.rows + bearing + offset;
    }

    // Clear the current texture
    glBindTexture(GL_TEXTURE_2D, 0);

    avgWidth = sumWidth / 128;

//    GLuint i = 6;
//    GLuint j = 6;
//    //for (i = 8; i <= 11; i++)
//    while (i <= 11 || j <= 11)
//    {
//        int bitmapPerWidth = std::pow(2, i) / maxWidth;
//        int bitmapPerRow = std::pow(2, j) / maxRows;

//        if ((bitmapPerWidth * bitmapPerRow) >= 128)
//        {
//            std::cout << "texture useage will be " << (128 / (float)(bitmapPerWidth * bitmapPerRow))*100 << "%\n";
//            break;
//        }
//        else
//        {
//            i++;
//            bitmapPerWidth = std::pow(2, i) / maxWidth;
//            if ((bitmapPerWidth * bitmapPerRow) >= 128)
//            {
//                std::cout << "texture useage will be " << (128 / (float)(bitmapPerWidth * bitmapPerRow))*100 << "%\n";
//                break;
//            }
//            else
//            {
//                bitmapPerRow = std::pow(2, j) / maxRows;
//                j++;
//            }
//        }
//    }

//    std::cout << "texture size should be " << std::pow(2, i) << "X" << std::pow(2, j) << "\n";
}

//-----------------------------------------------------------------------------
// Name : cacheGlythBatched
//-----------------------------------------------------------------------------
void mkFont::cacheGlythBatched(FT_Library ft, FT_Face face, int maxHeight, int maxOffset)
{
    int newCharHeight = maxHeight + maxOffset;

    int count = 0;
    for (GLubyte c = 0; c < 128; c++)
    {
        count++;

        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        monoBuffer emptyBuf;
        // init character glyph
        CharGlyphBat character = {
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        //TODO: think of ignoring when glyph has zero width or height
//         if (face->glyph->bitmap.width == 0)
//             continue;

        character.allocBuffer(face->glyph->bitmap.width, newCharHeight);

        int bearing = face->glyph->bitmap.rows - character.Bearing_.y;
        int rowsUp = newCharHeight - face->glyph->bitmap.rows - std::abs(bearing) - (maxOffset - (face->glyph->bitmap.rows - face->glyph->bitmap_top));
        int rowsDown = maxOffset - (face->glyph->bitmap.rows - face->glyph->bitmap_top);

        if (bearing > 0)
            rowsUp   += bearing;
        else
            rowsDown += bearing * -1;

        int i = 0;
        // add rows to the top of the glyph texture
        character.copyBufferHoriz(rowsUp, i, emptyBuf);
        // copy all the rows of the glyph
        monoBuffer glyphBuffer(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows);
        character.copyBufferHoriz(face->glyph->bitmap.rows, i, glyphBuffer);
        // add rows to the end of the glyph texture
        character.copyBufferHoriz(rowsDown, i, emptyBuf);

        // Cache the glyph for the current character
        charGlyphsBat.insert(std::pair<GLchar, CharGlyphBat>(c, std::move(character)));
    }
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
        //int bitmapPerWidth = std::pow(2, i) / maxWidth;
        int bitmapPerWidth = std::pow(2, i) / avgWidth;
        int bitmapPerRow = std::pow(2, j) / maxRows;

        if ((bitmapPerWidth * bitmapPerRow) >= 128)
        {
            std::cout << "texture useage will be " << (128 / (float)(bitmapPerWidth * bitmapPerRow))*100 << "%\n";
            break;
        }
        else
        {
            i++;
            //bitmapPerWidth = std::pow(2, i) / maxWidth;
            bitmapPerWidth = std::pow(2, i) / avgWidth;
            if ((bitmapPerWidth * bitmapPerRow) >= 128)
            {
                std::cout << "texture useage will be " << (128 / (float)(bitmapPerWidth * bitmapPerRow))*100 << "%\n";
                break;
            }
            else
            {
                bitmapPerRow = std::pow(2, j) / maxRows;
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

    int bitmapPerWidth = std::pow(2, i) / maxWidth;
    int bitmapPerRow = std::pow(2, j) / maxRows;

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
            textureHeightOffset += this->maxRows;
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
//        CharGlyphAtlas charGlyph = {
//            Rect(textureWidthOffset, textureHeightOffset, textureWidthOffset + face->glyph->bitmap.width, textureHeight - (textureHeightOffset + face->glyph->bitmap.rows)),
//            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
//            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
//            face->glyph->advance.x
//        };

        CharGlyphAtlas charGlyph = {
            Rect(  textureWidthOffset, textureHeight - (textureHeightOffset + face->glyph->bitmap.rows), textureWidthOffset + face->glyph->bitmap.width, textureHeight - textureHeightOffset),
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        charGlyphsAtlas.insert(std::pair<GLchar, CharGlyphAtlas>(c, charGlyph));

        copiedWidth += face->glyph->bitmap.width;
        textureNum++;
//        if (textureNum >= bitmapPerWidth)
//        {
//            textureWidthOffset = 0;
//            textureHeightOffset += this->maxRows;
//            textureNum = 0;
//        }
//        else
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

    textureAtlas = textureName;
}

//-----------------------------------------------------------------------------
// Name : setScreenHeight
//-----------------------------------------------------------------------------
void mkFont::setScreenHeight(int height)
{
    height_ = height;
}

//-----------------------------------------------------------------------------
// Name : getFontPath
//-----------------------------------------------------------------------------
std::string mkFont::getFontPath(std::string fontName)
{
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcResult result;
    // configure the search pattern,
    // assume "name" is a std::string with the desired font name in it
    FcPattern* pat = FcNameParse((const FcChar8*)(fontName.c_str()));
    FcConfigSubstitute(config, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    // find the font
    std::string path;
    FcPattern* font = FcFontMatch(config, pat, &result);
    if (font)
    {
       FcChar8* file = NULL;
       if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
       {
          // save the file to another std::string
          path = (char*)file;
       }
       FcPatternDestroy(font);
    }

    FcPatternDestroy(pat);
    return path;
}

//-----------------------------------------------------------------------------
// Name : renderFontAtlas
//-----------------------------------------------------------------------------
GLuint mkFont::renderFontAtlas(Sprite& sprite)
{
    CharGlyph ch = charGlyphs['A'];
    //sprite.AddTexturedQuad(Rect(0,0, 1024, 1024), ch.TextureID, Rect(0,0,0,0));
    sprite.AddTexturedQuad(Rect(200, 0, 712, 512), textureAtlas, Rect(0, 0, 0, 0));
    return ch.TextureID;
}
