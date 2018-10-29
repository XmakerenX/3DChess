#include "Scene.h"

const std::string Scene::s_meshShaderPath2 = "data/shaders/objectShader4";
//-----------------------------------------------------------------------------
// Name : Scene (constructor)
//-----------------------------------------------------------------------------
Scene::Scene()
{
    m_nActiveLights = 0;
    m_ubLight = 0;

    m_faceCount = -1;
    m_meshIndex = -1;
    
    m_curObj = nullptr;
    m_status = "";
}

//-----------------------------------------------------------------------------
// Name : Scene (destructor)
//-----------------------------------------------------------------------------
Scene::~Scene()
{
    if ( m_ubMaterial != 0)
        glDeleteBuffers(1, &m_ubMaterial );
    
    if ( m_ubLight != 0)
        glDeleteBuffers(1, &m_ubLight );
}

//-----------------------------------------------------------------------------
// Name : InitScene ()
//-----------------------------------------------------------------------------
void Scene::InitScene(int width, int height, const glm::vec3& cameraPosition/* = glm::vec3(0.0f, 20.0f, 70.0f)*/, const glm::vec3& cameraLookat/* = glm::vec3(0.0f, 0.0f, 0.0f)*/)
{
    meshShader =  m_assetManager.getShader( s_meshShaderPath2 );

    m_projectionLoc = glGetUniformLocation(meshShader->Program, "projection");
    m_matWorldLoc = glGetUniformLocation(meshShader->Program, "matWorld");
    m_matWorldInverseLoc = glGetUniformLocation(meshShader->Program, "matWorldInverseT");

    // Init the material unifrom buffer
    m_ubMaterialIndex = glGetUniformBlockIndex(meshShader->Program, "Material");
    glGenBuffers(1, &m_ubMaterial );
    glBindBufferBase(GL_UNIFORM_BUFFER, m_ubMaterialIndex, m_ubMaterial );
    glUniformBlockBinding(meshShader->Program, m_ubMaterialIndex, m_ubMaterialIndex );

    InitLights();
    InitObjects();
    InitCamera(width ,height, cameraPosition, cameraLookat);
}

//-----------------------------------------------------------------------------
// Name : InitCamera()
//-----------------------------------------------------------------------------
void Scene::InitCamera(int width, int height, const glm::vec3& position, const glm::vec3& lookat)
{
    m_camera.SetFOV(60.0f);
    m_camera.SetViewPort(0, 0, width, height, 1.0f, 5000.0f);
    m_camera.SetPostion(position);
    m_camera.SetLookAt(lookat);
}

//-----------------------------------------------------------------------------
// Name : InitLights()
//-----------------------------------------------------------------------------
void Scene::InitLights()
{
    m_light[0].dir = glm::vec4(1.0f, -0.8f, 0.4f, 0.0f);
    m_light[0].dir = glm::vec4(0.1f, -0.3f, 0.4f, 0.0f);
    m_light[0].ambient = glm::vec4(0.4f, 0.4f, 0.4f, 0.4f) * 0.4f;
    m_light[0].ambient.a = 1.0f;
    m_light[0].diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    m_light[0].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    m_light[0].pos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_nActiveLights++;

    m_ubLightIndex = glGetUniformBlockIndex(meshShader->Program, "lightBlock");
    glGenBuffers(1, &m_ubLight );
    glBindBufferBase(GL_UNIFORM_BUFFER, m_ubLightIndex, m_ubLight );
    glUniformBlockBinding(meshShader->Program, m_ubLightIndex, m_ubLightIndex );

    glBindBuffer(GL_UNIFORM_BUFFER, m_ubLight );
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LIGHT_PREFS) * m_nActiveLights, m_light, GL_STATIC_DRAW);

    meshShader->Use();
    glUniform1i(glGetUniformLocation(meshShader->Program, "nActiveLights"), m_nActiveLights );
}

//-----------------------------------------------------------------------------
// Name : InitObjects ()
//-----------------------------------------------------------------------------
void Scene::InitObjects()
{    
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(0.0f, 0.0f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f), // rotation
                           glm::vec3(0.5f, 0.5f, 0.5f), // scale
                           m_assetManager.getMesh("data/models/porsche.obj"),
                           s_meshShaderPath2 );
    
    m_objects.emplace_back(m_assetManager,
                           glm::vec3(18.5f, -0.5f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f),   // rotation
                           glm::vec3(8.0f, 8.0f, 8.0f),   // scale
                           m_assetManager.getMesh("data/models/king.fbx"),
                           s_meshShaderPath2 );

    m_curObj = &m_objects[0];

    m_objects.emplace_back(m_assetManager,
                           glm::vec3(0.f, 0.0f, 0.0f), // position
                           glm::vec3(0.0f, 0.0f, 0.0f),  // rotation
                           glm::vec3(1.0f, 1.0f, 1.0f),  // scale
                           m_assetManager.getMesh("cube.gen"),
                           s_meshShaderPath2);
    
}

