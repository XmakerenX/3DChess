#ifndef  _ASSETMANAGER_H
#define  _ASSETMANAGER_H

#include<iostream>
#include<fstream>
#include<sstream>
#include<unordered_map>
#include<vector>
#include<string>
#include<sstream>
#include<exception>

#include<cstdio>

#include<GL/glew.h>
#include <glm/glm.hpp>

#include<png.h>
#include<jpeglib.h>

#include <fbxsdk.h>

#include "Shader.h"
#include "Mesh.h"

struct Model;

struct OBJnode
{
    GLuint index;
    GLboolean averaged;
    OBJnode * next;
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

struct Group
{
	Group(std::string& rName)
	{
		name = rName;
		numTrinagles = 0;
        material = -1;
		
	}
	
	GLushort addVertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texCords)
	{
		for (GLushort i = 0; i  < vertices.size(); i++)
		{
			if (vertices[i].Position == pos && vertices[i].Normal == normal && 
				vertices[i].TexCoords == texCords)
				return i;
			
		}
		
		vertices.emplace_back(pos, normal, texCords);
		return vertices.size() - 1;
	}
	
	GLushort addVertex(Model& model,int* v, int* t, int* n);
	
	std::string name;
	GLuint numTrinagles;
	
	std::vector<Vertex> vertices;
	std::vector<GLushort> indices;
	
	GLuint material;
};

struct Model
{
	Model(std::string newMeshPath)
	:pos(0.0f, 0.0f, 0.0f)
	{
		meshPath = newMeshPath;
		
		numVertices = 0;
		verticesPos = nullptr;
		
		numNormals = 0;
		normals = nullptr;
		
		numTexCords = 0;
		texCords = nullptr;
		
		numfaceNormals = 0;
		faceNormals = nullptr;
		
		numTrinagles = 0;
		
		numMaterials = 0;
		//materials = nullptr;
		
		numGroups = 0;
	}
	
	~Model()
	{
		if (verticesPos != nullptr)
			delete[] verticesPos;
		
		if (normals != nullptr)
			delete[] normals;
		
		if (texCords != nullptr)
			delete[] texCords;
	}
	
	Group& addGroup(std::string name)
	{
		if (name == "")
			name = "defualt";
		
		for (unsigned int i = 0; i < groups.size(); i++)
		{
			if (groups[i].name == name)
				return groups[i];
		}
		
		groups.emplace_back(name);
		return groups[groups.size() - 1];
	}
	

	
	std::string meshPath;
	std::string matrialPath;
	
	GLuint numVertices;
	GLfloat* verticesPos;
	
	GLuint numNormals;
	GLfloat* normals;
	
	GLuint numTexCords;
	GLfloat* texCords;
	
	GLuint numfaceNormals;
	GLfloat* faceNormals;
	
	GLuint numTrinagles;
	
	GLuint numMaterials;
	//Material* materials;
	
	GLuint numGroups;
	std::vector<Group> groups;
	std::unordered_map<std::string,GLuint> materials;
	
	//std::vector<Vertex> vertices;
	//std::vector<GLushort> indices;
	
	glm::vec3 pos;
};

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();

    GLuint getTexture(const std::string& filePath);
    GLuint loadPng(const std::string& filePath);
    GLuint loadBMP(const std::string& filePath);
    GLuint loadJPEG(const std::string& filePath);

    Mesh* loadFBXMesh(const std::string& meshPath);
    glm::vec3 readFBXNormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter );
    glm::vec2 readFBXUV(FbxMesh* mesh, int controlPointIndex, int j, int k);
	
	Mesh*    getMesh(const std::string& meshPath);
	Mesh*	 loadObjMesh(const std::string& meshPath);
	void 	 objFirstPass(std::ifstream& in ,Model& model);
	void 	 objSecondPass(std::ifstream& in ,Model& model);
	void 	 objReadMatrial(Model& model, std::string matrialPath);
	
	Shader* getShader(const std::string& shaderPath);
	int 	getMaterialIndex(const Material& mat);
    Material& getMaterial(int materialIndex);
	int getAttribute(const std::string& texPath, const Material& mat,const std::string& shaderPath);
    int getAttribute(const std::string& texPath, GLuint matIndex,const std::string& shaderPath);

    const std::vector<Attribute>& getAttributeVector();

private:
    GLuint createTexture(GLsizei width, GLsizei height, GLenum format,unsigned char* data);

    const unsigned long START_TEXTURE_SIZE = 100;

    std::unordered_map<std::string,GLuint> m_textureCache;
	std::unordered_map<std::string, Mesh> m_meshCache;
	std::unordered_map<std::string, Shader*> m_shaderCache;
	std::vector<Material> m_materials;
	std::vector<Attribute> m_attributes;

    FbxManager* m_pFbxSdkManager;
};

#endif  //_ASSETMANAGER_H
