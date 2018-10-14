#include "FreeCam.h"
#include <utility>

//-----------------------------------------------------------------------------
// Name : FreeCam (constructor)
//-----------------------------------------------------------------------------
FreeCam::FreeCam()
{

}

//-----------------------------------------------------------------------------
// Name : FreeCam (copy constructor)
//-----------------------------------------------------------------------------
FreeCam::FreeCam(const Camera *pCamera)
{
    SetCameraDetails(pCamera);
}

//-----------------------------------------------------------------------------
// Name : SetCameraDetails()
//-----------------------------------------------------------------------------
void FreeCam::SetCameraDetails(const Camera *pCamera)
{
    if (!pCamera) return;

    m_vecPos = pCamera->GetPosition();
    m_vecRight = pCamera->GetRight();
    m_vecLook = pCamera->GetLook();
    m_vecUp = pCamera->GetUp();
    m_fFOV = pCamera->GetFOV();
    m_fNearClip = pCamera->GetNearClip();
    m_fFarClip = pCamera->GetFarClip();
    m_viewPort = pCamera->GetViewport();

    m_bViewDirty = true;
    m_bProjDirty = true;
    m_bFrustumDirty = true;

    m_rotateMatrix = glm::mat4x4(1.0f);
}

//-----------------------------------------------------------------------------
// Name : Rotate ()
// Desc : Rotate the camera about the cameras local axis
//        perfroms a standard three vector rotation(avoids gimble-lock)
//-----------------------------------------------------------------------------
void FreeCam::Rotate(float x, float y, float z)
{
    glm::mat4x4 mtxRotate(1.0f);
    // save the up vector before the x rotation in order for the y rotation to work correctly
    glm::vec3 upTemp;
    glm::vec3 lookTemp;

    upTemp = m_vecUp;
    lookTemp = m_vecLook;

    if (x != 0)
    {
        m_fPitch += x;

        if (m_fPitch > 89.0f)
        {
            x -= m_fPitch - 89.0f;
            m_fPitch = 89.0f;
        }

        if (m_fPitch < -89.0f)
        {
            x -= m_fPitch + 89.0f;
            m_fPitch = -89.0f;
        }

        // build rotation matrix
        mtxRotate = glm::rotate(mtxRotate, glm::radians(x), m_vecRight);

        m_rotateMatrix = std::move(mtxRotate);

        m_vecLook  = mtxRotate * glm::vec4(m_vecLook, 0.0f);
        m_vecUp    = mtxRotate * glm::vec4(m_vecUp, 0.0f);
        m_vecRight = mtxRotate * glm::vec4(m_vecRight, 0.0f);
    } // end if pitch

    if (y != 0)
    {
        m_fYaw += y;

        if (m_fYaw > 360.0f)
        {
            y -= 360.0f;
            m_fYaw = 360.0f;
        }

        if (m_fYaw < 0.0f)
        {
            y += 360.0f;
            m_fYaw = 0.0f;
        }

        // build rotation matrix
        mtxRotate = glm::rotate(glm::mat4x4(1.0f), glm::radians(y), upTemp);
        m_rotateMatrix *= mtxRotate;

        m_vecLook  = mtxRotate * glm::vec4(m_vecLook, 0.0f);
        m_vecUp    = mtxRotate * glm::vec4(m_vecUp, 0.0f);
        m_vecRight = mtxRotate * glm::vec4(m_vecRight, 0.0f);

    } // end if yaw

    if (z != 0)
    {
        m_fRoll += z;
        if (m_fRoll > 20.0f)
        {
            z -= (m_fRoll -20.0f);
            m_fRoll = 20.0f;
        }

        if (m_fRoll < -20.0f)
        {
            z -= (m_fRoll + 20.0f);
            m_fRoll = -20.0f;
        }

        // build rotation matrix
        mtxRotate = glm::rotate(glm::mat4x4(1.0f), glm::radians(z), m_vecLook);
        m_rotateMatrix *= mtxRotate;

        m_vecUp = mtxRotate * glm::vec4(m_vecUp, 0.0f);
        m_vecRight = mtxRotate * glm::vec4(m_vecRight, 0.0f);


    } // end if roll

    m_vecUp = glm::vec3(0.0f, 1.0f, 0.0f);

    m_vecLook = glm::normalize(m_vecLook);
    m_vecRight = glm::cross(m_vecUp, m_vecLook);
    m_vecRight = glm::normalize(m_vecRight);
    m_vecUp = glm::cross(m_vecLook, m_vecRight);
    m_vecUp = glm::normalize(m_vecUp);

    m_bViewDirty = true;
    m_bFrustumDirty = true;
}

