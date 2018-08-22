#include "Mesh.h"

//-----------------------------------------------------------------------------
// Name : Mesh (constructor)
//-----------------------------------------------------------------------------
Mesh::Mesh()
{
	
}

//-----------------------------------------------------------------------------
// Name : Mesh (constructor)
//-----------------------------------------------------------------------------
Mesh::Mesh(std::vector<SubMesh> sMeshes)
{
	subMeshes = sMeshes;
}

//-----------------------------------------------------------------------------
// Name : Mesh (constructor)
//-----------------------------------------------------------------------------
Mesh::Mesh(std::vector<SubMesh> sMeshes, std::vector<GLuint> dMaterials, std::vector<std::string> dTextures)
:subMeshes(sMeshes),
 defaultMaterials(dMaterials),
 defaultTextures(dTextures)
{
    // add empty texture for every material that doesn't have one
    for (int i = defaultTextures.size(); i < defaultMaterials.size(); i++)
        defaultTextures.push_back("");
}

//-----------------------------------------------------------------------------
// Name : Draw ()
//-----------------------------------------------------------------------------
void Mesh::Mesh::Draw(unsigned int subMeshIndex)
{
	subMeshes[subMeshIndex].Draw();
}

//-----------------------------------------------------------------------------
// Name : addSubMesh ()
//-----------------------------------------------------------------------------
void Mesh::addSubMesh(SubMesh subMesh)
{
	subMeshes.push_back(subMesh);
}

//-----------------------------------------------------------------------------
// Name : IntersectTriangle ()
//-----------------------------------------------------------------------------
bool Mesh::IntersectTriangle(glm::vec3& rayObjOrigin, glm::vec3& rayObjDir, int& faceCount, int& subMeshIndex)
{
    for (GLuint i = 0; i < subMeshes.size(); ++i)
    {
        if (subMeshes[i].IntersectTriangle(rayObjOrigin, rayObjDir, faceCount))
        {
            subMeshIndex = i;
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : CalcVertexNormals ()
//-----------------------------------------------------------------------------
void Mesh::CalcVertexNormals(GLfloat angle)
{
    for (SubMesh& mesh : subMeshes)
    {
        mesh.CalcVertexNormals(angle);
    }
}

//-----------------------------------------------------------------------------
// Name : getDefaultMaterials ()
//-----------------------------------------------------------------------------
std::vector<GLuint>& Mesh::getDefaultMaterials()
{
    return defaultMaterials;
}

//-----------------------------------------------------------------------------
// Name : getDefaultTextures ()
//-----------------------------------------------------------------------------
std::vector<std::string>& Mesh::getDefaultTextures()
{
    return defaultTextures;
}

