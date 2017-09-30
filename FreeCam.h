#ifndef  _FREECAM_H
#define  _FREECAM_H

#include "Camera.h"

class FreeCam : public Camera
{
public:
    struct RotationLimits
    {
        int minX;
        int maxX;
        int minZ;
        int maxZ;
    };

    //-----------------------------------------------------------------------------
    // Constructors & Destructors
    //-----------------------------------------------------------------------------
    FreeCam();
    FreeCam(const Camera* pCamera);

    //-----------------------------------------------------------------------------
    // Public Functions for This Class.
    //-----------------------------------------------------------------------------
    CAMERA_MODE GetCameraMode() const { return MODE_FREE;}
    void        Rotate       (float x, float y, float z);
    void        RotateAroundPoint(glm::vec3& point, float xDistance, float yDistance,
                                  RotationLimits rotLimits);
    void        SetCameraDetails(const Camera* pCamera);
    void        Move(GLuint direction, float distance);
    void        Yaw(float angle);
    void        Pitch(float angle);
    glm::mat4x4 &getRotateMatrix();

private:
    glm::vec3 m_rightAxis;
    glm::mat4x4 m_rotateMatrix;
};

#endif  //_FREECAM_H
