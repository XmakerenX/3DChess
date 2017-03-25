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
}

//-----------------------------------------------------------------------------
// Name : mkFont (destructor)
//-----------------------------------------------------------------------------
mkFont::~mkFont()
{
    for (auto it = NewCharacters_.begin(); it != NewCharacters_.end(); it++)
    {
        NewCharacter& ch = it->second;
        delete[] ch.buffer;
        ch.buffer = nullptr;
    }

    for (auto it = fontTexures_.begin(); it != fontTexures_.end(); it++)
    {
        FontString& f = it->second;
        glDeleteTextures(1,&f.texName);
    }
}

//-----------------------------------------------------------------------------
// Name : init
//-----------------------------------------------------------------------------
int mkFont::init(int height)
{
    // FreeType
     FT_Library ft;
     // All functions return a value different than 0 whenever an error occurred
     if (FT_Init_FreeType(&ft))
         std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

     // Load font as face
     FT_Face face;
     if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
     //if (FT_New_Face(ft, "/usr/share/fonts/TTF/LiberationSerif-Regular", 0, &face))
         std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

     // Set size to load glyphs as
     FT_Set_Pixel_Sizes(face, 0, 48);

     // Disable byte-alignment restriction
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

     int max = 0;
     int min = 255;
     int maxHeight = 0;
     // Load first 128 characters of ASCII set
     for (GLubyte c = 0; c < 128; c++)
     {
         // Load character glyph
         if (FT_Load_Char(face, c, FT_LOAD_RENDER))
         {
             std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
             continue;
         }
         // Generate texture
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
         Character character = {
             texture,
             glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
             glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
             face->glyph->advance.x
         };
         //std::cout << "glpyh for " << c << "\n";
         //std::cout << "width:" << face->glyph->bitmap.width << " rows:" <<face->glyph->bitmap.rows <<"\n";
         //std::cout << "left:" << face->glyph->bitmap_left << " top:"<< face->glyph->bitmap_top << "\n";
         if ( max < (character.Size.y - character.Bearing.y) )
             max = character.Size.y - character.Bearing.y;
         if ( min > (character.Size.y - character.Bearing.y) )
             min = character.Size.y - character.Bearing.y;
         if (maxHeight < character.Size.y)
             maxHeight = character.Size.y;
         Characters_.insert(std::pair<GLchar, Character>(c, character));
     }

     std::cout << "max:" << max << "\n";
     std::cout << "min:" << min << "\n";
     //std::cout << "max Height:" << maxHeight << "\n";

     glBindTexture(GL_TEXTURE_2D, 0);
     // Destroy FreeType once we're finished
     FT_Done_Face(face);
     FT_Done_FreeType(ft);

     // Configure VAO/VBO for texture quads
     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
     glBindVertexArray(VAO);
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindVertexArray(0);

     indices[0] = 0;
     indices[1] = 1;
     indices[2] = 2;
     indices[3] = 0;
     indices[4] = 3;
     indices[5] = 2;

     setScreenHeight(height);

     return 0;
}

