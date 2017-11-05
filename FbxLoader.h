#ifndef  _FBXLOADER_H
#define  _FBXLOADER_H

#include <fbxsdk.h>
#include<vector>
#include<string>
#include <glm/glm.hpp>
#include "subMesh.h"

class FbxLoader
{
public:
    FbxLoader()
    {
	m_pFbxSdkManager = nullptr;
    }

    bool LoadMesh(std::string meshPath, std::vector<SubMesh>& subMeshes);
private:
    glm::vec3 readFBXNormal(FbxMesh* mesh, int controlPointIndex, int vertexCounter );
    glm::vec2 readFBXUV(FbxMesh* mesh, int controlPointIndex, int j, int k);

    FbxManager* m_pFbxSdkManager;
};

#endif // _FBXLOADER_H
