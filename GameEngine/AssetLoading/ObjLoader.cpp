#include "ObjLoader.h"

#include<algorithm>

//-----------------------------------------------------------------------------
// Name : Group::Group(constructor)
//-----------------------------------------------------------------------------
Group::Group(std::string &rName)
{
    name = rName;
    numTrinagles = 0;
    material = -1;
}

//-----------------------------------------------------------------------------
// Name : Group::addVertex
//-----------------------------------------------------------------------------
GLushort Group::addVertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texCords)
{
    for (GLushort i = 0; i  < vertices.size(); i++)
    {
        if (vertices[i].Position == pos && vertices[i].Normal == normal &&
            vertices[i].TexCoords == texCords)
            return i;

    }

    vertices.emplace_back(pos, normal, texCords);
    return vertices.size() - 1;
}

//-----------------------------------------------------------------------------
// Name : Group::addVertex
//-----------------------------------------------------------------------------
GLushort Group::addVertex(Model& model,int* v, int* t, int* n)
{
    glm::vec3 pos;
    glm::vec3 normal = {0.0f, 0.0f, 0.0f};
    glm::vec2 texCords = {0.0f, 0.0f};

    if (v != nullptr)
    {
        if (*v >= 0)
        {
            pos.x = model.verticesPos[3*(*v - 1) + 0];
            pos.y = model.verticesPos[3*(*v - 1) + 1];
            pos.z = model.verticesPos[3*(*v - 1) + 2];
        }
        else
        {
            pos.x = model.verticesPos[3*(*v + model.numVertices) + 0];
            pos.y = model.verticesPos[3*(*v + model.numVertices) + 1];
            pos.z = model.verticesPos[3*(*v + model.numVertices) + 2];
        }
    }

    if (n != nullptr)
    {
        if (*n >= 0)
        {
            normal.x = model.normals[3*(*n - 1) + 0];
            normal.y = model.normals[3*(*n - 1) + 1];
            normal.z = model.normals[3*(*n - 1) + 2];
        }
        else
        {
            normal.x = model.normals[3*(*n + model.numNormals) + 0];
            normal.y = model.normals[3*(*n + model.numNormals) + 1];
            normal.z = model.normals[3*(*n + model.numNormals) + 2];
        }
    }

    if (t != nullptr)
    {
        if (*t != 0)
        {
            texCords.x = model.texCords[2*(*t - 1) + 0];
            texCords.y = model.texCords[2*(*t - 1) + 1];
        }
        else
        {
            texCords.x = model.texCords[2*(*t + model.numTexCords) + 0];
            texCords.y = model.texCords[2*(*t + model.numTexCords) + 1];
        }
    }

    return addVertex(pos, normal, texCords);
}

//-----------------------------------------------------------------------------
// Name : Model::Model(constructor)
//-----------------------------------------------------------------------------
Model::Model(std::string newMeshPath)
    :pos(0.0f, 0.0f, 0.0f)
{
    meshPath = newMeshPath;

    numVertices = 0;
    verticesPos = nullptr;

    numNormals = 0;
    normals = nullptr;

    numTexCords = 0;
    texCords = nullptr;

    numfaceNormals = 0;
    faceNormals = nullptr;

    numTrinagles = 0;

    numMaterials = 0;
    numGroups = 0;
}

//-----------------------------------------------------------------------------
// Name : Model::~Model(destructor)
//-----------------------------------------------------------------------------
Model::~Model()
{
    if (verticesPos != nullptr)
        delete[] verticesPos;

    if (normals != nullptr)
        delete[] normals;

    if (texCords != nullptr)
        delete[] texCords;
}

//-----------------------------------------------------------------------------
// Name : Model::addGroup()
//-----------------------------------------------------------------------------
Group& Model::addGroup(std::string name)
{
    if (name == "")
        name = "defualt";

    for (unsigned int i = 0; i < groups.size(); i++)
    {
        if (groups[i].name == name)
            return groups[i];
    }

    groups.emplace_back(name);
    return groups[groups.size() - 1];
}

