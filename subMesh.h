#ifndef  _SUBMESH_H
#define  _SUBMESH_H

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL // needed for gtx extensions
#include <glm/gtx/intersect.hpp>
#include "RenderTypes.h"
#include "Shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    Vertex() :Position(), Normal(), TexCoords() {}
    
    Vertex(glm::vec3 pos, glm::vec3 normal ,glm::vec2 texCord) 
        :Position(pos), Normal(normal), TexCoords(texCord) {}
};

class SubMesh {

public:
    std::vector<Vertex> m_vertices;
    std::vector<VertexIndex> m_indices;

    SubMesh(const std::vector<Vertex>& vertices, const std::vector<VertexIndex>& indices);

    void Draw();

    bool IntersectTriangle(glm::vec3& rayObjOrigin, glm::vec3& rayObjDir, int& faceCount);

    //TODO: cuase this functio to really work
    void CalcVertexNormals(GLfloat angle);

private:
    GLuint m_VAO, m_VBO, m_EBO;

    void setupMesh();
};


#endif  //_SUBMESH_H
