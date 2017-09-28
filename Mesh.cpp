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
{}

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