//-----------------------------------------------------------------------------
// Name : objFirstPass
//-----------------------------------------------------------------------------
void objFirstPass(AssetManager& asset, std::ifstream& in ,Model& model)
{
    //char buf[128];
    std::string buf;
    Group* group = nullptr;
    bool readNextLine = true;

    int v, n, t;

    group = &model.addGroup("defualt");

    in >> buf;
    while (!in.eof())
    {
        switch(buf[0])
        {
            case '#':
            {
                // jump to the end of the line
                std::getline(in, buf);
                //in.getline(buf, 128);
            }break;

            case 'v':
            {
                switch(buf[1])
                {
                    case '\0':
                    {
                        // jump to the end of the line
                        std::getline(in, buf);
                        //in.getline(buf, 128);
                        model.numVertices++;
                    }break;

                    case 'n':
                    {
                        // jump to the end of the line
                        std::getline(in, buf);
                        //in.getline(buf, 128);
                        model.numNormals++;
                    }break;

                    case 't':
                    {
                        // jump to the end of the line
                        std::getline(in, buf);
                        //in.getline(buf, 128);
                        model.numTexCords++;
                    }break;

                    default:
                        std::cout << "objFirstPass() - Unknown token\n";
                        break;
                }
            }break;

            case 'm':
            {
                std::getline(in, buf);
                //in.getline(buf, 128);
                std::stringstream sStream(buf);
                sStream >> buf;
                sStream >> buf;
                model.matrialPath = buf;
                objReadMatrial(asset, model, buf);
            }break;

            case 'u':
                std::getline(in, buf);
                //in.getline(buf, 128);
            break;

            case 'g':
            {
                std::getline(in, buf);
                //std::stringstream sStream(buf);
                //sStream >> buf;
                group = &model.addGroup(buf);
            }
            break;

            case 'f':
            {
                //std::string buf;
                v = n = t = 0;
                in >> buf;
                // can be one of v , v//n, v/t, v/t/n, x//x
                if (buf.find("//") != std::string::npos)
                {
                    // v//n
                    //TODO: add a function to read v and n
                    std::stringstream s(buf);
                    s >> v;
                    s = std::stringstream(buf.substr(buf.find_last_of('/') + 1, buf.size() ));
                    s >> n;

                    in >> buf;
                    s = std::stringstream(buf);
                    s >> v;
                    s = std::stringstream(buf.substr(buf.find_last_of('/') + 1, buf.size() ));
                    s >> n;

                    in >> buf;
                    s = std::stringstream(buf);
                    s >> v;
                    s = std::stringstream(buf.substr(buf.find_last_of('/') + 1, buf.size() ));
                    s >> n;

                    model.numTrinagles++;
                    group->numTrinagles++;

                    in >> buf;
                    readNextLine = false;
                    while (buf.find("//") != std::string::npos && !in.eof())
                    {
                        model.numTrinagles++;
                        group->numTrinagles++;

                        in >> buf;
                    }
                }
                else if (std::count(buf.begin(), buf.end(), '/') == 2)
                {
                    // v/t/
                    std::replace(buf.begin(), buf.end(), '/', ' ');
                    std::stringstream s(buf);
                    s >> v;
                    s >> t;
                    s >> n;

                    in >> buf;
                    std::replace(buf.begin(), buf.end(), '/', ' ');
                    s = std::stringstream(buf);
                    s >> v;
                    s >> t;
                    s >> n;
                    
                    in >> buf;
                    std::replace(buf.begin(), buf.end(), '/', ' ');
                    s = std::stringstream(buf);
                    s >> v;
                    s >> t;
                    s >> n;

                    model.numTrinagles++;
                    group->numTrinagles++;

                    in >> buf;
                    readNextLine = false;
                    while (buf.find("/") != std::string::npos && !in.eof())
                    {
                        model.numTrinagles++;
                        group->numTrinagles++;

                        in >> buf;
                    }
                }
                else if (std::count(buf.begin(), buf.end(), '/') == 1)
                {
                    // v/t
                    std::replace(buf.begin(), buf.end(), '/', ' ');
                    std::stringstream s(buf);
                    s >> v;
                    s >> t;

                    in >> buf;
                    std::replace(buf.begin(), buf.end(), '/', ' ');
                    s = std::stringstream(buf);
                    s >> v;
                    s >> t;

                    model.numTrinagles++;
                    group->numTrinagles++;

                    in >> buf;
                    readNextLine = false;
                    while ( std::count(buf.begin(), buf.end(), '/') > 0 )
                    {
                        model.numTrinagles++;
                        group->numTrinagles++;

                        in >> buf;
                    }
                }
                else
                {
                    // v
                    in >> v;

                    model.numTrinagles++;
                    group->numTrinagles++;

                    in >> buf;
                    readNextLine = false;
                    std::stringstream s(buf);
                    s >> v;
                    while (!s.fail() && !in.eof())
                    {
                        model.numTrinagles++;
                        group->numTrinagles++;

                        in >> buf;
                        s = std::stringstream(buf);
                        s >> v;
                    }
                }
            }break;

            default:
                std::getline(in, buf);
                //in.getline(buf, 128);
                break;

        }
        // next line was already read
        if (readNextLine)
            in >> buf;
        else
            readNextLine = true;
    }

    for (Group& g : model.groups)
    {
        //g.triangles = new GLuint[g.numTrinagles];
        g.numTrinagles = 0;
    }
}

