#include "Camera.h"
#include <cmath>

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Camera::Camera()
    :m_vecRight(1.0f, 0.0f, 0.0f),
     m_vecUp(0.0f, 1.0f, 0.0f),
     m_vecLook(0.0f, 0.0f, 0.0f),
     m_vecPos(0.0f, 0.0f, 0.0f),
     m_mtxView(1.0f),
     m_mtxProj(1.0f)
{
    m_fFOV = 60.0f;
    m_fNearClip = 1.0f;
    m_fFarClip = 100.0f;

    m_bViewDirty = true;
    m_bProjDirty = true;
    m_bFrustumDirty = true;

    m_fPitch = 0.0f;
    m_fRoll = 0.0f;
    m_fYaw = 0.0f;
}

//-----------------------------------------------------------------------------
// Copy Constructor
// TODO: make a proper copy Constructor
//-----------------------------------------------------------------------------
Camera::Camera(const Camera * pCamera)
    :m_vecRight(1.0f, 0.0f, 0.0f),
     m_vecUp(0.0f, 1.0f, 0.0f),
     m_vecLook(0.0f, 0.0f, 0.0f),
     m_vecPos(0.0f, 0.0f, 0.0f),
     m_mtxView(1.0f),
     m_mtxProj(1.0f)
{
    m_fFOV = 60.0f;
    m_fNearClip = 1.0f;
    m_fFarClip = 100.0f;

    m_bViewDirty = true;
    m_bProjDirty = true;
    m_bFrustumDirty = true;

    m_fPitch = 0.0f;
    m_fRoll = 0.0f;
}

//-----------------------------------------------------------------------------
// Name : Destructor
//-----------------------------------------------------------------------------
Camera::~Camera()
{}

//-----------------------------------------------------------------------------
// Name : SetLookAt
//-----------------------------------------------------------------------------
void Camera::SetLookAt(const glm::vec3 &vecLookat)
{
    glm::mat4x4 matrix;
    glm::vec3 up,up2;

    up2 = m_vecUp;
    up = glm::cross(vecLookat, m_vecRight);

    if (up == glm::vec3(0.0f, 0.0f, 0.0f))
        up = up2;

    //matrix = glm::lookAt(m_vecPos, vecLookat, up);
    matrix = glm::lookAt(m_vecPos, vecLookat, m_vecUp);

    //m_vecRight = glm::vec3(matrix[0][1], matrix[1][1], matrix[2][1]);
    m_vecRight = glm::vec3(matrix[0][0], matrix[1][0], matrix[2][0]);
    //m_vecRight = glm::vec3(matrix[1][0], matrix[1][1], matrix[1][2]);
    // WTF ???
    m_vecUp = up2;
    m_vecLook = glm::vec3(matrix[0][2], matrix[1][2], matrix[2][2]);

    m_vecUp = glm::vec3(matrix[0][1], matrix[1][1], matrix[2][1]);
    //m_vecLook = glm::vec3(matrix[2][0], matrix[2][1], matrix[2][2]);

    m_bViewDirty = true;
    m_bFrustumDirty = true;

}
//-----------------------------------------------------------------------------
// Name : UpdateRenderView
//-----------------------------------------------------------------------------
void Camera::UpdateRenderView()
{

}

//-----------------------------------------------------------------------------
// Name : UpdateRenderProj
//-----------------------------------------------------------------------------
void Camera::UpdateRenderProj()
{

}

//-----------------------------------------------------------------------------
// Name : SetViewport
//-----------------------------------------------------------------------------
void Camera::SetViewPort(long left, long top, long width, long height,
                         float nearClip, float farclip)
{
    m_viewPort.x = left;
    m_viewPort.y = top;
    m_viewPort.width = width;
    m_viewPort.height = height;
    m_viewPort.minZ = 0.0f;
    m_viewPort.maxZ = 1.0f;
    m_fNearClip = nearClip;
    m_fFarClip = farclip;

    m_bProjDirty = true;
    m_bFrustumDirty = true;

    //TODO: might make this line optional
    glViewport(left,top, width, height);

}


