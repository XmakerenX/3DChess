#include "Font.h"
#include <fontconfig/fontconfig.h>

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
int mkFont::init()
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

//     indices[0] = 0;
//     indices[1] = 1;
//     indices[2] = 2;
//     indices[3] = 0;
//     indices[4] = 3;
//     indices[5] = 2;
     indices[0] = 3;
     indices[1] = 2;
     indices[2] = 1;
     indices[3] = 3;
     indices[4] = 0;
     indices[5] = 1;
//     indices[0] = 0;
//     indices[1] = 2;
//     indices[2] = 3;
//     indices[3] = 0;
//     indices[4] = 1;
//     indices[5] = 2;


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
void mkFont::renderText(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, int height)
{
   // y = y*-1;
    // Activate corresponding render state
    shader->Use();
    glUniform3f(glGetUniformLocation(shader->Program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters_[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        //GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        //GLfloat ypos = 600 - y - ch.Bearing.y * scale;
        //GLfloat ypos = height - y + ch.Bearing.y * scale;
        //GLfloat ypos = y + (ch.Bearing.y) * scale;
        //GLfloat ypos = y + (ch.Size.y) * scale;
        //GLfloat ypos = y - (ch.Size.y - ((ch.Size.y - ch.Bearing.y))) * scale;
        GLfloat ypos;
        if (ch.Size.y - ch.Bearing.y == 0)
            ypos = height - y;
        else
            //ypos = height - y + (ch.Size.y - ch.Bearing.y) * scale;
            ypos = height - y;

        //ypos = 600 - ypos ;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
//        GLfloat vertices[6][4] = {
//            { xpos,     ypos + h,   0.0, 0.0 },
//            { xpos,     ypos,       0.0, 1.0 },
//            { xpos + w, ypos,       1.0, 1.0 },

//            { xpos,     ypos + h,   0.0, 0.0 },
//            { xpos + w, ypos,       1.0, 1.0 },
//            { xpos + w, ypos + h,   1.0, 0.0 }
//        };

//        GLfloat vertices[4][4] = {
//            { xpos,            ypos + h,   0.0, 0.0 },
//            { xpos,            ypos,       0.0, 1.0 },
//            { xpos + w, ypos,       1.0, 1.0 },
//            { xpos + w, ypos + h,   1.0, 0.0 }
//        };
//           GLfloat vertices[4][4] = {
//              { xpos,            600 - (ypos + h) + h,   0.0, 0.0 },
//              { xpos + w,            600 - (ypos + h) + h,       1.0, 0.0 },
//              { xpos + w, 600 - ypos - h,       1.0, 1.0 },
//              { xpos,  600 - ypos - h,   0.0, 1.0 }
//           };

           GLfloat vertices[4][4] = {
              { xpos,            ypos,   0.0, 0.0 },
              { xpos + w,            ypos,       1.0, 0.0 },
              { xpos + w,  ypos - h,       1.0, 1.0 },
              { xpos,  ypos - h,   0.0, 1.0 }
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
    GLfloat ypos = y;
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
    // Update VBO for the string
    GLfloat vertices[4][4] = {
        { xpos,                         ypos + h,   0.0, 0.0 },
        { xpos,                         ypos,       0.0, 1.0 },
        { xpos + ff.width * widthScale, ypos,       1.0, 1.0 },
        { xpos + ff.width * widthScale, ypos + h,   1.0, 0.0 }
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

    // calc the needed texture width
    for (c = text.begin(); c!= text.end(); c++)
    {
        NewCharacter ch = NewCharacters_[*c];

        // add to width num pixels before the char
        if (ch.Bearing.x >= 0)
            ww += abs(ch.Bearing.x);
            //ww += ch.Bearing.x;

        ww += ch.Size.x;
        hh += ch.Size.y;

        // add to width num pixels after the char
        ww += (ch.Advance >> 6) - ch.Size.x - ch.Bearing.x;
    }

    // get character height in pixels
    c = text.begin();
    int sizeY = (NewCharacters_[*c]).Size.y;
    // mul by width and get the needed texture size
    sizeY *= ww;
    unsigned char * buffer = new unsigned char[sizeY];

    int count = 0;
    int j = 0;
    int leftOver = 0;

    // add all the chars in the text to the texture buffer
    for (c = text.begin(); c != text.end(); c++)
    {
        // get char info (glypth , size , spaces)
        NewCharacter ch = NewCharacters_[*c];

        int extraStartPixels = 0;
        int advance = 0;
        int nColumnBlend = 0;
        // set how many pixels are before the char
        // negative value means adding more colums before the char
        if (ch.Bearing.x >= 0)
            extraStartPixels = abs(ch.Bearing.x);
        else
           if (ch.Bearing.x < 0)
           {
               std::cout <<"before j:= " << j << "\n";
               //TODO: check this line this seems so wrong....
               if (j + ch.Bearing.x >= 0)
               {
                  j = j + ch.Bearing.x;
                  leftOver += ch.Bearing.x;
                  nColumnBlend = abs(ch.Bearing.x);
               }
               //advance += abs(ch.Bearing.x);
               std::cout <<"after  j:= " << j << "\n";
           }

        // num of columns to add after the char glyph
        advance += (ch.Advance >> 6) - ch.Bearing.x - ch.Size.x;
        //advance = (ch.Advance >> 6) - ch.Bearing.x - ch.Size.x;


        // add the extra empty column before the char to the buffer
        for (int k = 0; k < extraStartPixels; k++)
        {
            // add a complete empty column
            for (int i = 0; i < ch.Size.y; i++)
            {
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
                if (nColumnBlend == 0 ||  ch.buffer[(k * ch.Size.x) + l] > 20)
                    buffer[ (k * ww) + j] = ch.buffer[(k * ch.Size.x) + l];

                //buffer[ (k * ch.Size.y) + j] = ch.buffer[(k * ch.Size.y) + l];

                //buffer[ (k * ww) + j] = 0;
                count++;
            }

            if (nColumnBlend != 0)
                nColumnBlend--;

            j++;
        }

        // add the needed columns after the char glyph
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
            w += abs(ch.Bearing.x) * scale;

        w += ch.Size.x * scale;
        h += ch.Size.y * scale;

        w += ((ch.Advance >> 6) - ch.Size.x - ch.Bearing.x) * scale;
    }

    NewCharacter ch = NewCharacters_[*(text.begin())];
    // fill with 0 the end of the buffer
    for (int l = leftOver; l < 0; l++)
    {
        for (int i = 0; i < ch.Size.y; i++)
        {
            buffer[ (i * ww) + j] = 0;
            count++;
        }
        j++;
    }


    if (count != sizeY)
        // something went wrong and likely have went over the bounds of the buffer
        // or didn't fill it till the end
        std::cout <<"count and sizeY don't match count: "<<count<<" sizeY: "<<sizeY<<"\n";


    // ask to allocate new texutre
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    c = text.begin();
    NewCharacter cc = NewCharacters_[*c];

    // create the text texture using the created buffer
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        ww,
        cc.Size.y,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        buffer
    );

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // add the new texture to the map
    FontString ff = {textureID, w, scale};
    fontTexures_.insert( std::pair< std::string, FontString>(text,ff));

    delete[] buffer;
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
