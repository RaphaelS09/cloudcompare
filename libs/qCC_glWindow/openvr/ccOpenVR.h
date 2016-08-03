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

    bool SetupStereoRenderTargets(QOpenGLContext *context);

    bool initCompositor();

    bool session;

    ccFrameBufferObject *fboLeft, *fboRight;
    uint32_t RenderWidth,RenderHeigh;



private:

    bool SetupRenderTarget(ccFrameBufferObject *fbo, QOpenGLContext *context);

    vr::IVRSystem *HMD;
    vr::IVRRenderModels *rendermodels;
    vr::HmdMatrix44_t mat4ProjectionLeft,mat4ProjectionRight;
    vr::HmdMatrix34_t mat4eyePosLeft,mat4eyePosRight;


};

#endif // CCOPENVR_H
