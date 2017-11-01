#ifndef  _OBJECT_H
#define _OBJECT_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Mesh.h"
#include "AssetManager.h"

class Object
{
public:
    Object          ();
	Object          (const glm::vec3& pos, const glm::vec3& angle, const glm::vec3& scale, Mesh* pMesh, std::vector<unsigned int> meshAttribute);
    Object          (AssetManager& asset, const glm::vec3& pos, const glm::vec3& angle, const glm::vec3& scale, Mesh* pMesh, std::string shaderPath);
    void InitObject (const glm::vec3& pos, const glm::vec3& angle, const glm::vec3& scale, Mesh* pMesh, std::vector<unsigned int> meshAttribute);
    ~Object         ();

    glm::mat4x4 GetWorldMatrix  ();
    glm::vec3   GetPosition     ();
    Mesh*       GetMesh         ();

    bool        IsObjectHidden  ();
    void        SetObjectHidden (bool newStatus);
    void        SetPos          (glm::vec3 newPos);
    void        SetRotAngles    (glm::vec3 newRotAngles);
	void 		SetScale  (glm::vec3 newScale);

    void        Rotate          (float x, float y, float z);
    void        TranslatePos    (float x ,float y, float z);
	
	void 		AttachMesh		(Mesh* pMesh);
	void 		SetObjectAttributes(std::vector<unsigned int> meshAttribute);
	void 		AddObjectAttribute(unsigned int attribute);
	
	void 		Draw			(Shader* shader, unsigned int attributeIndex, const glm::mat4x4 &matViewProj);

private:
    //glm::mat4x4 m_mtxTranslate;
    glm::vec3   m_pos;
    glm::mat4x4 m_mthxWorld;
    glm::mat4x4 m_mtxRot;
    glm::mat4x4 m_mtxScale;
    glm::vec3   m_rotAngles;

	std::vector<unsigned int> m_meshAttributes;
	
    Mesh*       m_pMesh;
    bool        m_hideObject;
	
	bool 		m_worldDirty;
};

#endif  //_OBJECT_H
