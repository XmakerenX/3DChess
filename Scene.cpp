#include "Scene.h"

const char* meshShaderPath2 = "objectShader2";

//-----------------------------------------------------------------------------
// Name : Scene (constructor)
//-----------------------------------------------------------------------------
Scene::Scene()
{
    nActiveLights = 0;
    ubLight = 0;

    curObj = nullptr;
}

//-----------------------------------------------------------------------------
// Name : Scene (destructor)
//-----------------------------------------------------------------------------
Scene::~Scene()
{
    glDeleteBuffers(1, &ubMaterial);
    if (ubLight != 0)
        glDeleteBuffers(1, &ubLight);
}

//-----------------------------------------------------------------------------
// Name : InitScene ()
//-----------------------------------------------------------------------------
void Scene::InitScene()
{
    meshShader =  m_assetManager.getShader(meshShaderPath2);

    projectionLoc = glGetUniformLocation(meshShader->Program, "projection");
    matWorldLoc = glGetUniformLocation(meshShader->Program, "matWorld");
    matWorldInverseLoc = glGetUniformLocation(meshShader->Program, "matWorldInverseT");

    // Init the material unifrom buffer
    ubMaterialIndex = glGetUniformBlockIndex(meshShader->Program, "Material");
    glGenBuffers(1, &ubMaterial);
    glBindBufferBase(GL_UNIFORM_BUFFER, ubMaterialIndex, ubMaterial);
    glUniformBlockBinding(meshShader->Program, ubMaterialIndex, ubMaterialIndex);

    InitLights();
    InitObjects();
}

//-----------------------------------------------------------------------------
// Name : InitCamera()
//-----------------------------------------------------------------------------
void Scene::InitCamera(int width, int height)
{
    m_camera.SetFOV(60.0f);
    m_camera.SetViewPort(0.0, 0.0, width, height, 1.0f, 1000.0f);
    m_camera.SetPostion(glm::vec3(0.0f, 20.0f, -70.0f));
    m_camera.SetLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
}

//-----------------------------------------------------------------------------
// Name : InitLights()
//-----------------------------------------------------------------------------
void Scene::InitLights()
{
    light[0].dir = glm::vec4(1.0f, -0.8f, 0.4f, 0.0f);
    light[0].ambient = glm::vec4(0.4f, 0.4f, 0.4f, 0.4f) * 0.4f;
    light[0].diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    light[0].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    light[0].pos = glm::vec3(0.0f, 0.0f, 0.0f);
    nActiveLights++;

    ubLightIndex = glGetUniformBlockIndex(meshShader->Program, "lightBlock");
    glGenBuffers(1, &ubLight);
    glBindBufferBase(GL_UNIFORM_BUFFER, ubLightIndex, ubLight);
    glUniformBlockBinding(meshShader->Program, ubLightIndex, ubLightIndex);

    glBindBuffer(GL_UNIFORM_BUFFER, ubLight);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LIGHT_PREFS) * nActiveLights, light, GL_STATIC_DRAW);

    meshShader->Use();
    glUniform1i(glGetUniformLocation(meshShader->Program, "nActiveLights"), nActiveLights);
}

//-----------------------------------------------------------------------------
// Name : InitObjects ()
//-----------------------------------------------------------------------------
void Scene::InitObjects()
{
    Mesh* pMesh = m_assetManager.loadObjMesh("porsche.obj");
    //pMesh->CalcVertexNormals(90.0f);
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(0.0f, 0.0f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(0.5f, 0.5f, 0.5f), // scale
                           pMesh,
                           meshShaderPath2);

    pMesh = m_assetManager.getMesh("king.fbx");
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(18.5f, -0.5f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f),   // rotation
                           glm::vec3(8.0f, 8.0f, 8.0f),   // scale
                           pMesh,
                           meshShaderPath2);

    curObj = &m_objects[1];

    pMesh = m_assetManager.getMesh("cube.obj");
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(20.6f, 1.5f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f),  // rotation
                           glm::vec3(0.6f, 0.6f, 0.6f),  // scale
                           pMesh,
                           meshShaderPath2);

}

//-----------------------------------------------------------------------------
// Name : Darwing ()
//-----------------------------------------------------------------------------
void Scene::Darwing()
{
    m_lastUsedAttrib.matIndex = -1;
    m_lastUsedAttrib.shaderIndex = "";
    m_lastUsedAttrib.texIndex = "";

    meshShader->Use();
    glm::vec3 eye = m_camera.GetPosition();
    glUniform3f(glGetUniformLocation(meshShader->Program, "vecEye"), eye.x, eye.y, eye.z);

    auto attribVector = m_assetManager.getAttributeVector();

    //TODO: optmize this in the camera class
    glm::mat4x4 projViewMat = m_camera.GetProjMatrix() * m_camera.GetViewMatrix();
    for (GLuint i = 0; i < attribVector.size(); i++)
    {
        for (Object obj : m_objects)
        {
            SetAttribute(attribVector[i]);

            //obj.Draw(meshShader, i, projViewMat);
            obj.Draw(projectionLoc, matWorldLoc, matWorldInverseLoc, i, projViewMat);
        }
    }
}