//-----------------------------------------------------------------------------
// Name : GetProjMatrix
// Desc : Returns the current projection martix
//-----------------------------------------------------------------------------
const glm::mat4x4& Camera::GetProjMatrix()
{
    if (m_bProjDirty)
    {
        float fAspect = (float)m_viewPort.width / (float)m_viewPort.height;

        //m_mtxProj = glm::perspective(1.0f, glm::radians(fAspect), m_fNearClip, m_fFarClip);
        //m_fFOV = 90.0f;
        //m_mtxProj = glm::perspective(glm::radians(m_fFOV), fAspect, m_fNearClip, m_fFarClip);
        m_mtxProj = glm::perspective(glm::radians(m_fFOV), fAspect, 1.0f, 1000.0f);
        m_bProjDirty = false;
    }

    return m_mtxProj;
}

//-----------------------------------------------------------------------------
// Name : GetViewMatrix
// Desc : Returns the current view martix
//-----------------------------------------------------------------------------
const glm::mat4x4& Camera::GetViewMatrix()
{
    static int calls = 0;

    if (m_bViewDirty)
    {
        // make sure that the axis are perpendicular to each other
        // do it every 50 calls as by than error could be noticeable
        if (calls == 50)
        {
            glm::vec3 temp = m_vecLook;
            m_vecLook  = glm::normalize(m_vecLook);
            m_vecRight = glm::cross(m_vecUp, m_vecLook);
            m_vecRight = glm::normalize(m_vecRight);
            m_vecUp    = glm::cross(m_vecLook, m_vecRight);
            m_vecUp    = glm::normalize(m_vecUp);

            calls = 0;
        }

        //m_vecLook = glm::vec3(0.0f, 0.0f, 0.0f);
        //m_vecUp = glm::vec3(0.0, 1.0f, 0.0f);
        //m_mtxView =  glm::lookAt(m_vecPos, m_vecLook, m_vecUp);
        m_mtxView[0][0] = m_vecRight.x;
        m_mtxView[0][1] = m_vecUp.x;
        m_mtxView[0][2] = m_vecLook.x;

        m_mtxView[1][0] = m_vecRight.y;
        m_mtxView[1][1] = m_vecUp.y;
        m_mtxView[1][2] = m_vecLook.y;

        m_mtxView[2][0] = m_vecRight.z;
        m_mtxView[2][1] = m_vecUp.z;
        m_mtxView[2][2] = m_vecLook.z;

        m_mtxView[3][0] =- glm::dot(m_vecPos, m_vecRight);
        m_mtxView[3][1] =- glm::dot(m_vecPos, m_vecUp);
        m_mtxView[3][2] =- glm::dot(m_vecPos, m_vecLook);

        calls++;

        m_bViewDirty = false;
    }

    return m_mtxView;
}

//-----------------------------------------------------------------------------
// Name : SetPosition
//-----------------------------------------------------------------------------
void Camera::SetPostion(const glm::vec3 &position)
{
    m_vecPos = position;
    m_bViewDirty = true;
    m_bFrustumDirty = true;
}
//-----------------------------------------------------------------------------
// Name : BoundsInFrustum()
// Desc : Determine whether or not the box passed is within the frustum.
//-----------------------------------------------------------------------------
bool Camera::BoundsInFrustum(const glm::vec3 &min, const glm::vec3& max)
{
    // make sure the Frustum Planes are updated
    CalcFrustumPlanes();

    glm::vec3 nearPoint, farPoint, normal;

    for (GLuint i = 0; i < 6; i++)
    {
        normal = glm::vec3(m_Frustum[i].a, m_Frustum[i].b, m_Frustum[i].c);

        if (normal.x > 0.0f)
        {
            if (normal.y > 0.0f)
            {
                if (normal.y > 0.0f)
                {
                    nearPoint.x = min.x;
                    nearPoint.y = min.y;
                    nearPoint.z = min.z;
                } // end if normal.z > 0
                else
                {
                    nearPoint.x = min.x;
                    nearPoint.y = min.y;
                    nearPoint.z = max.z;
                } // end if normal.z <= 0

            } // end if normal.y > 0
            else
            {
                if (normal.z > 0.0f)
                {
                    nearPoint.x = min.x;
                    nearPoint.y = max.y;
                    nearPoint.z = min.z;
                } // end if normal.z > 0
                else
                {
                    nearPoint.x = min.x;
                    nearPoint.y = max.y;
                    nearPoint.z = max.z;
                } // end if normal.z <= 0

            } // end if normal.y <= 0

        } // end if normal.x > 0
        else
        {
            if (normal.y > 0.0f)
            {
                if (normal.z > 0.0f)
                {
                    nearPoint.x = max.x;
                    nearPoint.y = min.y;
                    nearPoint.z = min.z;
                } // end if normal.z > 0
                else
                {
                    nearPoint.x = max.x;
                    nearPoint.y = min.y;
                    nearPoint.z = max.z;
                } // end if normal.z <= 0

            }// end if normal.y > 0
            else
            {
                if (normal.z > 0.0f)
                {
                    nearPoint.x = max.x;
                    nearPoint.y = max.y;
                    nearPoint.z = min.z;
                } // end if normal.z > 0
                else
                {
                    nearPoint.x = max.x;
                    nearPoint.y = max.y;
                    nearPoint.z = max.z;
                } // end if normal.z <= 0

            } // end if normal.y <= 0

        }// end if normal.x <= 0

        if (glm::dot(normal, nearPoint) + m_Frustum[i].d > 0.0f)
            return false;

    } // Next plane


    // Is within the frustum
    return true;
}