//-----------------------------------------------------------------------------
// Name : objSecondPass
//-----------------------------------------------------------------------------
void objSecondPass(std::ifstream& in, Model& model)
{
    int v, n, t;
    GLuint numVertices;
    GLuint numNormals;
    GLuint numTexCords;
    GLuint numTrinagles;

    GLuint material;
    Group* group;

    std::string buf;
    bool readNextLine = true;

    group = &model.addGroup("defualt");

    numVertices = numNormals = numTexCords = 0;
    numTrinagles = 0;
    material = 0;

    model.verticesPos = new float[model.numVertices * 3];
    model.normals = new float[model.numNormals * 3];
    model.texCords = new float[model.numTexCords * 2];

    in >> buf;
    while (!in.eof())
    {
        switch(buf[0])
        {
            case '#':
            {
                // jump to the end of the line
                std::getline(in, buf);
                //in.getline(buf, 128);
            }break;

            case 'v':
            {
                switch(buf[1])
                {
                    case '\0':
                    {
                        in >> model.verticesPos[3 * numVertices + 0];
                        in >> model.verticesPos[3 * numVertices + 1];
                        in >> model.verticesPos[3 * numVertices + 2];
                        numVertices++;
                    }break;

                    case 'n':
                    {
                        in >> model.normals[3 * numNormals + 0];
                        in >> model.normals[3 * numNormals + 1];
                        in >> model.normals[3 * numNormals + 2];
                        numNormals++;
                    }break;

                    case 't':
                    {
                        in >> model.texCords[2 * numTexCords + 0];
                        in >> model.texCords[2 * numTexCords + 1];
                        numTexCords++;
                    }break;
                }
            }break;

            case 'u':
            {
                std::getline(in, buf);
                std::stringstream s(buf);

                s >> buf;
                s >> buf;
                // group by material
                group = &model.addGroup(buf);
                group->material = material = model.materials[buf];
            }break;
            case 'g':
            {
                // Seems like I can ignore groups for now
                std::getline(in, buf);
                //in >> buf;
                //group = &model.addGroup(buf);
                //group->material = material;
            }break;

            case 'f':
            {
                //std::string buf;
                v = n = t = 0;

                in >> buf;
                // can be one of v , v//n, v/t, v/t/n, x//x
                if (buf.find("//") != std::string::npos)
                {
                    // v//n
                    // make the string easier to parse
                    std::stringstream s;

                    GLushort index;
                    // merge all the face vertex data together and add to the vector
                    for (int i = 0; i < 3; i ++)
                    {
                        // make the string easier to parse
                        std::replace(buf.begin(), buf.end(), '/', ' ');
                        std::stringstream s(buf);
                        s >> v;
                        s >> n;

                        index = group->addVertex(model,&v, nullptr, &n);
                        group->indices.push_back(index);

                        // will read the next word before quitting the loop
                        in >> buf;
                    }

                    readNextLine = false;
                    while (buf.find("//") != std::string::npos)
                    {
                        // make the string easier to parse
                        std::replace(buf.begin(), buf.end(), '/', ' ');
                        std::stringstream s(buf);
                        s >> v;
                        s >> n;

                        int indicesSize = group->indices.size();
                        group->indices.push_back(group->indices[indicesSize - 3]);
                        group->indices.push_back(group->indices[indicesSize - 1]);

                        index = group->addVertex(model, &v, nullptr, &n);
                        group->indices.push_back(index);

                        in >> buf;
                    }

                }
                else if (std::count(buf.begin(), buf.end(), '/') == 2)
                {
                    // v/t/n
                    GLushort index;

                    for (int i = 0; i < 3; i++)
                    {
                        std::replace(buf.begin(), buf.end(), '/', ' ');
                        std::stringstream s(buf);
                        s >> v;
                        s >> t;
                        s >> n;

                        index = group->addVertex(model ,&v, &t, &n);
                        group->indices.push_back(index);

                        in >> buf;
                    }

                    readNextLine = false;
                    while (std::count(buf.begin(), buf.end(), '/') > 0)
                    {
                        std::replace(buf.begin(), buf.end(), '/', ' ');
                        std::stringstream s(buf);
                        s >> v;
                        s >> t;
                        s >> n;

                        int indicesSize = group->indices.size();
                        group->indices.push_back(group->indices[indicesSize - 3]);
                        group->indices.push_back(group->indices[indicesSize - 1]);

                        index = group->addVertex(model, &v, &t, &n);
                        group->indices.push_back(index);

                        in >> buf;
                    }

                }
                else if (std::count(buf.begin(), buf.end(), '/') == 1)
                {
                    // v/t
                    GLushort index;

                    for (int i = 0; i < 3; i++)
                    {
                        std::replace(buf.begin(), buf.end(), '/', ' ');
                        std::stringstream s(buf);
                        s >> v;
                        s >> t;

                        index = group->addVertex(model, &v, &t, nullptr);
                        group->indices.push_back(index);

                        in >> buf;
                    }

                    readNextLine = false;
                    while (std::count(buf.begin(), buf.end(), '/') > 0)
                    {
                        std::replace(buf.begin(), buf.end(), '/', ' ');
                        std::stringstream s(buf);
                        s >> v;
                        s >> t;

                        int indicesSize = group->indices.size();
                        group->indices.push_back(group->indices[indicesSize - 3]);
                        group->indices.push_back(group->indices[indicesSize - 1]);

                        index = group->addVertex(model, &v, &t, nullptr);
                        group->indices.push_back(index);

                        in >> buf;
                    }

                }
                else
                {
                    // v
                    GLushort index;
                    std::stringstream s(buf);
                    s >> v;

                    //std::cout << v;

                    index = group->addVertex(model, &v, nullptr, nullptr);
                    group->indices.push_back(index);

                    for (int i = 0; i < 2; i++)
                    {
                        in >> v;
                        //std::cout <<" "<< v;

                        index = group->addVertex(model, &v, nullptr, nullptr);
                        group->indices.push_back(index);
                    }

                    //std::cout << "\n";

                    in >> buf;
                    readNextLine = false;
                    s = std::stringstream(buf);
                    s >> v;
                    while (!s.fail() && !in.eof())
                    {
                        //s = std::stringstream(buf);
                        //s >> v;
                        int indicesSize = group->indices.size();
                        group->indices.push_back(group->indices[indicesSize - 3]);
                        group->indices.push_back(group->indices[indicesSize - 1]);

                        index = group->addVertex(model,&v, nullptr, nullptr);
                        group->indices.push_back(index);

                        in >> buf;
                        s = std::stringstream(buf);
                        s >> v;
                    }
                }
            }break;

            default:
                std::getline(in, buf);
            break;

        }

        if (readNextLine)
            in >> buf;
        else
            readNextLine = true;
    }

}

