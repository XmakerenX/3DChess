#ifndef  _RENDERTYPES_H
#define  _RENDERTYPES_H

#include<string>
#include<vector>
#include<GL/glew.h>
#include<glm/glm.hpp>

typedef unsigned int VertexIndex;

//-----------------------------------------------------------------------------
// Common render types
//-----------------------------------------------------------------------------
struct Point
{
    Point() {}
    Point (int newX, int newY)
    {
        x = newX;
        y = newY;
    }

    Point(const Point& p)
    {
        x = p.x;
        y = p.y;
    }

    int x;
    int y;
};

struct Rect
{
    Rect()
    {
        left = top = right = bottom = 0;
    }

    Rect(int left , int top , int right , int bottom)
    {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }

    Rect(const Rect& copy)
    {
        left = copy.left;
        top = copy.top;
        right = copy.right;
        bottom = copy.bottom;
    }

    void offset(int x , int y)
    {
        left   += x;
        right  += x;
        top    += y;
        bottom += y;
    }

    void offset(Point pt)
    {
        offset(pt.x, pt.y);
    }

    void inflate(int x , int y)
    {
        left -= x;
        right += x;
        top -= y;
        bottom += y;
    }

    void inflate(Point pt)
    {
        inflate(pt.x, pt.y);
    }

    bool isPointInRect(int x, int y) const
    {
        if (x >= left && x <= right && y >= top && y <= bottom)
            return true;
        else
            return false;
    }

    bool isPointInRect(Point pt) const
    {
        return isPointInRect(pt.x, pt.y);
    }

    static Rect intersectRect(const Rect& a, const Rect& b)
    {
        return Rect(std::max(a.left, b.left), std::max(a.top, b.top), std::min(a.right, b.right), std::min(a.bottom, b.bottom) );
    }

    GLuint getWidth() const
    {
        return right - left;
    }

    GLuint getHeight() const
    {
        return bottom - top;
    }

    int left;
    int top;
    int right;
    int bottom;
};

struct Material
{
    glm::vec4 diffuse;
    glm::vec4 ambient;
    glm::vec4 specular; // specular 'shininess'
    glm::vec4 emissive;
    float power;        // Sharpness if specular highlight

    Material()
    {}
    
    Material(const glm::vec4& _diffuse, const glm::vec4& _ambient, const glm::vec4& _specular, const glm::vec4& _emissive, float _power)
        :diffuse(_diffuse), ambient(_ambient), specular(_specular), emissive(_emissive), power(_power)
    {}
    
    inline bool operator==(const Material& mat)
    {
        return (diffuse == mat.diffuse && ambient == mat.ambient && specular == mat.specular &&
            emissive == mat.emissive && power == mat.power);
    }
};

struct Attribute
{
    std::string  texIndex;
    unsigned int matIndex;
    std::string shaderIndex;

    inline bool operator==(const Attribute& atrib)
    {
        return (texIndex == atrib.texIndex && matIndex == atrib.matIndex && shaderIndex == atrib.shaderIndex);
    }
};

//-----------------------------------------------------------------------------
// Common render consts
//-----------------------------------------------------------------------------
const Rect EMPTY_RECT(0,0,0,0);
const glm::vec4 WHITE_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
const Material WHITE_MATERIAL(WHITE_COLOR, WHITE_COLOR, WHITE_COLOR, WHITE_COLOR, 1.0f);
const GLuint NO_TEXTURE = 0;

#endif // _RENDERTYPES_H
