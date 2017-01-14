#include "Font.h"

//-----------------------------------------------------------------------------
// Name : mkFont (constructor)
//-----------------------------------------------------------------------------
mkFont::mkFont()
{
    VAO = 0;
    VBO = 0;
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
// Name : Init
//-----------------------------------------------------------------------------
int mkFont::Init()
{
    // FreeType
     FT_Library ft;
     // All functions return a value different than 0 whenever an error occurred
     if (FT_Init_FreeType(&ft))
         std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

     // Load font as face
     FT_Face face;
     if (FT_New_Face(ft, "/usr/share/fonts/noto/NotoMono-Regular.ttf", 0, &face))
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
     indices[1] = 2;
     indices[2] = 3;
     indices[3] = 0;
     indices[4] = 3;
     indices[5] = 1;

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
     if (FT_New_Face(ft, "/usr/share/fonts/noto/NotoMono-Regular.ttf", 0, &face))
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
// Name : RenderText
//-----------------------------------------------------------------------------
void mkFont::RenderText(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
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
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
// Name : RenderTextBatched
//-----------------------------------------------------------------------------
void mkFont::RenderTextBatched(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
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

    GLuint textureID;

    if (fontTexures_.count(text) == 0)
    {
        GLfloat w = 0;

        GLuint ww = 0;
        GLuint hh = 0;

        for (c = text.begin(); c!= text.end(); c++)
        {
            NewCharacter ch = NewCharacters_[*c];

            if (ch.Bearing.x >= 0)
                ww += abs(ch.Bearing.x);

            ww += ch.Size.x;
            hh += ch.Size.y;

            ww += (ch.Advance >> 6) - ch.Size.x - ch.Bearing.x;
        }

        c = text.begin();
        int sizeY = (NewCharacters_[*c]).Size.y;
        sizeY *= ww;
        unsigned char * buffer = new unsigned char[sizeY];

        int count = 0;
        int j = 0;

        for (c = text.begin(); c != text.end(); c++)
        {
            NewCharacter ch = NewCharacters_[*c];

           int extraWidth = 0;
           if (ch.Bearing.x >= 0)
               extraWidth = abs(ch.Bearing.x);

            int advance = (ch.Advance >> 6) - ch.Bearing.x - ch.Size.x;
            if (ch.Bearing.x < 0)
                j + ch.Bearing.x;

            for (int k = 0; k < extraWidth; k++)
            {
                for (int i = 0; i < ch.Size.y; i++)
                {
                    buffer[ (i * ww) + j] = 0;
                    count++;
                }
                j++;
            }

            for (int l =0; l < ch.Size.x; l++)
            {
                for (int k = 0; k < ch.Size.y; k++)
                {
                    //buffer[ (k * ch.Size.y) + j] = ch.buffer[(k * ch.Size.y) + l];
                    buffer[ (k * ww) + j] = ch.buffer[(k * ch.Size.x) + l];
                    //buffer[ (k * ww) + j] = 0;
                    count++;
                }
                j++;
            }

            for (int l = 0; l < advance; l++)
            {
                for (int i = 0; i < ch.Size.y; i++)
                {
                    buffer[ (i * ww) + j] = 0;
                    count++;
                }
                j++;
            }

            if (ch.Bearing.x >= 0)
                w += abs(ch.Bearing.x) * scale;

            w += ch.Size.x * scale;
            h += ch.Size.y * scale;

            w += ((ch.Advance >> 6) - ch.Size.x - ch.Bearing.x) * scale;
        }

        if (count != sizeY)
            std::cout <<"count and sizeY don't match count: "<<count<<" sizeY: "<<sizeY<<"\n";


        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        c = text.begin();

        NewCharacter cc = NewCharacters_[*c];
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


        FontString ff = {textureID,w};

        fontTexures_.insert(std::pair<std::string,FontString>(text,ff));
        delete[] buffer;

    }

    FontString ff = fontTexures_[text];

    c = text.begin();
    NewCharacter ch = NewCharacters_[*c];

    h = ch.Size.y * scale;
    // Update VBO for the string
    GLfloat vertices[6][4] = {
        { xpos,            ypos + h,   0.0, 0.0 },
        { xpos,            ypos,       0.0, 1.0 },
        { xpos + ff.width, ypos,       1.0, 1.0 },

        { xpos,            ypos + h,   0.0, 0.0 },
        { xpos + ff.width, ypos,       1.0, 1.0 },
        { xpos + ff.width, ypos + h,   1.0, 0.0 }
    };

    glBindTexture(GL_TEXTURE_2D, ff.texName);
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);


    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
