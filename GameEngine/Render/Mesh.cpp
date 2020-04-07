#include "Mesh.h"

//-----------------------------------------------------------------------------
// Name : Mesh (constructor)
//-----------------------------------------------------------------------------
Mesh::Mesh()
{}

//-----------------------------------------------------------------------------
// Name : Mesh (constructor)
//-----------------------------------------------------------------------------
Mesh::Mesh(const std::vector<SubMesh>& sMeshes)
{
    m_subMeshes = sMeshes;
}

//-----------------------------------------------------------------------------
// Name : Mesh (constructor)
//-----------------------------------------------------------------------------
Mesh::Mesh(const std::vector<SubMesh>& sMeshes, const std::vector<GLuint>& dMaterials, const std::vector<std::string>& dTextures)
    :m_subMeshes(sMeshes),
     m_defaultMaterials(dMaterials),
     m_defaultTextures(dTextures)
{
    // add empty texture for every material that doesn't have one
    for (int i = m_defaultTextures.size(); i < m_defaultMaterials.size(); i++)
        m_defaultTextures.push_back("");
}

//-----------------------------------------------------------------------------
// Name : Mesh (constructor)
//-----------------------------------------------------------------------------
Mesh::Mesh(std::vector<SubMesh>&& sMeshes, std::vector<GLuint>&& dMaterials, std::vector<std::string>&& dTextures)
    :m_subMeshes(std::move(sMeshes)),
     m_defaultMaterials(std::move(dMaterials)),
     m_defaultTextures(std::move(dTextures))
{
    // add empty texture for every material that doesn't have one
    for (int i = m_defaultTextures.size(); i < m_defaultMaterials.size(); i++)
        m_defaultTextures.push_back("");
}

//-----------------------------------------------------------------------------
// Name : Mesh (copy constructor)
//-----------------------------------------------------------------------------
Mesh::Mesh(const Mesh& copyMesh)
    :m_subMeshes(copyMesh.m_subMeshes), 
     m_defaultMaterials(copyMesh.m_defaultMaterials),
     m_defaultTextures(copyMesh.m_defaultTextures)
{
}

//-----------------------------------------------------------------------------
// Name : Mesh (copy assignment)
//-----------------------------------------------------------------------------
Mesh& Mesh::operator=(const Mesh& copy)
{
    m_subMeshes = copy.m_subMeshes;
    m_defaultMaterials = copy.m_defaultMaterials;
    m_defaultTextures = copy.m_defaultTextures;
    
    return *this;
}

//-----------------------------------------------------------------------------
// Name : Mesh (move constructor)
//-----------------------------------------------------------------------------
Mesh::Mesh(Mesh&& moveMesh)
    :m_subMeshes(std::move(moveMesh.m_subMeshes)), 
     m_defaultMaterials(std::move(moveMesh.m_defaultMaterials)),
     m_defaultTextures(std::move(moveMesh.m_defaultTextures))
{
}

//-----------------------------------------------------------------------------
// Name : Mesh (move assignment)
//-----------------------------------------------------------------------------
Mesh& Mesh::operator=(Mesh&& move)
{
    m_subMeshes = std::move(move.m_subMeshes);
    m_defaultMaterials = std::move(move.m_defaultMaterials);
    m_defaultTextures = std::move(move.m_defaultTextures);
    
    return *this;
}

//-----------------------------------------------------------------------------
// Name : Draw ()
//-----------------------------------------------------------------------------
void Mesh::Mesh::Draw(unsigned int subMeshIndex)
{
    m_subMeshes[subMeshIndex].Draw();
}

//-----------------------------------------------------------------------------
// Name : addSubMesh ()
//-----------------------------------------------------------------------------
void Mesh::addSubMesh(SubMesh subMesh)
{
    m_subMeshes.push_back(subMesh);
}

//-----------------------------------------------------------------------------
// Name : IntersectTriangle ()
//-----------------------------------------------------------------------------
bool Mesh::IntersectTriangle(glm::vec3& rayObjOrigin, glm::vec3& rayObjDir, int& faceCount, int& subMeshIndex)
{
    for (GLuint i = 0; i < m_subMeshes.size(); ++i)
    {
        if (m_subMeshes[i].IntersectTriangle(rayObjOrigin, rayObjDir, faceCount))
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
    for (SubMesh& mesh : m_subMeshes)
    {
        mesh.CalcVertexNormals(angle);
    }
}

//-----------------------------------------------------------------------------
// Name : getDefaultMaterials ()
//-----------------------------------------------------------------------------
std::vector<GLuint>& Mesh::getDefaultMaterials()
{
    return m_defaultMaterials;
}

//-----------------------------------------------------------------------------
// Name : getDefaultTextures ()
//-----------------------------------------------------------------------------
std::vector<std::string>& Mesh::getDefaultTextures()
{
    return m_defaultTextures;
}
