#include "ccOpenVR.h"

#include <QMessageBox>

ccOpenVR::ccOpenVR()
{
    rendermodels=nullptr;
    session = false;
    fbo = new ccFrameBufferObject;
}

ccOpenVR::~ccOpenVR()
{
    delete fbo;
}

bool ccOpenVR::init()
{
    vr::EVRInitError error=vr::VRInitError_None;
    HMD = vr::VR_Init(&error, vr::VRApplication_Scene);
    if(error != vr::VRInitError_None)
    {
        HMD=nullptr;
        QMessageBox::critical(0,"VR_Init Failed",QString("Unable to init VR runtime: %1").arg(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
        return false;
    }
    rendermodels = (vr::IVRRenderModels*)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
    if(!rendermodels)
    {
        HMD=nullptr;
        vr::VR_Shutdown();
        QMessageBox::critical(0,"VR_Init Failed",QString("Unable to get render model interface: %1").arg(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
        return false;
    }
    return true;
}

void ccOpenVR::SetupCameras(float NearClip, float FarClip)
{
    mat4ProjectionLeft = HMD->GetProjectionMatrix(vr::Eye_Left,NearClip,FarClip,vr::API_OpenGL);
    mat4ProjectionRight = HMD->GetProjectionMatrix(vr::Eye_Right,NearClip,FarClip,vr::API_OpenGL);
    mat4eyePosLeft = HMD->GetEyeToHeadTransform(vr::Eye_Left);
    mat4eyePosRight =  HMD->GetEyeToHeadTransform(vr::Eye_Right);
}

bool ccOpenVR::SetupStereoRenderTargets()
{
    if(!HMD)
    {
        return false;
    }

    HMD->GetRecommendedRenderTargetSize(&RenderWidth,&RenderHeigh);

    if(!fbo->init(RenderWidth,RenderHeigh))
    {
        QMessageBox::critical(0,"OpenVR", "Cannot initialize framebuffer for OpenVR");
        return false;
    }

    return true;
}

ccGLMatrixd HmdMat44toccGLMatd(vr::HmdMatrix44_t m)
{
    ccGLMatrixd mat;
    for(int i=0;i<4;++i)
    {
        mat.setColumn(i,Tuple4Tpl<double>(m.m[i][0],m.m[i][1],m.m[i][2],m.m[i][3]));
    }
    return mat;
}

ccGLMatrixd HmdMat34toccGLMatd(vr::HmdMatrix34_t m)
{
    ccGLMatrixd mat;
    for(int i=0;i<3;++i)
    {
        mat.setColumn(i,Tuple4Tpl<double>(m.m[i][0],m.m[i][1],m.m[i][2],m.m[i][3]));
    }
    mat.setColumn(3,Tuple4Tpl<double>(0.0f,0.0f,0.0f,1.0f));
    return mat;
}