//TODO: searching each time for pointers is slow
//      consider saving them with the attribute indexes
//-----------------------------------------------------------------------------
// Name : SetAttribute ()
//-----------------------------------------------------------------------------
void Scene::SetAttribute(Attribute &attrib)
{
    // set the attributes that have changed
    // this check might be too cotly as this is in the render loop
    if (attrib.shaderIndex != m_lastUsedAttrib.shaderIndex)
    {
        m_assetManager.getShader(attrib.shaderIndex)->Use();
    }

    if (attrib.texIndex != m_lastUsedAttrib.texIndex)
    {
        glBindTexture(GL_TEXTURE_2D, m_assetManager.getTexture(attrib.texIndex));
    }

    if (attrib.matIndex != m_lastUsedAttrib.matIndex)
    {
        Material& mat = m_assetManager.getMaterial(attrib.matIndex);
        glBindBuffer(GL_UNIFORM_BUFFER, ubMaterial);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), &mat, GL_DYNAMIC_DRAW);
        m_lastUsedAttrib = attrib;
    }
}

//-----------------------------------------------------------------------------
// Name : reshape ()
//-----------------------------------------------------------------------------
void Scene::reshape(int width, int height)
{
    m_camera.SetViewPort(0.0f, 0.0f, width, height, 1.0f, 1000.0f);
}

//-----------------------------------------------------------------------------
// Name : processInput ()
//-----------------------------------------------------------------------------
void Scene::processInput(float timeDelta, bool keysStatus[], float X, float Y)
{
    GLuint direction = 0;

    if (keysStatus[GK_UP])
    {
        direction |= Camera::DIR_FORWARD;
    }

    if (keysStatus[GK_DOWN])
    {
        direction |= Camera::DIR_BACKWARD;
    }

    if (keysStatus[GK_LEFT])
    {
        direction |= Camera::DIR_LEFT;
    }

    if (keysStatus[GK_RIGHT])
    {
        direction |= Camera::DIR_RIGHT;
    }

    m_camera.Move(direction, 10* timeDelta);

    float x = 0,y = 0,z = 0;
    if (keysStatus[GK_D])
    {
        //x += 0.001f;
        x -= 10.0f;
    }

    if (keysStatus[GK_A])
    {
        //x -= 0.001f;
        x += 10.0f;
    }

    if (keysStatus[GK_W])
    {
        //y += 0.001f;
        y += 10.0f;
    }

    if (keysStatus[GK_S])
    {
        //y -= 0.001f;
        y -= 10.0f;
    }

    if (keysStatus[GK_Z])
    {
        //z += 0.001f;
        z += 10.0f;
    }

    if (keysStatus[GK_X])
    {
        //z -= 0.001f;
        z -= 10.0f;
    }

    curObj->TranslatePos(x * timeDelta,y * timeDelta,z * timeDelta);
    //m_objects[1].TranslatePos(x * timeDelta,y * timeDelta,z * timeDelta);

    if (keysStatus[GK_Q])
    {
        curObj->Rotate(-(glm::pi<float>() / 4) * timeDelta, 0.0f, 0.0f);
        //m_objects[1].Rotate(-(glm::pi<float>() / 4) * timeDelta, 0.0f, 0.0f);
    }

    if (keysStatus[GK_E])
    {
        curObj->Rotate(glm::pi<float>() / 4 * timeDelta, 0.0f, 0.0f);
        //m_objects[1].Rotate(glm::pi<float>() / 4 * timeDelta, 0.0f, 0.0f);
    }

    if (X != 0.0f || Y != 0.0)
        if (X || Y)
            m_camera.Rotate(-Y, -X, 0.0f);
}

//-----------------------------------------------------------------------------
// Name : PickObject ()
//-----------------------------------------------------------------------------
//TODO: make the picking process take the subMesh closes to camera first...
Object* Scene::PickObject(Point &cursor, int& faceCount, int& meshIndex)
{
    const glm::mat4x4& matPorj = m_camera.GetProjMatrix();

    glm::vec3 v;
    v.x = ( ( ( 2.0f * cursor.x) / m_camera.GetViewport().width) - 1) / matPorj[0][0];
    v.y = -( ( ( 2.0f * cursor.y) / m_camera.GetViewport().height) - 1) / matPorj[1][1];
    v.z = -1.0f;

    glm::mat4x4 view = m_camera.GetViewMatrix();
    view = glm::inverse(view);

    glm::vec3 rayDir(
          v.x * view[0][0] + v.y * view[1][0] + v.z * view[2][0],   // x
          v.x * view[0][1] + v.y * view[1][1] + v.z * view[2][1],   // y
          v.x * view [0][2] + v.y * view [1][2] + v.z * view[2][2]);// z

    glm::vec3 rayOrigin(view[3][0], view[3][1], view [3][2]);

    for (Object& obj : m_objects)
    {
        //if (!obj.IsObjectHidden())
        {
            glm::mat4x4 worldInverse = glm::inverse(obj.GetWorldMatrix());
            glm::vec3 rayObjOrigin = worldInverse * glm::vec4(rayOrigin, 1.0f);
            glm::vec3 rayObjDir = worldInverse * glm::vec4(rayDir, 0.0f);

            if (obj.GetMesh()->IntersectTriangle(rayObjOrigin, rayObjDir, faceCount, meshIndex))
            {
                curObj = &obj;
                return &obj;
            }
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Name : GetObject ()
//-----------------------------------------------------------------------------
Object& Scene::GetObject(int objIndex)
{
    return m_objects[objIndex];
}
