#ifndef  _SCENE_H
#define  _SCENE_H

#include <vector>
#include "AssetLoading/AssetManager.h"
#include "FreeCam.h"
#include "Object.h"
#include "input.h"
#include "mouseEventsGame.h"

struct LIGHT_PREFS
{
    LIGHT_PREFS()
        :pos(0.0f, 0.0f, 0.0f),
        dir(0.0f, 0.0f, 0.0f, 0.0f),
        attenuation(0.0f, 0.0f, 0.0f),
        ambient(0.0f, 0.0f, 0.0f, 0.0f),
        diffuse(0.0f, 0.0f, 0.0f, 0.0f),
        specular(0.0f, 0.0f, 0.0f, 0.0f),
        outerCutoff(0.0f)
    {}

    LIGHT_PREFS(glm::vec3& newPos, glm::vec4& newDir, glm::vec3& newAtten, glm::vec4& newAmbient,
                glm::vec4 newDiffuse, glm::vec4 newSpecular, float newSpecPower)
        :pos(newPos),
         dir(newDir),
         attenuation(newAtten),
         ambient(newAmbient),
         diffuse(newDiffuse),
         specular(newSpecular),
         outerCutoff(newSpecPower)
    {}

    glm::vec3 pos;
    float pad; //padding to fit the glsl struct

    glm::vec4 dir;
    glm::vec3 attenuation;
    float pad2; //padding to fit the glsl struct

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    float outerCutoff;
    float innerCutoff;
    glm::vec2 pad3; //padding to fit the glsl struct

};

class Scene
{

public:
    Scene();
    virtual ~Scene();

    virtual void InitScene(int width, int height, const glm::vec3& cameraPosition = glm::vec3(0.0f, 20.0f, 70.0f), const glm::vec3& cameraLookat = glm::vec3(0.0f, 0.0f, 0.0f));
    virtual void InitObjects();
    void InitCamera(int width, int height, const glm::vec3& position, const glm::vec3& lookat);
    void InitLights();

    virtual void Drawing(double frameTimeDelta);
    void SetAttribute(Attribute& attrib);

    void reshape(int width, int height);
    void processInput (double timeDelta, bool keysStatus[], float X, float Y);
    virtual bool handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);

    virtual Object *PickObject(Point& cursor, int& faceCount, int &meshIndex);
    Object& GetObject(int objIndex);
    
    int getFaceCount();
    int getMeshIndex();
    
    std::string& getStatus()
    {
        return m_status;
    }


protected:
    std::vector<Object> m_objects;
    AssetManager m_assetManager;
    static const std::string s_meshShaderPath2;
    Object* m_curObj;
    
    FreeCam m_camera;
    int m_faceCount;
    int m_meshIndex;
    
    Shader* meshShader;
    // cache for unifrom variable location
    GLuint m_projectionLoc;
    GLuint m_matWorldLoc;
    GLuint m_matWorldInverseLoc;
    GLuint m_vecEyeLoc;

    GLuint m_ubMaterialIndex;
    GLuint m_ubMaterial;

    LIGHT_PREFS m_light[4];
    int m_nActiveLights;
    GLuint m_ubLightIndex;
    GLuint m_ubLight;

    Attribute m_lastUsedAttrib;
    
    std::string m_status;
};

#endif  //_SCENE_H
