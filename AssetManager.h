#ifndef  _ASSETMANAGER_H
#define  _ASSETMANAGER_H

#include<iostream>
#include<fstream>
#include<cstdio>
#include<unordered_map>
#include<GL/glew.h>
#include<png.h>
#include<jpeglib.h>
#include<sstream>

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();

    GLuint getTexture(const std::string& filePath);
    GLuint loadPng(const std::string& filePath);
    GLuint loadBMP(const std::string& filePath);
    GLuint loadJPEG(const std::string& filePath);

private:
    GLuint createTexture(GLsizei width, GLsizei height, GLenum format,unsigned char* data);

    const unsigned long START_TEXTURE_SIZE = 100;

    std::unordered_map<std::string,GLuint> textureCache_;
};

#endif  //_ASSETMANAGER_H
