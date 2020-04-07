#ifndef  _ASSETMANAGER_H
#define  _ASSETMANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

#include <cstdio>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <png.h>
#include <jpeglib.h>

#include "../Render/RenderTypes.h"
#include "../Render/Shader.h"
#include "../Render/Mesh.h"
#include "../Render/Font.h"
#ifdef FBX
#include "FbxLoader.h"
#endif
#include "ObjLoader.h"

#ifndef _WIN32
#define MAX_PATH 256
#endif // _WIN32

struct TextureInfo
{
        TextureInfo()
            :width(0), height(0)
        {}
        TextureInfo(int _width, int _height)
        {
                width = _width;
                height = _height;
        }

        int width;
        int height;
};

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();

    GLuint    getTexture(const std::string& filePath);
    const TextureInfo& getTextureInfo(GLuint textureName);


    Mesh*     getMesh(const std::string& meshPath);

    Shader*   getShader(const std::string& shaderPath);
    int       getMaterialIndex(const Material& mat);
    Material& getMaterial(int materialIndex);
    int       getAttribute(const std::string& texPath, GLint wrapMode, const Material& mat,const std::string& shaderPath);
    int       getAttribute(const std::string& texPath, GLint wrapMode, GLuint matIndex, const std::string& shaderPath);
    mkFont *  getFont(std::string fontName, int fontSize, bool isPath = false);

    const std::vector<Attribute>& getAttributeVector();

private:
    static TextureInfo s_noTextureInfo;
    GLuint createTexture(GLsizei width, GLsizei height, GLenum format,unsigned char* data);

    GLuint loadPng(const std::string& filePath);
    GLuint loadBMP(const std::string& filePath);
    GLuint loadJPEG(const std::string& filePath);

    Mesh*  loadObjMesh(const std::string& meshPath);
    Mesh*  loadFBXMesh(const std::string& meshPath);
    Mesh*  generateMesh(const std::string& meshString);

    const unsigned long START_TEXTURE_SIZE = 100;

    std::unordered_map<std::string,GLuint>   m_textureCache;
    std::unordered_map<GLuint, TextureInfo>  m_textureInfoCache;
    std::unordered_map<std::string, Mesh>    m_meshCache;
    std::unordered_map<std::string, Shader*> m_shaderCache;
    std::unordered_map< std::string, mkFont> m_fontCache;
    std::vector<Material> m_materials;
    std::vector<Attribute> m_attributes;

    #ifdef FBX
    FbxLoader m_fbxLoader;
    #endif
};

#endif  //_ASSETMANAGER_H
