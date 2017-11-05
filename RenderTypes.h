#ifndef  _RENDERTYPES_H
#define  _RENDERTYPES_H

#include<string>
#include<vector>
#include <glm/glm.hpp>

struct Point
{
    int x;
    int y;
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

#endif // _RENDERTYPES_H
