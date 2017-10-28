#ifndef  _SUBMESH_H
#define  _SUBMESH_H

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
	
	Vertex()
	:Position(), Normal(), TexCoords()
	{}
	
	Vertex(glm::vec3 pos, glm::vec3 normal ,glm::vec2 texCord)
	:Position(pos), Normal(normal), TexCoords(texCord)
	{}
};

class SubMesh {

public:
    std::vector<Vertex> vertices;
    std::vector<GLushort> indices;

    SubMesh(std::vector<Vertex> vertices, std::vector<GLushort> indices);

    void Draw();

    void CalcVertexNormals(GLfloat angle);

private:
    GLuint VAO, VBO, EBO;

    void setupMesh();
};


#endif  //_SUBMESH_H
