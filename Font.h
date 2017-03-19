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
    GLfloat scale;
};

class mkFont
{
public:
    mkFont(std::__cxx11::string fontName);
    ~mkFont();

    int init();
    int newInit();
    void renderText(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, int height);
    void renderTextBatched(Shader *shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
    std::string getFontPath(std::string fontName);

    static void printallFonts();


private:
    std::string fontPath;
    std::map<GLchar, Character> Characters_;
    std::map<GLchar, NewCharacter> NewCharacters_;
    std::map<std::string, FontString> fontTexures_;
    GLuint VAO,VBO;
    GLushort indices[6];

    bool cacheTextTexutre(std::string text, GLfloat scale);
};

#endif  //_FONT_H
