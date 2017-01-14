#ifndef  _FONT_H
#define  _FONT_H

#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Shader.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint     TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    GLuint     Advance;     // Horizontal offset to advance to next glyph
};

struct NewCharacter {
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    GLuint     Advance;     // Horizontal offset to advance to next glyph
    unsigned char* buffer;
};

struct FontString {
    GLuint texName;
    GLfloat width;
};

class mkFont
{
public:
    mkFont();
    ~mkFont();

    int Init();
    int newInit();
    void RenderText(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
    void RenderTextBatched(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);


private:
    std::map<GLchar, Character> Characters_;
    std::map<GLchar, NewCharacter> NewCharacters_;
    std::map<std::string, FontString> fontTexures_;
    GLuint VAO,VBO;
    GLuint indices[6];

};

#endif  //_FONT_H
