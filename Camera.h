#ifndef  _CAMERA_H
#define  _CAMERA_H

#include<GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
    //-----------------------------------------------------------------------------
    // Enumerators and Structures
    //-----------------------------------------------------------------------------
    enum CAMERA_MODE {
        MODE_FIRST       = 1,
        MODE_FREE        = 2,
        MODE_THIRD       = 3,
    };

    enum DIRECTION{
        DIR_FORWARD     = 1,
        DIR_BACKWARD    = 2,
        DIR_LEFT        = 4,
        DIR_RIGHT       = 8,
        DIR_UP          = 16,
        DIR_DOWN        = 32,

        DIR_FORCE_32BIT = 0x7FFFFFFF
    };

    struct VIEWPORT
    {
        VIEWPORT()
        {
            x       = 0;
            y       = 0;
            width   = 0;
            height  = 0;
            minZ    = 0;
            maxZ    = 1;
        }

        GLuint x;
        GLuint y;
        GLuint width;
        GLuint height;
        float minZ;
        float maxZ;
    };

    struct PLANE3D
    {
      float a;
      float b;
      float c;
      float d;
    };

    //-----------------------------------------------------------------------------
    // Constructors & Destructors
    //-----------------------------------------------------------------------------
    Camera();
    Camera(const Camera * pCamera);
    virtual ~Camera();

    //-----------------------------------------------------------------------------
    // Public Functions
    //-----------------------------------------------------------------------------
    void SetFOV     (float FOV) {m_fFOV = FOV; m_bProjDirty = true;}
    void SetViewPort(long left, long top, long width, long height,
                     float nearClip, float farclip);
    void SetLookAt  (const glm::vec3& vecLookat);

    void UpdateRenderView               ();
    void UpdateRenderProj               ();
    const glm::mat4x4& GetProjMatrix    ();

    float GetFOV                        () const {return m_fFOV;}
    float GetNearClip                   () const {return m_fNearClip;}
    float GetFarClip                    () const {return m_fFarClip;}
    const VIEWPORT& GetViewport ( ) const { return m_viewPort; }

    const glm::vec3 GetPosition         () const {return m_vecPos;}
    const glm::vec3 GetLook             () const {return m_vecLook;}
    const glm::vec3 GetUp               () const {return m_vecUp;}
    const glm::vec3 GetRight            () const {return m_vecRight;}
    const glm::mat4x4& GetViewMatrix    ();

    //-----------------------------------------------------------------------------
    // Public Virtual Functions
    //-----------------------------------------------------------------------------
    virtual void SetPostion(const glm::vec3& position);
    virtual void Move      (GLuint direction, float distance) = 0;
    virtual void Rotate    (float x, float y, float z) = 0;
    //virtual void Update    (float timeScale, float lag) = 0;
    virtual void SetCameraDetails (const Camera* pCamera) = 0;

    virtual CAMERA_MODE GetCameraMode() const = 0;

    bool BoundsInFrustum( const glm::vec3& min, const glm::vec3& max);

protected:
    //-----------------------------------------------------------------------------
    // Protected Functions
    //-----------------------------------------------------------------------------
    void CalcFrustumPlanes();

    //-----------------------------------------------------------------------------
    // Protected Variables
    //-----------------------------------------------------------------------------
    glm::mat4x4 m_mtxView; // Cached view matrix
    glm::mat4x4 m_mtxProj; // Cached projection matrix
    PLANE3D m_Frustum[6];
    //D3DXPLANE m_Frustum[6]; // The 6 planes of our frustum.

    bool m_bViewDirty;
    bool m_bProjDirty;
    bool m_bFrustumDirty;

    // Perspective Projection parameters
    float m_fFOV;
    float m_fNearClip;
    float m_fFarClip;
    VIEWPORT m_viewPort;
    //D3DVIEWPORT9 m_Viewport; // The viewport details into which we are rendering.

    // Camera current position & orientation
    glm::vec3 m_vecPos;
    glm::vec3 m_vecUp;
    glm::vec3 m_vecLook;
    glm::vec3 m_vecRight;

    float m_fPitch;
    float m_fRoll;
    float m_fYaw;
};

#endif  //_CAMERA_H
