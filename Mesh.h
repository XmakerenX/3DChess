#ifndef  _MESH_H
#define  _MESH_H

#include <string>
#include "subMesh.h"

class Mesh {

public:
    Mesh();
    Mesh(const std::vector<SubMesh>& sMeshes);
    Mesh(const std::vector<SubMesh>& sMeshes, const std::vector<GLuint>& dMaterials, const std::vector<std::string>& dTextures);
    Mesh(std::vector<SubMesh>&& sMeshes, std::vector<GLuint>&& dMaterials, std::vector<std::string>&& dTextures);
    Mesh(const Mesh& copyMesh);
    Mesh& operator=(const Mesh& copy);
    Mesh(Mesh&& moveMesh);
    Mesh& operator=(Mesh&& move);

    // Render the mesh
    void Draw(unsigned int subMeshIndex);
    void addSubMesh(SubMesh subMesh);

    bool IntersectTriangle(glm::vec3& rayObjOrigin,glm::vec3& rayObjDir, int& faceCount, int& subMeshIndex);
    void CalcVertexNormals(GLfloat angle);

    std::vector<GLuint>& getDefaultMaterials();
    std::vector<std::string>& getDefaultTextures();

private:
    std::vector<SubMesh> m_subMeshes;
    std::vector<GLuint> m_defaultMaterials;
    std::vector<std::string> m_defaultTextures;
};


#endif  //_MESH_H