//-----------------------------------------------------------------------------
// Name : objReadMatrial
//-----------------------------------------------------------------------------
void objReadMatrial(AssetManager& asset, Model& model, std::string matrialPath)
{
    std::ifstream in;
    std::string& meshPath = model.meshPath;
    std::string dir;
    char buf[128];

    GLuint numMaterials;
    Material curMaterial;
    std::string curMaterialName;

    std::size_t found = meshPath.find_last_of("/\\");
    if (found != std::string::npos)
        dir = meshPath.substr(0, found + 1);
    else
        dir = "";

    in.open(dir + matrialPath);

    if (!in.is_open())
    {
        std::cout << "objReadMatrial() failed: can't open material file "<< dir + matrialPath << "\n";
        return;
    }

    // first pass of the file
    numMaterials = 1;

    in >> buf;
    while (!in.eof())
    {
        switch(buf[0])
        {
            case '#':
                in.getline(buf, 128);
            break;
            case 'n':
                in.getline(buf, 128);
                numMaterials++;
            break;
            default:
                in.getline(buf, 128);
            break;
        }

        in >> buf;
    }

    in.clear();
    in.seekg(0, std::ios::beg);

    //model.materials = new Material[numMaterials];
    model.numMaterials = numMaterials;

    // second pass
    numMaterials = 0;

    in >> buf;
    while (!in.eof())
    {
        switch(buf[0])
        {
            case '#':
                in.getline(buf, 128);
            break;
            case 'n':
                GLuint materialIndex;

                if (numMaterials != 0)
                {
                    // new material detected
                    // save the pervious material
                    materialIndex = asset.getMaterialIndex(curMaterial);
                    model.materials.insert(std::pair<std::string, GLuint>(curMaterialName, materialIndex));
                    curMaterial = Material();
                }

                numMaterials++;

                in >> buf;
                curMaterialName = buf;
            break;
            case 'N':
            {
                float shininess;
                in >> curMaterial.power;
                curMaterial.power /= 1000.0;
                curMaterial.power *= 128.0;
            }
            break;

            case 'K':
                switch (buf[1])
                {
                    case 'd':
                        in >> curMaterial.diffuse.r;
                        in >> curMaterial.diffuse.g;
                        in >> curMaterial.diffuse.b;
                        curMaterial.diffuse.a = 1.0f;
                    break;
                    case 's':
                        in >> curMaterial.specular.r;
                        in >> curMaterial.specular.g;
                        in >> curMaterial.specular.b;
                        curMaterial.specular.a = 1.0f;
                    break;
                    case 'a':
                        in >> curMaterial.ambient.r;
                        in >> curMaterial.ambient.g;
                        in >> curMaterial.ambient.b;
                        curMaterial.ambient.a = 1.0f;
                    break;
                    default:
                        in.getline(buf, 128);
                    break;
                }
        }
        in >> buf;
    }

    // save the last material added
    GLuint materialIndex = asset.getMaterialIndex(curMaterial);
    model.materials.insert(std::pair<std::string, GLuint>(curMaterialName, materialIndex));
}
