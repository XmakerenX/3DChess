#include "Object.h"

//-----------------------------------------------------------------------------
// Name : Object (constructor)
//-----------------------------------------------------------------------------
Object::Object()
    :rotAngles_(0.0f)
{
    pMesh_ = nullptr;
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
glm::mat4x4 Object::GetWorldMatrix()
{
    return mtxScale_ * mtxRot_*  mtxWorld_;
}

//-----------------------------------------------------------------------------
// Name : IsObjectHidden
//-----------------------------------------------------------------------------
bool Object::IsObjectHidden()
{
    return hideObject_;
}

//-----------------------------------------------------------------------------
// Name : SetObjectHidden
//-----------------------------------------------------------------------------
void Object::SetObjectHidden(bool newStatus)
{
    hideObject_ = newStatus;
}

//-----------------------------------------------------------------------------
// Name : SetPos
//-----------------------------------------------------------------------------
void Object::SetPos(glm::vec3 newPos)
{

}

//-----------------------------------------------------------------------------
// Name : SetRotAngles
//-----------------------------------------------------------------------------
void Object::SetRotAngles(glm::vec3 newRotAngles)
{
    glm::mat4x4 mtxRotX;
    glm::mat4x4 mtxRotY;
    glm::mat4x4 mtxRotZ;

    rotAngles_ = newRotAngles;

    glm::rotate(mtxRotX,rotAngles_.x,glm::vec3(1.0f,0.0f,0.0f));
    glm::rotate(mtxRotY,rotAngles_.y,glm::vec3(0.0f,1.0f,0.0f));
    glm::rotate(mtxRotZ,rotAngles_.z,glm::vec3(0.0f,0.0f,1.0f));
    mtxRot_ = mtxRotX * mtxRotY * mtxRotZ;
}

//-----------------------------------------------------------------------------
// Name : Rotate
//-----------------------------------------------------------------------------
void Object::Rotate(float x, float y, float z)
{
    glm::mat4x4 mtxRotX;
    glm::mat4x4 mtxRotY;
    glm::mat4x4 mtxRotZ;

    rotAngles_.x += x;
    rotAngles_.y += y;
    rotAngles_.z += z;

    glm::rotate(mtxRotX,rotAngles_.x,glm::vec3(1.0f,0.0f,0.0f));
    glm::rotate(mtxRotY,rotAngles_.y,glm::vec3(0.0f,1.0f,0.0f));
    glm::rotate(mtxRotZ,rotAngles_.z,glm::vec3(0.0f,0.0f,1.0f));
    mtxRot_ = mtxRotX * mtxRotY * mtxRotZ;
}