//-----------------------------------------------------------------------------
// Name : CalcFrustumPlanes
// Desc : Calculate the 6 frustum planes based on the current values.
//-----------------------------------------------------------------------------
void Camera::CalcFrustumPlanes()
{
    if (!m_bFrustumDirty) return;

    glm::mat4x4 viewProj = GetViewMatrix() * GetProjMatrix();

    // Left clipping plane
    m_Frustum[0].a = -(viewProj[0][3] + viewProj[0][0]);
    m_Frustum[0].b = -(viewProj[1][3] + viewProj[1][0]);
    m_Frustum[0].c = -(viewProj[2][3] + viewProj[2][0]);
    m_Frustum[0].d = -(viewProj[3][3] + viewProj[3][0]);

    // Right clipping plane
    m_Frustum[1].a = -(viewProj[0][3] - viewProj[0][0]);
    m_Frustum[1].b = -(viewProj[1][3] - viewProj[1][0]);
    m_Frustum[1].c = -(viewProj[2][3] - viewProj[2][0]);
    m_Frustum[1].d = -(viewProj[3][3] - viewProj[3][0]);

    // Top clipping plane
    m_Frustum[2].a = -(viewProj[0][3] - viewProj[0][1]);
    m_Frustum[2].b = -(viewProj[1][3] - viewProj[1][1]);
    m_Frustum[2].c = -(viewProj[2][3] - viewProj[2][1]);
    m_Frustum[2].d = -(viewProj[3][3] - viewProj[3][1]);

    // Bottom clipping plane
    m_Frustum[3].a = -(viewProj[0][3] + viewProj[0][1]);
    m_Frustum[3].b = -(viewProj[1][3] + viewProj[1][1]);
    m_Frustum[3].c = -(viewProj[2][3] + viewProj[2][1]);
    m_Frustum[3].d = -(viewProj[3][3] + viewProj[3][1]);

    // Near clipping plane
    m_Frustum[4].a = -(viewProj[0][2]);
    m_Frustum[4].a = -(viewProj[1][2]);
    m_Frustum[4].a = -(viewProj[2][2]);
    m_Frustum[4].a = -(viewProj[3][2]);

    // Far clipping plane
    m_Frustum[5].a = -(viewProj[0][3] - viewProj[0][2]);
    m_Frustum[5].a = -(viewProj[1][3] - viewProj[1][2]);
    m_Frustum[5].a = -(viewProj[2][3] - viewProj[2][2]);
    m_Frustum[5].a = -(viewProj[3][3] - viewProj[3][2]);

    for (GLuint i = 0; i < 6; i++)
    {
        float vecSize = m_Frustum[i].a * m_Frustum[i].a +
                         m_Frustum[i].b * m_Frustum[i].b +
                         m_Frustum[i].c * m_Frustum[i].c;

        vecSize = std::sqrt(vecSize);
        m_Frustum[i].a /= vecSize;
        m_Frustum[i].b /= vecSize;
        m_Frustum[i].c /= vecSize;
        m_Frustum[i].d /= vecSize;
    }

    m_bFrustumDirty = false;
}
