#include "Object.h"

//-----------------------------------------------------------------------------
// Name : Object (constructor)
//-----------------------------------------------------------------------------
Object::Object()
	:m_mthxWorld(1.0f),
	m_mtxTranslate(1.0f),
	m_mtxRot(1.0f),
	m_mtxScale(1.0f),
    m_rotAngles(0.0f)
{
    m_pMesh = nullptr;
	m_hideObject = false;
	m_worldDirty = false;
}

//-----------------------------------------------------------------------------
// Name : Object (constructor)
//-----------------------------------------------------------------------------
Object::Object::Object(const glm::vec3& pos, const glm::vec3& angle, const glm::vec3& scale, Mesh* pMesh, std::vector<unsigned int> meshAttribute)
	:m_mtxTranslate(1.0f),
	m_mtxScale(1.0f)
{
	SetPos(pos);
	SetRotAngles(angle);
	SetScale(scale);
	// update world matrix
	m_worldDirty = true;
	GetWorldMatrix();
	
	AttachMesh(pMesh);
	SetObjectAttributes(meshAttribute);
	
	m_hideObject = false;
}


//-----------------------------------------------------------------------------
// Name : Object (destructor)
//-----------------------------------------------------------------------------
Object::~Object()
{

}

//-----------------------------------------------------------------------------
// Name : GetWorldMatrix
//-----------------------------------------------------------------------------
//TODO: think of returning a const refernce
glm::mat4x4 Object::GetWorldMatrix()
{
	if (m_worldDirty)
	{
		m_mthxWorld = m_mtxScale * m_mtxRot*  m_mtxTranslate;
		m_worldDirty = false;
	}
	
	return m_mthxWorld;
}

//-----------------------------------------------------------------------------
// Name : IsObjectHidden
//-----------------------------------------------------------------------------
bool Object::IsObjectHidden()
{
    return m_hideObject;
}

//-----------------------------------------------------------------------------
// Name : SetObjectHidden
//-----------------------------------------------------------------------------
void Object::SetObjectHidden(bool newStatus)
{
    m_hideObject = newStatus;
}

//-----------------------------------------------------------------------------
// Name : SetPos
//-----------------------------------------------------------------------------
void Object::SetPos(glm::vec3 newPos)
{
	m_mtxTranslate[3][0] = newPos.x;
	m_mtxTranslate[3][1] = newPos.y;
	m_mtxTranslate[3][2] = newPos.z;
	m_worldDirty = true;
}

//-----------------------------------------------------------------------------
// Name : SetRotAngles
//-----------------------------------------------------------------------------
void Object::SetRotAngles(glm::vec3 newRotAngles)
{
    glm::mat4x4 mtxRotX;
    glm::mat4x4 mtxRotY;
    glm::mat4x4 mtxRotZ;

    m_rotAngles = newRotAngles;

    glm::rotate(mtxRotX,m_rotAngles.x,glm::vec3(1.0f,0.0f,0.0f));
    glm::rotate(mtxRotY,m_rotAngles.y,glm::vec3(0.0f,1.0f,0.0f));
    glm::rotate(mtxRotZ,m_rotAngles.z,glm::vec3(0.0f,0.0f,1.0f));
    m_mtxRot = mtxRotX * mtxRotY * mtxRotZ;
}

//-----------------------------------------------------------------------------
// Name : SetObjectScale
//-----------------------------------------------------------------------------
void Object::SetScale(glm::vec3 newScale)
{
	m_mtxScale = glm::scale(m_mtxScale, newScale);
}


//-----------------------------------------------------------------------------
// Name : Rotate
//-----------------------------------------------------------------------------
void Object::Rotate(float x, float y, float z)
{
    glm::mat4x4 mtxRotX;
    glm::mat4x4 mtxRotY;
    glm::mat4x4 mtxRotZ;

    m_rotAngles.x += x;
    m_rotAngles.y += y;
    m_rotAngles.z += z;

    glm::rotate(mtxRotX,m_rotAngles.x,glm::vec3(1.0f,0.0f,0.0f));
    glm::rotate(mtxRotY,m_rotAngles.y,glm::vec3(0.0f,1.0f,0.0f));
    glm::rotate(mtxRotZ,m_rotAngles.z,glm::vec3(0.0f,0.0f,1.0f));
    m_mtxRot = mtxRotX * mtxRotY * mtxRotZ;
	
	m_worldDirty = true;
}

//-----------------------------------------------------------------------------
// Name : attachMesh
//-----------------------------------------------------------------------------
void Object::AttachMesh(Mesh* pMesh)
{
	m_pMesh = pMesh;
}

//-----------------------------------------------------------------------------
// Name : SetObjectAttributes
//-----------------------------------------------------------------------------
void Object::SetObjectAttributes(std::vector<unsigned int> meshAttribute)
{
	//TODO force moving on the vector paremeter to prevent pointless copying
	m_meshAttributes = meshAttribute;
}

//-----------------------------------------------------------------------------
// Name : AddObjectAttribute
//-----------------------------------------------------------------------------
void Object::AddObjectAttribute(unsigned int attribute)
{
	m_meshAttributes.push_back(attribute);
}

//-----------------------------------------------------------------------------
// Name : Draw
//-----------------------------------------------------------------------------
void Object::Draw(Shader* shader, unsigned int attributeIndex, glm::mat4x4& matViewProj)
{
	for (unsigned int i = 0; i < m_meshAttributes.size(); i++)
	{
		if (m_meshAttributes[i] == attributeIndex)
		{
			glUniformMatrix4fv(glGetUniformLocation(shader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(m_mthxWorld * matViewProj));
			m_pMesh->Draw(i);
			break;
		}
	}
}
