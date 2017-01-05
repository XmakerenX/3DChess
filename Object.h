#ifndef  _OBJECT_H
#define _OBJECT_H

#include "Mesh.h"

class Object
{
public:
    Object          ();
    ~Object         ();

    glm::mat4x4 GetWorldMatrix  ();

    bool        IsObjectHidden  ();
    void        SetObjectHidden (bool newStatus);
    void        SetPos          (glm::vec3 newPos);
    void        SetRotAngles    (glm::vec3 newRotAngles);
    void        Rotate          (float x, float y, float z);

private:
    glm::mat4x4 mtxWorld_;
    glm::mat4x4 mtxRot_;
    glm::mat4x4 mtxScale_;
    glm::vec3   rotAngles_;

    Mesh*       pMesh_;
    bool        hideObject_;
};

#endif  //_OBJECT_H