//-----------------------------------------------------------------------------
// Name : Darwing ()
//-----------------------------------------------------------------------------
void Scene::Drawing(double frameTimeDelta)
{
    //clear m_lastUsedAttrib
    m_lastUsedAttrib.matIndex = -1;
    // space used for the path as no valid path should include only a space
    m_lastUsedAttrib.shaderIndex = " ";
    m_lastUsedAttrib.texIndex = " ";

    // sets the camera position in the shader
    // TODO: this call should only happen if the camera had moved since previous frame
    // should add a flag to represent that.
    meshShader->Use();
    glm::vec3 eye = m_camera.GetPosition();
    glUniform3f(glGetUniformLocation(meshShader->Program, "vecEye"), eye.x, eye.y, eye.z);

    auto attribVector = m_assetManager.getAttributeVector();

    //TODO: optmize this in the camera class
    glm::mat4x4 projViewMat = m_camera.GetProjMatrix() * m_camera.GetViewMatrix();
    for (GLuint i = 0; i < attribVector.size(); i++)
    {
        SetAttribute(attribVector[i]);
        for (Object obj : m_objects)
        {
            obj.Draw( m_projectionLoc, m_matWorldLoc, m_matWorldInverseLoc, i, projViewMat);
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
        
        if (attrib.texIndex == "")
            glUniform1i(glGetUniformLocation(m_assetManager.getShader(attrib.shaderIndex)->Program, "textured"), 0);
        else
        {
            glBindTexture(GL_TEXTURE_2D, m_assetManager.getTexture(attrib.texIndex));
            // set texture warp mode
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, attrib.wrapMode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, attrib.wrapMode);
            
            glUniform1i(glGetUniformLocation(m_assetManager.getShader(attrib.shaderIndex)->Program, "textured"), 1);
        }
    }
    
    if (attrib.matIndex != m_lastUsedAttrib.matIndex)
    {
        Material& mat = m_assetManager.getMaterial(attrib.matIndex);
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubMaterial );
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), &mat, GL_DYNAMIC_DRAW);
    }
    
    m_lastUsedAttrib = attrib;
}

//-----------------------------------------------------------------------------
// Name : reshape ()
//-----------------------------------------------------------------------------
void Scene::reshape(int width, int height)
{
    m_camera.SetViewPort(0, 0, width, height, 1.0f, 5000.0f);
}

//-----------------------------------------------------------------------------
// Name : processInput ()
//-----------------------------------------------------------------------------
void Scene::processInput(double timeDelta, bool keysStatus[], float X, float Y)
{
    GLuint direction = 0;

    if (keysStatus[static_cast<int>(GK_VirtualKey::GK_Up)])
        direction |= Camera::DIR_FORWARD;

    if (keysStatus[static_cast<int>(GK_VirtualKey::GK_Down)])
        direction |= Camera::DIR_BACKWARD;

    if (keysStatus[static_cast<int>(GK_VirtualKey::GK_Left)])
        direction |= Camera::DIR_LEFT;

    if (keysStatus[static_cast<int>(GK_VirtualKey::GK_Right)])
        direction |= Camera::DIR_RIGHT;

    m_camera.Move(direction, 10 * timeDelta);

    float x = 0,y = 0,z = 0;
    if (keysStatus[GK_D])
        x += 10.0f;

    if (keysStatus[GK_A])
        x -= 10.0f;

    if (keysStatus[GK_W])
        y += 10.0f;

    if (keysStatus[GK_S])
        y -= 10.0f;

    if (keysStatus[GK_Z])
        z += 10.0f;

    if (keysStatus[GK_X])
        z -= 10.0f;

    m_curObj->TranslatePos(x * timeDelta, y * timeDelta, z * timeDelta);

    if (keysStatus[GK_Q])
        m_curObj->Rotate(-(glm::pi<float>() / 4) * timeDelta, 0.0f, 0.0f);

    if (keysStatus[GK_E])
        m_curObj->Rotate(glm::pi<float>() / 4 * timeDelta, 0.0f, 0.0f);

    if (X != 0.0f || Y != 0.0)
        if (X || Y)
            m_camera.Rotate(-Y, -X, 0.0f);
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent ()
//-----------------------------------------------------------------------------
bool Scene::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    switch(event.type)
    {
        case MouseEventType::RightButton:
        {
            Object* temp = PickObject(event.cursorPos, m_faceCount, m_meshIndex);
            if(temp != nullptr)
                m_curObj = temp;
            
            return true;
        }break;
        
        default:
            return false;
    }
    
    return false;
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
                m_curObj = &obj;
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

//-----------------------------------------------------------------------------
// Name : getFaceCount ()
//-----------------------------------------------------------------------------
int Scene::getFaceCount()
{
    return m_faceCount;
}

//-----------------------------------------------------------------------------
// Name : getMeshIndex ()
//-----------------------------------------------------------------------------
int Scene::getMeshIndex()
{
    return m_meshIndex;
}
