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

private:

    vr::IVRSystem *HMD;
    vr::IVRRenderModels *rendermodels;
    uint32_t RenderWidth,RenderHeigh;
    vr::HmdMatrix44_t mat4ProjectionLeft,mat4ProjectionRight;
    vr::HmdMatrix34_t mat4eyePosLeft,mat4eyePosRight;

};

#endif // CCOPENVR_H
