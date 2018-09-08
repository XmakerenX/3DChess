#ifndef  _SCENE_H
#define  _SCENE_H

#include <vector>
//#include <glm/gtx/intersect.hpp>
#include "AssetManager.h"
#include "FreeCam.h"
#include "Object.h"
#include "input.h"
#include "mouseEventsGame.h"
#include "gameInput.h"

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

    void InitScene();
    virtual void InitObjects();
    void InitCamera(int width, int height);
    void InitLights();

    virtual void Drawing();
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
        return status;
    }


protected:
    std::vector<Object> m_objects;
    AssetManager m_assetManager;
    static const std::string meshShaderPath2;
    Object* curObj;
    
    FreeCam m_camera;
    int m_faceCount;
    int m_meshIndex;
    
    Shader* meshShader;
    // cache for unifrom variable location
    GLuint projectionLoc;
    GLuint matWorldLoc;
    GLuint matWorldInverseLoc;
    GLuint vecEyeLoc;

    GLuint ubMaterialIndex;
    GLuint ubMaterial;

    LIGHT_PREFS light[4];
    int nActiveLights;
    GLuint ubLightIndex;
    GLuint ubLight;

    Attribute m_lastUsedAttrib;
    
    std::string status;
};

#endif  //_SCENE_H