//-----------------------------------------------------------------------------
// Name : newInit
//-----------------------------------------------------------------------------
int mkFont::newInit()
{
    // FreeType
     FT_Library ft;
     // All functions return a value different than 0 whenever an error occurred
     if (FT_Init_FreeType(&ft))
         std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

     // Load font as face
     FT_Face face;
     if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
     //if (FT_New_Face(ft, "/usr/share/fonts/TTF/LiberationSerif-Regular", 0, &face))
         std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

     // Set size to load glyphs as
     FT_Set_Pixel_Sizes(face, 0, 48);

     // Disable byte-alignment restriction
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

     int max = 0;
     int min = 255;
     int maxHeight = 0;
     int maxOffset = 0;
     int newHeight;
     // Load first 128 characters of ASCII set
     for (GLubyte c = 0; c < 128; c++)
     {
         // Load character glyph
         if (FT_Load_Char(face, c, FT_LOAD_RENDER))
         {
             std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
             continue;
         }
         int bearing = face->glyph->bitmap.rows - face->glyph->bitmap_top;
         if (bearing < 0)
             bearing = bearing *-1;

         int offset = face->glyph->bitmap.rows - face->glyph->bitmap_top;
         if (maxOffset < offset )
             maxOffset = offset;

         if (maxHeight < face->glyph->bitmap.rows + bearing + offset )
             maxHeight = face->glyph->bitmap.rows + bearing + offset;
     }


     std::cout << "max Height:" << maxHeight << "\n";
     newHeight = maxHeight + maxOffset;

     for (GLubyte c = 0; c < 128; c++)
     {
         // Load character glyph
         if (FT_Load_Char(face, c, FT_LOAD_RENDER))
         {
             std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
             continue;
         }

         NewCharacter character = {
             glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
             glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
             face->glyph->advance.x,
             new unsigned char[face->glyph->bitmap.width * newHeight]
         };

         int bearing = character.Size.y - character.Bearing.y;
         //int rowsUp = maxHeight - face->glyph->bitmap.rows - std::abs(bearing) - (face->glyph->bitmap.rows - face->glyph->bitmap_top);
         int rowsUp = maxHeight - face->glyph->bitmap.rows - std::abs(bearing) - (maxOffset - (face->glyph->bitmap.rows - face->glyph->bitmap_top));
         //int rowsDown = maxOffset - face->glyph->bitmap.rows - face->glyph->bitmap_top;
         int rowsDown = maxOffset - (face->glyph->bitmap.rows - face->glyph->bitmap_top);

         if (bearing > 0)
             rowsUp   += bearing;
         else
             rowsDown += bearing * -1;

         //rowsUp += face->glyph->bitmap.rows - face->glyph->bitmap_top;

         int i = 0;

         for (i =0; i < rowsUp; i++)
         {
             for (int j = 0; j < face->glyph->bitmap.width; j++)
             {
                 character.buffer[ (i * face->glyph->bitmap.width) + j] = 0;
             }
         }

         for (int k =0; k < face->glyph->bitmap.rows; k++)
         {
             for (int j = 0; j < face->glyph->bitmap.width; j++)
             {
                 character.buffer[ (i * face->glyph->bitmap.width) + j] = face->glyph->bitmap.buffer[(k * face->glyph->bitmap.width) + j];
             }
             i++;
         }


         for (int k = 0; k < rowsDown; k++)
         {
             for (int j = 0; j < face->glyph->bitmap.width; j++)
             {
                 character.buffer[ (i * face->glyph->bitmap.width) + j] = 0;
             }
             i++;
         }
         //std::cout <<"c: "<<(int)c<<"i:" << i<<"\n";
         character.Size.y = newHeight;
         NewCharacters_.insert(std::pair<GLchar, NewCharacter>(c, character));
     }

     // Destroy FreeType once we're finished
     FT_Done_Face(face);
     FT_Done_FreeType(ft);

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
    Character ch = Characters_[*c];
    float maxCharHeight = ch.Size.y;
    //TODO: cache this crap somehow...
    for (c = text.begin() + 1; c != text.end(); c++)
    {
        Character ch = Characters_[*c];

        if (ch.Size.y > maxCharHeight)
            maxCharHeight = ch.Size.y;
    }

    //maxCharHeight = maxCharHeight * scale;

    // Iterate through all characters
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters_[*c];

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
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, indices);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
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
    GLfloat ypos = height_ - y;
    GLfloat h = 0;

    // check if the given text is cahced in the map
    if (fontTexures_.count(text) == 0)
    {
        cacheTextTexutre(text,scale);
    }

    FontString ff = fontTexures_[text];
    GLfloat widthScale = scale / ff.scale;

    c = text.begin();
    NewCharacter ch = NewCharacters_[*c];

    h = ch.Size.y * scale;
    h = ff.height * scale;
    // Update VBO for the string
