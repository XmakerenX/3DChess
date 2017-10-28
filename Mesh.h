#ifndef  _MESH_H
#define  _MESH_H

#include <string>
#include "subMesh.h"

class Mesh {

public:
    // Constructor
    Mesh();
	Mesh(std::vector<SubMesh> sMeshes);
	Mesh(std::vector<SubMesh> sMeshes, std::vector<GLuint> dMaterials,
		std::vector<std::string> dTextures);

    // Render the mesh
    void Draw(unsigned int subMeshIndex);
	void addSubMesh(SubMesh subMesh);

    void CalcVertexNormals(GLfloat angle);

    std::vector<GLuint>& getDefaultMaterials();
    std::vector<std::string>& getDefaultTextures();

private:
	std::vector<SubMesh> subMeshes;
	std::vector<GLuint> defaultMaterials;
	std::vector<std::string> defaultTextures;
};


#endif  //_MESH_H
