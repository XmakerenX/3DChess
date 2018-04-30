#ifndef  _ASSETMANAGER_H
#define  _ASSETMANAGER_H

#include<iostream>
#include<fstream>
#include<sstream>
#include<unordered_map>
#include<vector>
#include<string>
#include<sstream>

#include<cstdio>

#include<GL/glew.h>
#include <glm/glm.hpp>

#include<png.h>
#include<jpeglib.h>

#include "FbxLoader.h"

#include "RenderTypes.h"
#include "Shader.h"
#include "Mesh.h"
#include "ObjLoader.h"
#include "Font.h"

#define MAX_PATH 256

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();

    GLuint getTexture(const std::string& filePath);


    Mesh*  getMesh(const std::string& meshPath);
	
	Shader* getShader(const std::string& shaderPath);
	int 	getMaterialIndex(const Material& mat);
    Material& getMaterial(int materialIndex);
	int getAttribute(const std::string& texPath, const Material& mat,const std::string& shaderPath);
    int getAttribute(const std::string& texPath, GLuint matIndex,const std::string& shaderPath);
    mkFont *getFont(std::string fontName, int fontSize, bool isPath = false);

    const std::vector<Attribute>& getAttributeVector();

private:
    GLuint createTexture(GLsizei width, GLsizei height, GLenum format,unsigned char* data);

    GLuint loadPng(const std::string& filePath);
    GLuint loadBMP(const std::string& filePath);
    GLuint loadJPEG(const std::string& filePath);

    Mesh*  loadObjMesh(const std::string& meshPath);
    Mesh*  loadFBXMesh(const std::string& meshPath);

    const unsigned long START_TEXTURE_SIZE = 100;

    std::unordered_map<std::string,GLuint> m_textureCache;
	std::unordered_map<std::string, Mesh> m_meshCache;
	std::unordered_map<std::string, Shader*> m_shaderCache;
    //std::unordered_map< std::pair<std::string, GLuint>, mkFont> m_fontCache;
    std::unordered_map< std::string, mkFont> m_fontCache;
	std::vector<Material> m_materials;
	std::vector<Attribute> m_attributes;

    FbxLoader m_fbxLoader;
};

#endif  //_ASSETMANAGER_H
