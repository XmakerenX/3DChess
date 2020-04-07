#ifndef  _OBJLOADER_H
#define  _OBJLOADER_H

#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Render/RenderTypes.h"
// for vertex
#include "../Render/Mesh.h"
#include "AssetManager.h"


class AssetManager;
struct Model;

struct Group
{
    Group(std::string& rName);

    GLushort addVertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texCords);
    GLushort addVertex(Model& model,int* v, int* t, int* n);

    std::string name;
    GLuint numTrinagles;

    std::vector<Vertex> vertices;
    std::vector<VertexIndex> indices;

    GLuint material;
};

struct Model
{
    Model(std::string newMeshPath);
    ~Model();

    Group& addGroup(std::string name);

    std::string meshPath;
    std::string matrialPath;

    GLuint numVertices;
    GLfloat* verticesPos;

    GLuint numNormals;
    GLfloat* normals;

    GLuint numTexCords;
    GLfloat* texCords;

    GLuint numfaceNormals;
    GLfloat* faceNormals;

    GLuint numTrinagles;
    GLuint numMaterials;
    GLuint numGroups;
    
    std::vector<Group> groups;
    std::unordered_map<std::string,GLuint> materials;

    glm::vec3 pos;
};

void objFirstPass(AssetManager &asset, std::ifstream& in , Model& model);
void objSecondPass(std::ifstream& in ,Model& model);
void objReadMatrial(AssetManager& asset, Model& model, std::string matrialPath);

#endif // _OBJLOADER_H