//-----------------------------------------------------------------------------
// Name : RotateAroundPoint ()
//-----------------------------------------------------------------------------
void FreeCam::RotateAroundPoint(glm::vec3 &point, float xDistance, float yDistance,
                                RotationLimits rotLimits)
{
    glm::mat4x4 matrix;
    glm::vec3 vecUp = m_vecUp;

    m_vecPos += m_vecRight * xDistance;
    m_vecPos += m_vecUp * yDistance;

    if (m_vecPos.z < rotLimits.minZ)
        m_vecPos.z = static_cast<float>(rotLimits.minZ);

    if (m_vecPos.z > rotLimits.maxZ)
        m_vecPos.z = static_cast<float>(rotLimits.maxZ);

    if (m_vecPos.x > rotLimits.minX)
        m_vecPos.x = static_cast<float>(rotLimits.minX);

    if (m_vecPos.x > rotLimits.maxX)
        m_vecPos.x = static_cast<float>(rotLimits.maxX);

    matrix = glm::lookAt(m_vecPos,point, vecUp);

    m_vecRight = glm::vec3(matrix[0][0], matrix[1][0], matrix[2][0]);
    m_vecUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_vecLook = glm::vec3(matrix[0][2], matrix[1][2], matrix[2][2]);

    m_bViewDirty = true;
    m_bFrustumDirty = true;
}

//-----------------------------------------------------------------------------
// Name : Move ()
//-----------------------------------------------------------------------------
void FreeCam::Move(GLuint direction, float distance)
{
    m_rightAxis = m_vecRight;
    m_rightAxis.y = 0;

    if (direction & Camera::DIR_FORWARD)
        m_vecPos += m_vecLook * (-distance);

    if (direction & Camera::DIR_BACKWARD)
        m_vecPos += m_vecLook * distance;

    if (direction & Camera::DIR_RIGHT)
        m_vecPos += m_rightAxis * distance;

    if (direction & Camera::DIR_LEFT)
        m_vecPos += m_rightAxis * (-distance);

    if (direction & Camera::DIR_UP)
        m_vecPos += m_vecUp * distance;

    if (direction & Camera::DIR_DOWN)
        m_vecPos += m_vecUp * (-distance);

//    glm::mat4x4 matrix;
//    glm::vec3 vecUp = m_vecUp;

//    //TODO: make the center not be a const point!!!!!
//    matrix = glm::lookAt(m_vecPos, glm::vec3(0,0,0), vecUp);

//    m_vecRight = glm::vec3(matrix[0][0], matrix[1][0], matrix[2][0]);
//    m_vecUp = glm::vec3(0.0f, 1.0f, 0.0f);
//    m_vecLook = glm::vec3(matrix[0][2], matrix[1][2], matrix[2][2]);

    m_bViewDirty = true;
    m_bFrustumDirty = true;

}

//-----------------------------------------------------------------------------
// Name : Yaw ()
//-----------------------------------------------------------------------------
void FreeCam::Yaw(float angle)
{
    glm::mat4x4 yawMatrix(1.0f);

    m_fYaw += angle;
    if (m_fYaw > 2 * glm::pi<float>())
        m_fYaw -= 2 * glm::pi<float>();

    if (m_fYaw < 0.0f)
        m_fYaw += 2 * glm::pi<float>();

    yawMatrix = glm::rotate(yawMatrix, angle, m_vecUp);
    m_vecLook = yawMatrix * glm::vec4(m_vecLook, 0.0f);
    m_vecRight = yawMatrix * glm::vec4(m_vecRight, 0.0f);

    m_vecUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

//-----------------------------------------------------------------------------
// Name : Pitch ()
//-----------------------------------------------------------------------------
void FreeCam::Pitch(float angle)
{
    glm::mat4x4 pitchMatrix(1.0f);

    m_fPitch += angle;
    if (m_fPitch > 2 * glm::pi<float>())
        m_fPitch -= 2* glm::pi<float>();

    if (m_fPitch < 0.0f)
        m_fPitch += 2 * glm::pi<float>();

    pitchMatrix = glm::rotate(pitchMatrix, angle, m_vecRight);
    m_vecLook = pitchMatrix * glm::vec4(m_vecLook, 0.0f);
    m_vecUp = pitchMatrix * glm::vec4(m_vecUp, 0.0f);

    m_vecUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

//-----------------------------------------------------------------------------
// Name : getRotateMatrix ()
//-----------------------------------------------------------------------------
glm::mat4x4& FreeCam::getRotateMatrix()
{
    return m_rotateMatrix;
}
