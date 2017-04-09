#include "Font.h"
#include <fontconfig/fontconfig.h>
#include <vector>

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
int mkFont::init(int height)
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
     FT_Set_Pixel_Sizes(face, 0, 48);

     // Disable byte-alignment restriction
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

     // cache glyphs for rednerText
     cacheGlyth(ft, face);
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
     glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4 + 1, NULL, GL_DYNAMIC_DRAW);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindVertexArray(0);

     // set indices for quad rendering
     // first triangle
     indices[0] = 0;
     indices[1] = 1;
     indices[2] = 2;
     // secound triangle
     indices[3] = 0;
     indices[4] = 3;
     indices[5] = 2;

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
        CharGlyph ch = charGlyphs[*c];

        if (ch.Size.y > maxCharHeight)
            maxCharHeight = ch.Size.y;
    }

    // Iterate through all characters
    for (c = text.begin(); c != text.end(); c++)
    {
        CharGlyph ch = charGlyphs[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = height_ - y - (ch.Size.y - ch.Bearing.y)*scale - (maxCharHeight - ch.Size.y)*scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[4][4] = {
            { xpos,            ypos - h,   0.0, 1.0 },
            { xpos,            ypos,       0.0, 0.0 },
            { xpos + w, ypos,       1.0, 0.0 },
            { xpos + w, ypos - h,   1.0, 1.0 }
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Be sure to use glBufferSubData and not glBufferData
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Render quad
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, indices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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
    // TODO: test better maxOffset_ * scale work
    GLfloat ypos = height_ - y - maxOffset_ * scale;
    GLfloat h = 0;

    // check if the given text is cahced in the map
    if (stringTextures.count(text) == 0)
    {
        cacheTextTexutre(text);
    }

    FontString ff = stringTextures[text];

    c = text.begin();
    //NewCharacter ch = NewCharacters_[*c];
    CharGlyphBat ch = charGlyphsBat[*c];

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
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, indices);

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
        CharGlyphBat ch = charGlyphsBat[*c];

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
    //unsigned char * buffer = new unsigned char[bufferHeight * texWidth];
    unsigned char * buffer;

    buffer = monBuffer.getBuffer();

    int count = 0;
    // how many empty column are left in the end of texture
    int leftOver = 0;
    int j = 0;

    // add all the chars in the text to the texture buffer
    for (c = text.begin(); c != text.end(); c++)
    {
        // get char info (glypth , size , spaces)
        //NewCharacter ch = NewCharacters_[*c];
        CharGlyphBat ch = charGlyphsBat[*c];

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
               std::cout <<"before j:= " << j << "\n";
               // check if going back x columns if we are still in texture bounds
               if (j + ch.Bearing_.x >= 0)
               {
                  // move back x columns
                  j = j + ch.Bearing_.x;
                  leftOver += ch.Bearing_.x;
                  nColumnBlend = std::abs(ch.Bearing_.x);
               }
               //advance += std::abs(ch.Bearing.x);
               std::cout <<"after  j:= " << j << "\n";
           }

        // number of columns to add after the char glyph
        advance += (ch.Advance_ >> 6) - ch.Bearing_.x - ch.width_;

        // add the extra empty columns before the char to the buffer
        monBuffer.copyBufferVert(extraStartPixels, j, emptyBuf, nColumnBlend);

        // add the char glypth to the buffer]
        //monoBuffer temp(ch.buffer,ch.Size.x,ch.Size.y);
        //monBuffer.copyBufferVert(ch.Size.x,j, temp, nColumnBlend);
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
