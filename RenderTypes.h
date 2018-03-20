#ifndef  _RENDERTYPES_H
#define  _RENDERTYPES_H

#include<string>
#include<vector>
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

    int x;
    int y;
};

struct Rect
{
    Rect() {}
    Rect(int left , int top , int right , int bottom)
    {
        this->left = left;
        this->top = top;
        this->right = right;
        this->botoom = bottom;
    }

    int left;
    int top;
    int right;
    int botoom;
};

struct Material
{
	glm::vec4 diffuse;
	glm::vec4 ambient;
	glm::vec4 specular; // specular 'shininess'
	glm::vec4 emissive;
	float power; 		// Sharpness if specular highlight

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
const Rect EMPTY_RECT = Rect(0,0,0,0);
const glm::vec3 WHITE_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
const GLuint NO_TEXTURE = 0;

#endif // _RENDERTYPES_H
