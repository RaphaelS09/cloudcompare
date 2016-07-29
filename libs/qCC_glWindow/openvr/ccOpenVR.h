#ifndef CCOPENVR_H
#define CCOPENVR_H

//qCC_db
#include <ccIncludeGL.h>
#include <ccLog.h>

//CCLib
#include <CCGeom.h>

//CCFbo
#include <ccFrameBufferObject.h>

//OpenVR
#include <openvr.h>

//system
#include <vector>

#include <QOpenGLFunctions_2_1>
#include <QOpenGLExtensions>

ccGLMatrixd HmdMat44toccGLMatd(vr::HmdMatrix44_t m);
ccGLMatrixd HmdMat34toccGLMatd(vr::HmdMatrix34_t m);

class ccOpenVR
{
public:
    ccOpenVR();
    ~ccOpenVR();

    bool init();

    uint32_t getRecommendedRenderWidth() const
    {
        return RenderWidth;
    }

    uint32_t getRecommendedRenderHeigh() const
    {
        return RenderHeigh;
    }

    void SetupCameras(float NearClip, float FarClip);

    bool SetupStereoRenderTargets();

    ccFrameBufferObject *fbo;
    bool session;

private:

    vr::IVRSystem *HMD;
    vr::IVRRenderModels *rendermodels;
    uint32_t RenderWidth,RenderHeigh;
    vr::HmdMatrix44_t mat4ProjectionLeft,mat4ProjectionRight;
    vr::HmdMatrix34_t mat4eyePosLeft,mat4eyePosRight;

    struct FramebufferDesc
    {
        GLuint m_nDepthBufferId;
        GLuint m_nRenderTextureId;
        GLuint m_nRenderFramebufferId;
        GLuint m_nResolveTextureId;
        GLuint m_nResolveFramebufferId;
    };

    FramebufferDesc leftEyeDesc;
    FramebufferDesc rightEyeDesc;

    bool CreateFrameBuffer(uint32_t w, uint32_t h, FramebufferDesc buf);

    QOpenGLFunctions_2_1 func;
    QOpenGLExtension_ARB_framebuffer_object extfunc;

};

#endif // CCOPENVR_H