//    GLfloat vertices[4][4] = {
//        { xpos,                         ypos + h,   0.0, 0.0 },
//        { xpos,                         ypos,       0.0, 1.0 },
//        { xpos + ff.width * widthScale, ypos,       1.0, 1.0 },
//        { xpos + ff.width * widthScale, ypos + h,   1.0, 0.0 }
//    };

    GLfloat vertices[4][4] = {
        { xpos,                         ypos - h,   0.0, 1.0 },
        { xpos,                         ypos,       0.0, 0.0 },
        { xpos + ff.width * widthScale, ypos,       1.0, 0.0 },
        { xpos + ff.width * widthScale, ypos - h,   1.0, 1.0 }
    };


    glBindTexture(GL_TEXTURE_2D, ff.texName);
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Render quad
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, indices);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
// Name : cacheTextTexutre
//-----------------------------------------------------------------------------
bool mkFont::cacheTextTexutre(std::string text, GLfloat scale)
{
    std::string::const_iterator c;
    GLfloat w = 0;
    GLuint ww = 0;
    GLuint hh = 0;
    GLuint h = 0;
    GLfloat h_2 = 0;

    // calc the needed texture width
    for (c = text.begin(); c!= text.end(); c++)
    {
        NewCharacter ch = NewCharacters_[*c];

        // add to width num pixels before the char
        if (ch.Bearing.x >= 0)
            ww += std::abs(ch.Bearing.x);
            //ww += ch.Bearing.x;

        ww += ch.Size.x;
        hh += ch.Size.y;

        // add to width num pixels after the char
        ww += (ch.Advance >> 6) - ch.Size.x - ch.Bearing.x;
    }

    // get character height which is the texture height
    c = text.begin();
    int sizeY = (NewCharacters_[*c]).Size.y;
    // allocate the texture buffer
    unsigned char * buffer = new unsigned char[sizeY * ww];

    int count = 0;
    // how many empty column are left in the end of texture
    int leftOver = 0;
    int j = 0;

    // add all the chars in the text to the texture buffer
    for (c = text.begin(); c != text.end(); c++)
    {
        // get char info (glypth , size , spaces)
        NewCharacter ch = NewCharacters_[*c];

        // number of empty column at the start of texture
        int extraStartPixels = 0;
        int advance = 0;
        // number of columns which are overlapping with previous char
        int nColumnBlend = 0;
        // set how many pixels are before the char
        // negative value means adding more columns before the char
        if (ch.Bearing.x >= 0)
            extraStartPixels = std::abs(ch.Bearing.x);
        else
           if (ch.Bearing.x < 0)
           {
               std::cout <<"before j:= " << j << "\n";
               // check if going back x columns if we are still in texture bounds
               if (j + ch.Bearing.x >= 0)
               {
                  // move back x columns
                  j = j + ch.Bearing.x;
                  leftOver += ch.Bearing.x;
                  nColumnBlend = std::abs(ch.Bearing.x);
               }
               //advance += std::abs(ch.Bearing.x);
               std::cout <<"after  j:= " << j << "\n";
           }

        // number of columns to add after the char glyph
        advance += (ch.Advance >> 6) - ch.Bearing.x - ch.Size.x;


        // add the extra empty columns before the char to the buffer
        for (int k = 0; k < extraStartPixels; k++)
        {
            // add a complete empty column
            for (int i = 0; i < ch.Size.y; i++)
            {
                // only clear columns which are not overlapping
                if (nColumnBlend == 0)
                    buffer[ (i * ww) + j] = 0;

                count++;
            }

            if (nColumnBlend != 0)
                nColumnBlend--;

            j++;
        }

        // add the char glypth to the buffer
        for (int l =0; l < ch.Size.x; l++)
        {
            for (int k = 0; k < ch.Size.y; k++)
            {
                // change value in buffer only if is not overlapping pervious value
                // or color in buffer is negligible( less than 20)
                if (nColumnBlend == 0 ||  ch.buffer[(k * ch.Size.x) + l] > 20)
                    buffer[ (k * ww) + j] = ch.buffer[(k * ch.Size.x) + l];

                count++;
            }

            if (nColumnBlend != 0)
                nColumnBlend--;

            j++;
        }

        // add the needed empty columns after the char glyph
        for (int l = 0; l < advance; l++)
        {
            for (int i = 0; i < ch.Size.y; i++)
            {
                buffer[ (i * ww) + j] = 0;
                count++;
            }
            j++;
        }

        // calc the width of the text rect using the given scale
        if (ch.Bearing.x >= 0)
            w += std::abs(ch.Bearing.x) * scale;

        w += ch.Size.x * scale;
        h += ch.Size.y * scale;

        w += ((ch.Advance >> 6) - ch.Size.x - ch.Bearing.x) * scale;
    } // end of adding all char to texture

    // get char height
    NewCharacter ch = NewCharacters_[*(text.begin())];
    // clear the unused columns at the end of the buffer
    for (int l = leftOver; l < 0; l++)
    {
        for (int i = 0; i < ch.Size.y; i++)
        {
            buffer[ (i * ww) + j] = 0;
            count++;
        }
        j++;
    }

    // sainty check
    // TODO: currently check is always failing so either fix or remove it...
    if (count != sizeY)
        // something went wrong and likely have went over the bounds of the buffer
        // or didn't fill it till the end
        std::cout <<"count and sizeY don't match count: "<<count<<" sizeY: "<<sizeY<<"\n";


    c = text.begin();
    NewCharacter cc = NewCharacters_[*c];


    // trim all the empty rows in the buffer
    bool empty = true;
    // index of rows that aren't completly empty
    std::vector<int> linesToCopy;
    for (int i = 0; i < cc.Size.y; i++)
        for (int j = 0; j < ww; j++)
        {
            // if row isn't empty add it index and move to next row
            if (buffer[(i * ww) + j] != 0)
            {
                empty = false;
                linesToCopy.push_back(i);
                break;
            }
        }

    // calc size of new buffer trimed buffer
    int Sizex = (linesToCopy.size())*ww;
    unsigned char * newBuffer = new unsigned char[(linesToCopy.size())*ww + 12*ww];

    // add 12 empty rows at the head of the buffer
    // text looks better this way and gives exact same results as the regular renderText()
    int i = 0;
    for ( i = 0; i < 12; i++)
        for (int j = 0; j < ww; j++)
        {
            newBuffer[(i * ww) + j] = 0;
        }

    // copy the non empty rows in the original buffer
    for ( int k = 0; k < linesToCopy.size(); k++)
    {
        for (int j = 0; j < ww; j++)
        {
            newBuffer[(i * ww) + j] = buffer[(linesToCopy[k] * ww) + j];
        }
        i++;
    }

    // calc the new height of the texutre
    int newSizeY = linesToCopy.size() + 12;
    h_2 = newSizeY;

    // ask to allocate new texutre
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    c = text.begin();
    cc = NewCharacters_[*c];

    // create the text texture using the created buffer
//    glTexImage2D(
//        GL_TEXTURE_2D,
//        0,
//        GL_RED,
//        ww,
//        cc.Size.y,
//        0,
//        GL_RED,
//        GL_UNSIGNED_BYTE,
//        buffer
//    );

    // create the text mono texture using the the trimmed buffer
//    glTexImage2D(
//        GL_TEXTURE_2D,
//        0,
//        GL_RED,
//        w,
//        h_2,
//        0,
//        GL_RED,
//        GL_UNSIGNED_BYTE,
//        newBuffer
//    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        ww,
        h_2,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        newBuffer
    );

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // cache the texture generated in the map
    FontString ff = {textureID, w,h_2, scale};
    fontTexures_.insert( std::pair< std::string, FontString>(text,ff));

    // free the buffers
    delete[] newBuffer;
    delete[] buffer;
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
//std::string mkFont::getFontPath(std::string fontName)
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
